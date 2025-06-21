import os
from typing import Optional, Tuple
import requests
from sqlalchemy import select
from Cherry.analyzers.analyzer import ProductAnalyzer
from Cherry.database import Database
from Cherry.models import Client, Location
from PoopBiter.figs import GEO_DUDE_FIG_ID
from PoopBiter.products import FigProduct, Product, ProductInfo, ProductType, TypedProductType, TextTypedProduct
from PoopBiter import logger


class LocationFormattingAnalyzer(ProductAnalyzer):
    GOOGLE_GEOLOCATION_API_URL = "https://maps.googleapis.com/maps/api/geocode/json"

    def __init__(self) -> None:
        super().__init__()
        self._parameters = {
            "key": os.getenv("GOOGLE_LOCATION_API_KEY")
        }
        self._api_url = self.GOOGLE_GEOLOCATION_API_URL

    def should_analyze_info(self, product_info: ProductInfo) -> bool:
        return product_info.product_type == ProductType.FIG_PRODUCT

    def should_analyze_product(self, product_info: ProductInfo, product: Product) -> bool:
        # GEO_DUDE_DISCOVER_NETWORKS_OPERATION_TYPE = 1  # should find a way to enforce this
        return (
            isinstance(product, FigProduct) and
            product.fig_id == GEO_DUDE_FIG_ID
        )

    async def analyze(self, product_info: ProductInfo, product: Product) -> None:
        product: FigProduct = product

        typed_product = product.product

        if typed_product.type != TypedProductType.TEXT:
            raise ValueError(
                f"Unexpected GeoDude::DiscoverNetworks product type: {typed_product.type}")

        location = await self._get_client_location_info(product_info.client_id)
        if location is None:
            logger.debug(
                f"client location is unknown for client {product_info.client_id:x}, skipping API request for formatting")
            return
        
        existing_formatted_location = await self._get_location_formatting_info(location)
        
        if existing_formatted_location is not None:
            logger.debug(
                f"client location is already formatted for client {product_info.client_id:x}: {existing_formatted_location}")
            return
        
        formatted_location = await self._format_location(location)
        await self._commit_location(location, formatted_location)
        logger.info(
            f"committed location formatting {location} for location {location}")

    async def _get_client_location_info(self, client_id: int) -> Optional[Tuple[Tuple[float, float], float]]:
        async for session in Database.get_db():
            stmt = select(Client).where(Client.client_id == client_id)
            result = await session.execute(stmt)
            client = result.unique().scalar_one_or_none()

            if not client:
                raise LookupError(
                    f"Client {hex(client_id)} not found in database")

            location = ((client.location_lat, client.location_long),
                        client.location_accuracy_meters)

            return location if location[-1] is not None else None

    async def _get_location_formatting_info(self, location: Tuple[Tuple[float, float], float]) -> str:
        (latitude, longitude), accuracy = location
        async for session in Database.get_db():
            stmt = select(Location).where(Location.latitude == latitude and Location.longitude == longitude)
            result = await session.execute(stmt)
            location = result.unique().scalar_one_or_none()
            
            if location is None or location.label is None:
                return None
            return location.label
    
    async def _set_location_formatting_info(self, latitude: float, longitude: float, label: str) -> str:
        async for session in Database.get_db():
            stmt = select(Location).where(Location.latitude ==
                                          latitude and Location.longitude == longitude)
            result = await session.execute(stmt)
            location = result.unique().scalar_one_or_none()

            location.label = label

    async def _format_location(self, location: Tuple[Tuple[float, float], float]) -> str:
        (latitude, longitude), accuracy = location
        google_api_request_parameters = {
            **self._parameters,
            "latlng": f"{latitude},{longitude}",
        }
        response = requests.post(self._api_url, params=google_api_request_parameters)
        logger.api(self._api_url, google_api_request_parameters, response)

        if response.status_code != 200:
            raise LookupError(
                f"Failed to format location, api responded with {response.status_code}: {response.content}")
            
        data = response.json()
        results = data["results"]
        if len(results) == 0:
            raise LookupError(f"Location API-lookup returned no matching results")
        return results[0]["formatted_address"]

    async def _commit_location(self, location: Tuple[Tuple[float, float], float], formatted_location: str) -> None:
        (latitude, longitude), accuracy = location
        async for session in Database.get_db():
            result = await session.execute(select(Location).where(Location.latitude == latitude and Location.longitude == longitude))
            obj = result.unique().scalar_one_or_none()

            if obj is None:
                session.add(Location(latitude=latitude, longitude=longitude, label=formatted_location))
            else:
                obj.label = formatted_location
