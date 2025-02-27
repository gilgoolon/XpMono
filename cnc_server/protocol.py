import abc
import enum
import secrets
import struct
import asyncio
from dataclasses import dataclass
from typing import Optional, Any, List

import structs

class RequestType(enum.IntEnum):
    KEEP_ALIVE = 0
    RETURN_PRODUCTS = 1

class ResponseType(enum.IntEnum):
    KEEP_ALIVE = 0
    EXECUTE_COMMANDS = 1

@dataclass
class RequestHeader:
    request_type: RequestType
    client_id: int

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "RequestHeader":
        return cls(*await structs.read_struct(reader, "<II"))

@dataclass
class Product:
    product_id: int
    data: bytes

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "Product":
        product_id, = await structs.read_struct(reader, "<I")
        data = await structs.read_sized_buffer(reader)
        return cls(product_id, data)

@dataclass
class ReturnProductsRequestData:
    products: List[Product]

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "ReturnProductsRequestData":
        products_count, = await structs.read_struct(reader, "<I")
        products = [Product.from_stream(reader) for _ in range(products_count)]
        return cls(products)

@dataclass
class Request:
    header: RequestHeader
    data: Optional[Any] = None

    @classmethod
    async def from_stream(cls, reader: asyncio.StreamReader) -> "Request":
        header = await RequestHeader.from_stream(reader)

        if header.request_type == RequestType.KEEP_ALIVE:
            return cls(header)
        elif header.request_type == RequestType.RETURN_PRODUCTS:
            return cls(header, await ReturnProductsRequestData.from_stream(reader))

        raise ProtocolError(f"Invalid request type: {header.request_type}")

class Response(abc.ABC):
    @abc.abstractmethod
    def to_raw(self) -> bytes:
        pass

    @classmethod
    @abc.abstractmethod
    def type(cls) -> ResponseType:
        pass

@dataclass
class KeepAliveResponse(Response):
    def to_raw(self) -> bytes:
        return struct.pack("<I", self.type())

    @classmethod
    def type(cls) -> ResponseType:
        return ResponseType.KEEP_ALIVE

@dataclass
class Command:
    command_id: int
    data: bytes

    def to_raw(self) -> bytes:
        return struct.pack("<II", self.command_id, len(self.data)) + self.data

@dataclass
class ExecuteCommandsResponse(Response):
    commands: List[Command]

    def to_raw(self) -> bytes:
        return (
            struct.pack("<II", self.type(), len(self.commands)) +
            b''.join(command.to_raw() for command in self.commands)
        )
    
    @classmethod
    def type(cls) -> ResponseType:
        return ResponseType.EXECUTE_COMMANDS


class ProtocolError(Exception):
    pass

async def write_response(writer: asyncio.StreamWriter, response: Response):
    data = response.to_raw()
    writer.write(data)
    await writer.drain()

def generate_id() -> int:
    COMMAND_ID_BITS = 32
    return secrets.randbits(COMMAND_ID_BITS)

