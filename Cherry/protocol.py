from datetime import datetime
from typing import List
from pydantic import BaseModel


class ClientConnection(BaseModel):
    client_id: int
    ip: str

class ClientInfo(BaseModel):
    client_id: int
    last_connection: datetime
    current_ip: str

class DetailedClientInfo(BaseModel):
    client_id: int
    last_connection: datetime
    ip_history: List[dict]
    products: List[str]
    commands_dir: str
