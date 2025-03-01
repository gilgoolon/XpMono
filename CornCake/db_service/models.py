from sqlalchemy import Column, Integer, String, DateTime, ForeignKey, create_engine
from sqlalchemy.orm import declarative_base, relationship
from sqlalchemy.sql import func

Base = declarative_base()

class Client(Base):
    __tablename__ = "clients"
    
    client_id = Column(Integer, primary_key=True)
    last_connection = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    ip_addresses = relationship("ClientIP", back_populates="client")
    products = relationship("ClientProduct", back_populates="client")

class ClientIP(Base):
    __tablename__ = "client_ips"
    
    id = Column(Integer, primary_key=True)
    client_id = Column(Integer, ForeignKey("clients.client_id"))
    ip_address = Column(String)
    first_seen = Column(DateTime(timezone=True), server_default=func.now())
    last_seen = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    client = relationship("Client", back_populates="ip_addresses")

class ClientProduct(Base):
    __tablename__ = "client_products"
    
    id = Column(Integer, primary_key=True)
    client_id = Column(Integer, ForeignKey("clients.client_id"))
    product_id = Column(Integer)
    command_id = Column(Integer)
    product_type = Column(Integer)
    client = relationship("Client", back_populates="products")
