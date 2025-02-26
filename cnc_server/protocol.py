import struct
import enum
import asyncio
from dataclasses import dataclass
from typing import Optional, Any, Type

class RequestType(enum.IntEnum):
    STATUS = 1
    COMMAND_RESULT = 2

class ResponseType(enum.IntEnum):
    KEEP_ALIVE = 1
    EXECUTE_COMMANDS = 2

@dataclass
class BaseRequest:
    request_id: int
    client_id: int
    request_type: RequestType

@dataclass
class StatusRequest(BaseRequest):
    timestamp: str

@dataclass
class CommandResultRequest(BaseRequest):
    command_id: int
    result: str

@dataclass
class BaseResponse:
    request_id: int
    client_id: int
    response_type: ResponseType

@dataclass
class KeepAliveResponse(BaseResponse):
    timestamp: str

@dataclass
class ExecuteCommandsResponse(BaseResponse):
    commands: str

class ProtocolError(Exception):
    pass

class RequestParser:
    HEADER_FORMAT = "!II"  # Two uint32: request_id, client_id
    HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

    @classmethod
    async def read_exact(cls, reader: asyncio.StreamReader, size: int) -> bytes:
        """Read exactly size bytes from the reader."""
        data = await reader.read(size)
        if len(data) < size:
            raise ProtocolError("Connection closed while reading")
        return data

    @classmethod
    async def parse_header(cls, reader: asyncio.StreamReader) -> tuple[int, int]:
        """Parse the common header fields."""
        header_data = await cls.read_exact(reader, cls.HEADER_SIZE)
        return struct.unpack(cls.HEADER_FORMAT, header_data)

    @classmethod
    async def parse_string(cls, reader: asyncio.StreamReader) -> str:
        """Parse a length-prefixed string."""
        length_data = await cls.read_exact(reader, 4)
        length = struct.unpack("!I", length_data)[0]
        if length > 1024 * 1024:  # 1MB limit
            raise ProtocolError("String too long")
        string_data = await cls.read_exact(reader, length)
        return string_data.decode('utf-8')

    @classmethod
    async def parse_request(cls, reader: asyncio.StreamReader) -> BaseRequest:
        """Parse a request from the reader."""
        request_id, client_id = await cls.parse_header(reader)
        
        # Read request type
        type_data = await cls.read_exact(reader, 4)
        request_type = RequestType(struct.unpack("!I", type_data)[0])
        
        # Create base request
        base = BaseRequest(request_id, client_id, request_type)
        
        # Parse specific request type
        if request_type == RequestType.STATUS:
            timestamp = await cls.parse_string(reader)
            return StatusRequest(base.request_id, base.client_id, base.request_type, timestamp)
        elif request_type == RequestType.COMMAND_RESULT:
            command_id_data = await cls.read_exact(reader, 4)
            command_id = struct.unpack("!I", command_id_data)[0]
            result = await cls.parse_string(reader)
            return CommandResultRequest(base.request_id, base.client_id, base.request_type, command_id, result)
        else:
            raise ProtocolError(f"Unknown request type: {request_type}")

class ResponseSerializer:
    @classmethod
    def serialize_string(cls, s: str) -> bytes:
        """Serialize a string with length prefix."""
        encoded = s.encode('utf-8')
        return struct.pack("!I", len(encoded)) + encoded

    @classmethod
    def serialize_response(cls, response: BaseResponse) -> bytes:
        """Serialize a response to bytes."""
        # Common header
        data = struct.pack("!III",
            response.request_id,
            response.client_id,
            response.response_type.value
        )
        
        # Type-specific fields
        if isinstance(response, KeepAliveResponse):
            data += cls.serialize_string(response.timestamp)
        elif isinstance(response, ExecuteCommandsResponse):
            data += cls.serialize_string(response.commands)
        else:
            raise ProtocolError(f"Unknown response type: {type(response)}")
            
        return data

async def write_response(writer: asyncio.StreamWriter, response: BaseResponse):
    """Write a response to the stream."""
    data = ResponseSerializer.serialize_response(response)
    writer.write(data)
    await writer.drain()
