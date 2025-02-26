import abc
import ctypes
import struct
import enum
import asyncio
from dataclasses import dataclass
from typing import Optional, Any

class RequestType(enum.IntEnum):
    KEEP_ALIVE = 0
    RETURN_PRODUCTS = 1

class ResponseType(enum.IntEnum):
    KEEP_ALIVE = 0
    EXECUTE_COMMANDS = 1

async def read_exact(reader: asyncio.StreamReader, size: int) -> bytes:
    data = await reader.read(size)
    if len(data) < size:
        raise ProtocolError("Connection closed while reading")
    return data

@dataclass
class RequestHeader:
    request_type: RequestType
    client_id: int

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "RequestHeader":
        FORMAT = "<II"
        header_data = await read_exact(reader, struct.calcsize(FORMAT))
        return RequestHeader(*struct.unpack(FORMAT, header_data))

@dataclass
class ReturnProductsRequestData:
    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "RequestHeader":
        raise NotImplemented()

@dataclass
class Request:
    header: RequestHeader
    data: Optional[Any] = None

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "RequestHeader":
        header = await RequestHeader.from_stream(reader)

        if header.request_type == RequestType.KEEP_ALIVE:
            return Request(header)
        elif header.request_type == RequestType.RETURN_PRODUCTS:
            return Request(header, await ReturnProductsRequestData.from_stream(reader))

        raise ProtocolError(f"Invalid request type: {header.request_type}")

class Response(abc.ABC):
    @abc.abstractmethod
    def to_raw(self) -> bytes:
        pass

@dataclass
class KeepAliveResponse(Response):
    def to_raw(self) -> bytes:
        return struct.pack("<I", ResponseType.KEEP_ALIVE)

@dataclass
class ExecuteCommandsResponse(Response):
    def to_raw(self) -> bytes:
        raise NotImplemented()


class ProtocolError(Exception):
    pass

async def write_response(writer: asyncio.StreamWriter, response: Response):
    data = response.to_raw()
    writer.write(data)
    await writer.drain()
