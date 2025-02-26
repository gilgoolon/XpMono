import asyncio
import json
import os
from datetime import datetime
import logging
from protocol import Command, ExecuteCommandsResponse, KeepAliveResponse, ProtocolError, Request, Response, write_response

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class CNCServer:
    def __init__(self, host='0.0.0.0', port=8888):
        self.host = host
        self.port = port
        self.clients = {}
        self.base_dir = os.path.dirname(os.path.abspath(__file__))
        self.commands_dir = os.path.join(self.base_dir, 'client_commands')
        self.logs_dir = os.path.join(self.base_dir, 'client_logs')
        
        # Create necessary directories
        os.makedirs(self.commands_dir, exist_ok=True)
        os.makedirs(self.logs_dir, exist_ok=True)

    def get_client_commands(self, client_id) -> list[Command]:
        """Get all pending command files for a client."""
        client_dir = os.path.join(self.commands_dir, str(client_id))
        if not os.path.exists(client_dir):
            return []
        
        commands = []
        command_files = sorted([f for f in os.listdir(client_dir) if f.endswith('.cmd')])
        
        for cmd_file in command_files:
            try:
                # Extract command ID from filename (assuming format: command_<id>.cmd)
                cmd_id = int(cmd_file.split('_')[1].split('.')[0])
                file_path = os.path.join(client_dir, cmd_file)
                
                # Read binary data
                with open(file_path, 'rb') as f:
                    cmd_data = f.read()
                
                commands.append(Command(command_id=cmd_id, data=cmd_data))
                
                # Delete the file after reading
                os.remove(file_path)
                
            except Exception as e:
                logging.error(f"Error reading command file {cmd_file} for client {client_id}: {e}")
                continue
        
        return commands

    def log_client_response(self, client_id, response):
        """Log client response to a file."""
        client_log_dir = os.path.join(self.logs_dir, str(client_id))
        os.makedirs(client_log_dir, exist_ok=True)
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        log_file = os.path.join(client_log_dir, f"response_{timestamp}.log")
        
        try:
            with open(log_file, 'w') as f:
                f.write(f"Timestamp: {timestamp}\n")
                f.write(f"Response: {response}\n")
        except Exception as e:
            logging.error(f"Error logging response for client {client_id}: {e}")

    def handle_request(self, request: Request) -> Response:
        """Handle a parsed request and generate appropriate response."""
        commands = self.get_client_commands(request.header.client_id)
        if len(commands) == 0:
            return KeepAliveResponse()
        return ExecuteCommandsResponse(commands)

    async def handle_client(self, reader, writer):
        """Handle individual client connections."""
        client_addr = writer.get_extra_info('peername')
        logging.info(f"New connection from {client_addr}")

        try:
            while True:
                try:
                    # Parse the request using our protocol parser
                    request = await Request.from_stream(reader)
                    logging.info(f"Received request type {request.header.request_type} from client {request.header.client_id}")
                    
                    # Handle the request
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

                await asyncio.sleep(0.1)

        finally:
            writer.close()
            await writer.wait_closed()
            logging.info(f"Client {client_addr} disconnected")

    async def start(self):
        """Start the CNC server."""
        server = await asyncio.start_server(
            self.handle_client,
            self.host,
            self.port
        )

        logging.info(f"CNC Server started on {self.host}:{self.port}")
        
        async with server:
            await server.serve_forever()

if __name__ == "__main__":
    server = CNCServer()
    try:
        asyncio.run(server.start())
    except KeyboardInterrupt:
        logging.info("Server shutdown requested")
    except Exception as e:
        logging.error(f"Server error: {e}")
