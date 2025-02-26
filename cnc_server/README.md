# CNC Server

A Command and Control (CNC) server that manages multiple client connections via sockets. The server can send commands to specific clients and maintain connections through keep-alive messages.

## Features

- Asynchronous handling of multiple client connections
- Client-specific command file processing
- Response logging for each client
- Two types of responses:
  - ExecuteCommands: When commands are available for the client
  - KeepAlive: When no commands are pending

## Directory Structure

```
cnc_server/
├── server.py         # Main server implementation
├── client.py         # Example client implementation
├── client_commands/  # Directory for client-specific command files
└── client_logs/      # Directory for client response logs
```

## Usage

1. Start the server:
```bash
python server.py
```

2. Start one or more clients:
```bash
python client.py
```

3. To send commands to a specific client:
   - Create a file in the `client_commands/<client_id>` directory with a `.cmd` extension
   - The server will automatically detect and send these commands to the appropriate client

## Command Files

Command files should be placed in the `client_commands/<client_id>` directory with the following naming convention:
```
command_<timestamp>.cmd
```

The server will process these files in order and delete them after sending to the client.
