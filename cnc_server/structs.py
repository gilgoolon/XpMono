import asyncio
import struct


async def read_struct(reader: asyncio.StreamReader, struct_format: str) -> tuple:
    return struct.unpack(struct_format, await read_exact(reader, struct.calcsize(struct_format)))

async def read_exact(reader: asyncio.StreamReader, size: int) -> bytes:
    data = await reader.read(size)
    if len(data) < size:
        raise RuntimeError("Connection closed while reading")
    return data

async def read_sized_buffer(reader: asyncio.StreamReader) -> bytes:
    length, = await read_struct(reader, "<I")
    return await read_exact(reader, length)
