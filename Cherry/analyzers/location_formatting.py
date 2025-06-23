import os
import requests
from sqlalchemy import select
from sqlalchemy.orm import joinedload
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

        async for session in Database.get_db():
            stmt = select(Client).where(Client.client_id == product_info.client_id).options(
                joinedload(Client.location)
            )
            result = await session.execute(stmt)
            client = result.unique().scalar_one_or_none()

            if not client:
                raise LookupError(
                    f"Client {hex(product_info.client_id)} not found in database")

            location = client.location
            if len(location) == 0:
                logger.debug(
                    f"client location is unknown for client {product_info.client_id:x}, skipping API request for formatting")
                return

            location = location[0]
            if location.label is not None:
                logger.debug(
                    f"client location is already formatted for client {product_info.client_id:x}: {location.label}")
                return

            formatted_location = await self._format_location(location.latitude, location.longitude)
            location.label = formatted_location
            logger.info(
                f"committed location formatting {formatted_location} for location {location.id}")

    async def _format_location(self, latitude: float, longitude: float) -> str:
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
