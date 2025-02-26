import asyncio
import logging
from datetime import datetime
from protocol import (
    RequestParser, ProtocolError, StatusRequest,
    CommandResultRequest, RequestType, ResponseType,
    BaseResponse, KeepAliveResponse, ExecuteCommandsResponse,
    write_response
)

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class CNCClient:
    def __init__(self, host='localhost', port=8888):
        self.host = host
        self.port = port
        self.request_id = 0
        self.client_id = None  # Will be set when connecting

    def next_request_id(self) -> int:
        """Get the next request ID."""
        self.request_id += 1
        return self.request_id

    async def send_status_request(self, writer: asyncio.StreamWriter) -> None:
        """Send a status request to the server."""
        request = StatusRequest(
            request_id=self.next_request_id(),
            client_id=self.client_id,
            request_type=RequestType.STATUS,
            timestamp=datetime.now().isoformat()
        )
        await write_response(writer, request)

    async def send_command_result(self, writer: asyncio.StreamWriter, command_id: int, result: str) -> None:
        """Send a command result to the server."""
        request = CommandResultRequest(
            request_id=self.next_request_id(),
            client_id=self.client_id,
            request_type=RequestType.COMMAND_RESULT,
            command_id=command_id,
            result=result
        )
        await write_response(writer, request)

    async def handle_response(self, response: BaseResponse) -> None:
        """Handle server response."""
        if isinstance(response, KeepAliveResponse):
            logging.info("Received keep-alive response")
        elif isinstance(response, ExecuteCommandsResponse):
            logging.info(f"Received commands to execute: {response.commands}")
            # In a real implementation, execute the commands here
            
            # Send back the result
            await self.send_command_result(
                self.writer,
                response.request_id,  # Using request_id as command_id for this example
                f"Executed commands: {response.commands}"
            )

    async def connect(self):
        """Connect to the server and handle the communication loop."""
        try:
            self.reader, self.writer = await asyncio.open_connection(
                self.host, self.port
            )
            logging.info(f"Connected to server at {self.host}:{self.port}")
            
            # For this example, we'll use a random client ID
            import random
            self.client_id = random.randint(1, 1000000)
            logging.info(f"Using client ID: {self.client_id}")
            
            while True:
                # Send status request
                await self.send_status_request(self.writer)
                
                try:
                    # Parse server response
                    response = await RequestParser.parse_request(self.reader)
                    await self.handle_response(response)
                except ProtocolError as e:
                    logging.error(f"Protocol error: {e}")
                    break
                    
                await asyncio.sleep(1)  # Wait before sending next request
                
        except asyncio.CancelledError:
            logging.info("Client shutdown requested")
        except Exception as e:
            logging.error(f"Client error: {e}")
        finally:
            if hasattr(self, 'writer'):
                self.writer.close()
                await self.writer.wait_closed()
            logging.info("Connection closed")

if __name__ == "__main__":
    client = CNCClient()
    try:
        asyncio.run(client.connect())
    except KeyboardInterrupt:
        logging.info("Client shutdown requested")
