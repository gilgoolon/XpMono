import os
import asyncio
import logging
from typing import Optional
import aiohttp
import argparse
from glob import glob
from pathlib import Path
from logger import make_logger
from protocol import Command, ExecuteCommandsResponse, KeepAliveResponse, ProtocolError, Request, RequestType, Response, write_response, generate_id


class CNCServer:
    def __init__(self, interface: str, port: int, root: Path, logger: logging.Logger):
        self._interface = interface
        self._port = port
        self._root = root
        self._commands_dir = self._root / "commands"
        self._products_dir = self._root / "products"
        self._logger = logger
        self._cherry_session: Optional[aiohttp.ClientSession] = None
        self._cherry_url = "http://localhost:8000"

        os.makedirs(self._commands_dir, exist_ok=True)

    def get_client_commands(self, client_id) -> list[Command]:
        client_dir = self._commands_dir / f"{client_id:x}"

        commands = []
        for path in glob((client_dir / "*.cmd").as_posix()):
            try:
                path = Path(path)
                command_id = generate_id()
                command_data = path.read_bytes()
                
                commands.append(Command(command_id=command_id, data=command_data))
                
                path.unlink()

            except Exception as e:
                self._logger.error(f"Error reading command file {path} for client {hex(client_id)}: {e}")
                continue
        
        return commands

    def handle_return_products(self, request: Request) -> None:
        client_products_path = self._products_dir / f"{request.header.client_id:x}"

        for product in request.data.products:
            product_path = client_products_path / f"{product.command_id:x}" / f"{product.product_id:x}-{product.product_type}"
            os.makedirs(product_path.parent.as_posix(), exist_ok=True)
            product_path.write_bytes(product.data)
            self._logger.info(f"Client {request.header.client_id:x} received product {product.product_id:x} for command {product.command_id:x}")

    def make_client_response(self, request: Request) -> Response:
        commands = self.get_client_commands(request.header.client_id)
        if len(commands) == 0:
            return KeepAliveResponse()
        return ExecuteCommandsResponse(commands)

    def handle_request(self, request: Request) -> Response:
        self._logger.info(f"Received request type {request.header.request_type} from client {request.header.client_id:x}")

        if request.header.request_type == RequestType.RETURN_PRODUCTS:
            self.handle_return_products(request)

        response = self.make_client_response(request)
        self._logger.info(f"Sending response type {response.type()} from client {request.header.client_id:x}")
        return response

    async def notify_client_connection(self, ip: str, client_id: int):
        data = {
            "ip": ip,
            "client_id": f"{client_id:x}"
        }
        ENDPOINT = "/client-connected"
        async with self._cherry_session.post(self._cherry_url + ENDPOINT, json=data) as response:
            OK = 200
            if response.status != OK:
                self._logger.error(f"Failed to notify client connection: {await response.text()}")

    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        client_addr = writer.get_extra_info("peername")
        ip, port = client_addr
        self._logger.info(f"New connection from {client_addr}")

        try:
            while True:
                try:
                    request = await Request.from_stream(reader)                    
                    await self.notify_client_connection(ip, request.header.client_id)
                    response = self.handle_request(request)

                    await write_response(writer, response)

                except ProtocolError as e:
                    self._logger.error(f"Protocol error from {client_addr}: {e}")
                    break
                except ConnectionError:
                    break
                except Exception as e:
                    self._logger.error(f"Error handling client {client_addr}: {e}")
                    break

        finally:
            writer.close()
            await writer.wait_closed()
            self._logger.info(f"Client {client_addr} disconnected")

    async def start(self):
        server = await asyncio.start_server(
            self.handle_client,
            self._interface,
            self._port
        )

        self._cherry_session = aiohttp.ClientSession()
        self._logger.info(f"CNC Server started on {self._interface}:{self._port}")
        
        async with server:
            await server.serve_forever()

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Liver CNC Server")
    parser.add_argument("--root", type=Path, default=Path(__file__).parent, help="Root folder of the cnc (commands, logs, etc...)")
    parser.add_argument("--port", "-p", type=int, default=8888, help="Server port for clients")

    args = parser.parse_args()

    INTERFACE = "0.0.0.0"
    LOGFILENAME = "log.txt"

    logger = make_logger(args.root / LOGFILENAME)
    server = CNCServer(interface=INTERFACE, port=args.port, root=args.root, logger=logger)

    try:
        asyncio.run(server.start())
    except KeyboardInterrupt:
        logger.info("Server shutdown requested")
    except Exception as e:
        logger.error(f"Server error: {e}")
