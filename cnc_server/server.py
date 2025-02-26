import asyncio
import json
import os
from datetime import datetime
import logging
from protocol import (
    RequestParser, ProtocolError, BaseRequest, StatusRequest,
    CommandResultRequest, ResponseType, KeepAliveResponse,
    ExecuteCommandsResponse, write_response
)

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

    def get_client_commands(self, client_id):
        """Check for command files for a specific client."""
        client_dir = os.path.join(self.commands_dir, str(client_id))
        if not os.path.exists(client_dir):
            return None
        
        command_files = [f for f in os.listdir(client_dir) if f.endswith('.cmd')]
        if not command_files:
            return None
            
        # Read the first command file
        command_file = os.path.join(client_dir, command_files[0])
        try:
            with open(command_file, 'r') as f:
                commands = f.read()
            # Delete the file after reading
            os.remove(command_file)
            return commands
        except Exception as e:
            logging.error(f"Error reading command file for client {client_id}: {e}")
            return None

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

    async def handle_request(self, request: BaseRequest, writer: asyncio.StreamWriter) -> None:
        """Handle a parsed request and generate appropriate response."""
        if isinstance(request, StatusRequest):
            # Check for commands
            commands = self.get_client_commands(request.client_id)
            
            if commands:
                response = ExecuteCommandsResponse(
                    request_id=request.request_id,
                    client_id=request.client_id,
                    response_type=ResponseType.EXECUTE_COMMANDS,
                    commands=commands
                )
            else:
                response = KeepAliveResponse(
                    request_id=request.request_id,
                    client_id=request.client_id,
                    response_type=ResponseType.KEEP_ALIVE,
                    timestamp=datetime.now().isoformat()
                )
                
            await write_response(writer, response)
            
        elif isinstance(request, CommandResultRequest):
            # Log the command result
            self.log_client_response(
                request.client_id,
                f"Command {request.command_id} result: {request.result}"
            )
            
            # Send keep-alive response
            response = KeepAliveResponse(
                request_id=request.request_id,
                client_id=request.client_id,
                response_type=ResponseType.KEEP_ALIVE,
                timestamp=datetime.now().isoformat()
            )
            await write_response(writer, response)

    async def handle_client(self, reader, writer):
        """Handle individual client connections."""
        client_addr = writer.get_extra_info('peername')
        logging.info(f"New connection from {client_addr}")

        try:
            while True:
                try:
                    # Parse the request using our protocol parser
                    request = await RequestParser.parse_request(reader)
                    logging.info(f"Received request type {request.request_type} from client {request.client_id}")
                    
                    # Handle the request
                    await self.handle_request(request, writer)
                    
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
