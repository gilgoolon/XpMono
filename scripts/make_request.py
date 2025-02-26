import json
from pathlib import Path
import base64
import struct


dll_path = Path("C:\\Users\\alper\\OneDrive\\Documents\\XpMono\\Apricot\\Release\\SimpleDll.dll")

output = Path("C:\\Users\\alper\\input.txt")
library = base64.b64encode(dll_path.read_bytes()).decode('utf-8')

command = {
    "info": {
        "type": 0,
    },
    "parameters": {
        "library_id": 0,
        "library_buffer": library
    }
}

commands = [json.dumps(command)]

EXECUTE_COMMANDS_ID = 0
request = struct.pack("<II", EXECUTE_COMMANDS_ID, len(commands))
for command_data in commands:
    request += struct.pack("<I", len(command_data)) + command_data

output.write_bytes(request)
