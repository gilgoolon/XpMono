from typing import Tuple
from sqlalchemy import select
from Cherry.analyzers.analyzer import ProductAnalyzer
from Cherry.database import Database
from Cherry.models import Client
from PoopBiter import logger
from PoopBiter.figs import GEO_DUDE_FIG_ID
from PoopBiter.products import FigProduct, Product, ProductInfo, ProductType, TypedProductType, TextTypedProduct
from PoopBiter.parsing import parse_structured_product


class GeoLocationImporterAnalyzer(ProductAnalyzer):
    def should_analyze_info(self, product_info: ProductInfo) -> bool:
        return product_info.product_type == ProductType.FIG_PRODUCT

    def should_analyze_product(self, product_info: ProductInfo, product: Product) -> bool:
        GEO_DUDE_FETCH_GEO_LOCATION_OPERATION_ID = 2
        return (
            isinstance(product, FigProduct) and
            product.fig_id == GEO_DUDE_FIG_ID and
            product.fig_operation_type == GEO_DUDE_FETCH_GEO_LOCATION_OPERATION_ID
        )

    async def analyze(self, product_info: ProductInfo, product: Product) -> None:
        product: FigProduct = product

        typed_product = product.product

        if typed_product.type != TypedProductType.TEXT:
            raise ValueError(
                f"Unexpected GeoDude::FetchGeoLocation product type: {typed_product.type}")

        typed_product: TextTypedProduct = typed_product

        sections = parse_structured_product(typed_product.text)
        geolocation_sections = filter(
            lambda section: section.name == "Geolocation", sections)
        locations = [location.fields for location in next(
            geolocation_sections).objects if location.object_type == "Location"]

        if len(locations) == 0:
            raise ValueError(f"No locations found in Geolocation section")

        location_fields = next(locations)
        location = ((location_fields["latitude"],
                    location_fields["longitude"]), location_fields["accuracy"])

        await self._commit_location(product_info.client_id, location)
        logger.info(
            f"committed location {location} for client {product_info.client_id:x}")

    async def _commit_location(self, client_id: int, location: Tuple[Tuple[float, float], int]) -> None:
        (location_lat, location_long), accuracy_meters = location
        async for session in Database.get_db():
            result = await session.execute(select(Client).where(Client.client_id == client_id))
            obj = result.scalar_one()

            obj.location_lat = location_lat
            obj.location_long = location_long
            obj.location_accuracy_meters = accuracy_meters
