from datetime import datetime
from typing import List, Optional
from pydantic import BaseModel


class ClientConnection(BaseModel):
    client_id: str
    ip: str

class ClientCommand(BaseModel):
    client_id: str
    data: str

class ClientInfo(BaseModel):
    client_id: str
    last_connection: datetime
    current_ip: str
    nickname: Optional[str]
    location: Optional[str]

class DetailedClientInfo(BaseModel):
    client_id: str
    last_connection: datetime
    ip_history: List[dict]
    nickname: Optional[str]
    location: Optional[str]
    products: List[str]
