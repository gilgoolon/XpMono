import uuid
from sqlalchemy import UUID, Column, Float, Integer, String, DateTime, ForeignKey, Text
from sqlalchemy.orm import declarative_base, relationship
from sqlalchemy.sql import func

Base = declarative_base()

class Client(Base):
    __tablename__ = "clients"
    
    client_id = Column(Integer, primary_key=True)
    nickname = Column(Text, None)
    last_connection = Column(DateTime(timezone=True), server_default=func.now())
    location_id = Column(UUID, None)
    ip_addresses = relationship("ClientIP", back_populates="client")
    location = relationship("Location", back_populates="client")

class ClientIP(Base):
    __tablename__ = "client_ips"
    
    id = Column(Integer, primary_key=True)
    client_id = Column(Integer, ForeignKey("clients.client_id"))
    ip_address = Column(String)
    first_seen = Column(DateTime(timezone=True), server_default=func.now())
    last_seen = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    client = relationship("Client", back_populates="ip_addresses")


class Location(Base):
    __tablename__ = "locations"

    id = Column(UUID(as_uuid=True), ForeignKey(
        "clients.location_id"), primary_key=True, default=uuid.uuid4)
    latitude = Column(Float, None)
    longitude = Column(Float, None)
    accuracy_meters = Column(Integer, None)
    label = Column(Text, None)
    client = relationship("Client", back_populates="location")
