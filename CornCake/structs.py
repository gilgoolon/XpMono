import asyncio
import struct


async def read_struct(reader: asyncio.StreamReader, struct_format: str) -> tuple:
    return struct.unpack(struct_format, await reader.readexactly(struct.calcsize(struct_format)))

async def read_sized_buffer(reader: asyncio.StreamReader) -> bytes:
    length, = await read_struct(reader, "<I")
    return await reader.readexactly(length)
