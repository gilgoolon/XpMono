import os
import argparse
import asyncio
import logging
from glob import glob
from pathlib import Path
from protocol import Command, ExecuteCommandsResponse, KeepAliveResponse, ProtocolError, Request, Response, write_response

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class CNCServer:
    def __init__(self, interface: str, port: int, root: Path):
        self.interface = interface
        self.port = port
        self.root = root
        self.commands_dir = self.root / "commands"
        self.log_file = self.root / "log.txt"

        os.makedirs(self.commands_dir, exist_ok=True)

    def get_client_commands(self, client_id) -> list[Command]:
        client_dir = self.commands_dir / hex(client_id)

        commands = []
        for path in glob((client_dir / "*.cmd").as_posix()):
            try:
                path = Path(path)
                command_id = int(path.stem, 16)
                command_data = path.read_bytes()
                
                commands.append(Command(command_id=command_id, data=command_data))
                
                path.unlink()

            except Exception as e:
                logging.error(f"Error reading command file {path} for client {hex(client_id)}: {e}")
                continue
        
        return commands

    def handle_request(self, request: Request) -> Response:
        commands = self.get_client_commands(request.header.client_id)
        if len(commands) == 0:
            return KeepAliveResponse()
        return ExecuteCommandsResponse(commands)

    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        client_addr = writer.get_extra_info('peername')
        logging.info(f"New connection from {client_addr}")

        try:
            while True:
                try:
                    request = await Request.from_stream(reader)
                    logging.info(f"Received request type {request.header.request_type} from client {hex(request.header.client_id)}")
                    
                    response = self.handle_request(request)

                    await write_response(writer, response)

                except ProtocolError as e:
                    logging.error(f"Protocol error from {client_addr}: {e}")
                    break
                except ConnectionError:
                    break
                except Exception as e:
                    logging.error(f"Error handling client {client_addr}: {e}")
                    break

        finally:
            writer.close()
            await writer.wait_closed()
            logging.info(f"Client {client_addr} disconnected")

    async def start(self):
        server = await asyncio.start_server(
            self.handle_client,
            self.interface,
            self.port
        )

        logging.info(f"CNC Server started on {self.interface}:{self.port}")
        
        async with server:
            await server.serve_forever()

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Liver CNC Server")
    parser.add_argument("--root", type=Path, default=Path(__file__).parent, help="Root folder of the cnc (commands, logs, etc...)")
    parser.add_argument("--port", "-p", type=int, default=8888, help="Server port for clients")

    args = parser.parse_args()

    INTERFACE = '0.0.0.0'

    server = CNCServer(interface=INTERFACE, port=args.port, root=args.root)

    try:
        asyncio.run(server.start())
    except KeyboardInterrupt:
        logging.info("Server shutdown requested")
    except Exception as e:
        logging.error(f"Server error: {e}")
