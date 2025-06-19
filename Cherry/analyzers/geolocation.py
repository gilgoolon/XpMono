import os
from typing import Dict, List, Tuple
import requests
from sqlalchemy import select
from sqlalchemy.orm import joinedload
from Cherry.analyzers.analyzer import ProductAnalyzer
from Cherry.database import Database
from Cherry.models import Client
from PoopBiter import logger
from PoopBiter.figs import GEO_DUDE_FIG_ID
from PoopBiter.products import FigProduct, Product, ProductInfo, ProductType, TypedProductType, TextTypedProduct
from PoopBiter.parsing import parse_structured_product

class GeoLocationAnalyzer(ProductAnalyzer):
    GOOGLE_GEOLOCATION_API_URL = "https://www.googleapis.com/geolocation/v1/geolocate?key={key}"

    def __init__(self) -> None:
        super().__init__()
        api_key = os.getenv("GOOGLE_LOCATION_API_KEY")
        self._api_url = self.GOOGLE_GEOLOCATION_API_URL.format(key=api_key)
    
    def should_analyze_info(self, product_info: ProductInfo) -> bool:
        return product_info.product_type == ProductType.FIG_PRODUCT

    def should_analyze_product(self, product_info: ProductInfo, product: Product) -> bool:
        GEO_DUDE_DISCOVER_NETWORKS_OPERATION_ID = 1
        return (
            isinstance(product, FigProduct) and
            product.fig_id == GEO_DUDE_FIG_ID and
            product.fig_operation_type == GEO_DUDE_DISCOVER_NETWORKS_OPERATION_ID
        )

    async def analyze(self, product_info: ProductInfo, product: Product) -> None:
        product: FigProduct = product
        
        typed_product = product.product
        
        if typed_product.type != TypedProductType.TEXT:
            raise ValueError(
                f"Unexpected GeoDude::DiscoverNetworks product type: {typed_product.type}")
        
        typed_product: TextTypedProduct = typed_product
        
        sections = parse_structured_product(typed_product.text)
        networks_sections = filter(lambda section: section.name == "Networks", sections)
        networks = [network.fields for network in next(
            networks_sections).objects if network.object_type == "Network"]
        
        (client_ip, location) = await self._get_client_location_info(product_info.client_id)
        if location is not None:
            logger.debug(
                f"client location already known for client {product_info.client_id:x}, skipping API request for geolocation")
            return
        logger.debug(
            f"found latest ip {client_ip} for client id {product_info.client_id:x}")
        location = await self._find_location(client_ip, networks)
        await self._commit_location(product_info.client_id, location)
        logger.info(
            f"committed location {location} for client {product_info.client_id:x}")
    
    async def _get_client_location_info(self, client_id: int) -> str:
        async for session in Database.get_db():
            stmt = select(Client).where(Client.client_id == client_id).options(
                joinedload(Client.ip_addresses),
            )
            result = await session.execute(stmt)
            client = result.unique().scalar_one_or_none()

            if not client:
                raise LookupError(f"Client {hex(client_id)} not found in database")

            location = ((client.location_long, client.location_lat),
                        client.location_accuracy_meters)

            return (client.ip_addresses[-1].ip_address, location if location[-1] is not None else None)
    
    @classmethod
    def _google_api_jsonify_network(cls, network: Dict[str, str]) -> Dict[str, str]:
        return {
            "macAddress": network.get("bssid"),
            "signalStrength": network.get("signal_strength_dbm"),
            "age": 0,
        }

    async def _find_location(self, ip_address: str, networks: List[Dict[str, str]]) -> Tuple[Tuple[float, float], int]:
        google_api_request_body = {
            "considerIp": "false",
            "wifiAccessPoints": [self._google_api_jsonify_network(network) for network in networks]
        }
        response = requests.post(self._api_url, json=google_api_request_body)
        logger.api(self._api_url, google_api_request_body, response)

        if response.status_code != 200:
            raise LookupError(
                f"Failed to find location, api responded with {response.status_code}: {response.content}")
        data = response.json()
        location = data["location"]
        location_lat = location["lat"]
        location_long = location["lng"]
        accuracy_meters = data["accuracy"]
        return ((location_lat, location_long), accuracy_meters)
    
    async def _commit_location(self, client_id: int, location: Tuple[Tuple[float, float], int]) -> None:
        (location_lat, location_long), accuracy_meters = location
        async for session in Database.get_db():
            result = await session.execute(select(Client).where(Client.client_id == client_id))
            obj = result.scalar_one()

            obj.location_lat = location_lat
            obj.location_long = location_long
            obj.location_accuracy_meters = accuracy_meters
