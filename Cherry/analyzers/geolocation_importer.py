import uuid
from sqlalchemy import select
from sqlalchemy.orm import joinedload
from Cherry.analyzers.analyzer import ProductAnalyzer
from Cherry.database import Database
from Cherry.models import Client, Location
from PoopBiter import logger
from PoopBiter.figs import GEO_DUDE_FIG_ID
from PoopBiter.products import FigProduct, Product, ProductInfo, ProductType, TypedProductType, TextTypedProduct
from PoopBiter.parsing import parse_structured_product
from PoopBiter.location import format_coordinates


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

        async for session in Database.get_db():
            stmt = select(Client).where(Client.client_id == product_info.client_id).options(
                joinedload(Client.location)
            )
            result = await session.execute(stmt)
            client = result.unique().scalar_one_or_none()

            if not client:
                raise LookupError(
                    f"Client {hex(product_info.client_id)} not found in database")

            location = Location(id=uuid.uuid4(),
                                latitude=location_fields["latitude"], longitude=location_fields["longitude"], accuracy_meters=location_fields["accuracy"])
            session.add(location)
            client.location_id = location.id
            logger.info(
                f"committed location {format_coordinates(location)} for client {product_info.client_id:x}")
