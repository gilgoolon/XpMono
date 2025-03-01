from fastapi import FastAPI, Depends, HTTPException
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select
from sqlalchemy.orm import joinedload
from datetime import datetime
from typing import List
from pydantic import BaseModel

from database import get_db, init_db
from models import Client, ClientIP, ClientProduct

app = FastAPI()

class ClientConnection(BaseModel):
    client_id: int
    ip: str

class ProductInfo(BaseModel):
    product_id: int
    command_id: int
    product_type: int

class ClientInfo(BaseModel):
    client_id: int
    last_connection: datetime
    current_ip: str
    product_count: int

class DetailedClientInfo(BaseModel):
    client_id: int
    last_connection: datetime
    ip_history: List[dict]
    products: List[ProductInfo]

@app.on_event("startup")
async def startup():
    await init_db()

@app.post("/client-connected")
async def client_connected(connection: ClientConnection, db: AsyncSession = Depends(get_db)):
    # Get or create client
    client = await db.get(Client, connection.client_id)
    if not client:
        client = Client(client_id=connection.client_id)
        db.add(client)
    
    # Update or create IP address
    stmt = select(ClientIP).where(
        ClientIP.client_id == connection.client_id,
        ClientIP.ip_address == connection.ip
    )
    result = await db.execute(stmt)
    ip_record = result.scalar_one_or_none()
    
    if not ip_record:
        ip_record = ClientIP(
            client_id=connection.client_id,
            ip_address=connection.ip
        )
        db.add(ip_record)
    else:
        ip_record.last_seen = datetime.utcnow()
    
    await db.commit()
    return {"status": "success"}

@app.get("/get-clients", response_model=List[ClientInfo])
async def get_clients(db: AsyncSession = Depends(get_db)):
    stmt = select(Client).options(
        joinedload(Client.ip_addresses),
        joinedload(Client.products)
    )
    result = await db.execute(stmt)
    clients = result.unique().scalars().all()
    
    return [
        ClientInfo(
            client_id=client.client_id,
            last_connection=client.last_connection,
            current_ip=client.ip_addresses[-1].ip_address if client.ip_addresses else None,
            product_count=len(client.products)
        ) for client in clients
    ]

@app.get("/get-client/{client_id}", response_model=DetailedClientInfo)
async def get_client_details(client_id: int, db: AsyncSession = Depends(get_db)):
    stmt = select(Client).where(Client.client_id == client_id).options(
        joinedload(Client.ip_addresses),
        joinedload(Client.products)
    )
    result = await db.execute(stmt)
    client = result.unique().scalar_one_or_none()
    
    if not client:
        raise HTTPException(status_code=404, detail="Client not found")
    
    return DetailedClientInfo(
        client_id=client.client_id,
        last_connection=client.last_connection,
        ip_history=[{
            "ip": ip.ip_address,
            "first_seen": ip.first_seen,
            "last_seen": ip.last_seen
        } for ip in sorted(client.ip_addresses, key=lambda x: x.last_seen, reverse=True)],
        products=[
            ProductInfo(
                product_id=product.product_id,
                command_id=product.command_id,
                product_type=product.product_type
            ) for product in client.products
        ]
    )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
