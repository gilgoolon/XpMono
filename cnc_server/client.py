import asyncio
import json
import logging
from datetime import datetime

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class CNCClient:
    def __init__(self, host='localhost', port=8888):
        self.host = host
        self.port = port

    async def connect(self):
        try:
            self.reader, self.writer = await asyncio.open_connection(
                self.host, self.port
            )
            logging.info(f"Connected to server at {self.host}:{self.port}")
            
            while True:
                # Send request to server first
                request = {
                    "type": "status_request",
                    "timestamp": datetime.now().isoformat()
                }
                request_data = json.dumps(request).encode() + b'\n'
                self.writer.write(request_data)
                await self.writer.drain()
                
                # Wait for server response
                data = await self.reader.readline()
                if not data:
                    break
                
                message = json.loads(data.decode())
                logging.info(f"Received from server: {message}")
                
                # Handle different message types
                if message['type'] == 'ExecuteCommands':
                    # In a real implementation, you would execute the commands here
                    logging.info(f"Executing commands: {message['commands']}")
                else:  # KeepAlive
                    logging.info("Received keep-alive response")
                
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
