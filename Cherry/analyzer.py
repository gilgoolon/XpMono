import os
import asyncio
from glob import glob
from pathlib import Path
from typing import List, Optional

from watchfiles import awatch, Change

from PoopBiter import logger
from Cherry.analyzers.analyzer import ProductAnalyzer
from Cherry.analyzers.geolocation import GeoLocationAnalyzer
from Cherry.analyzers.location_formatting import LocationFormattingAnalyzer
from PoopBiter.products import Product, ProductInfo

def get_client_products(root: Path, client_id: str) -> List[str]:
    expression = root / "products" / client_id / "*" / "*"
    return [Path(path).absolute().as_posix() for path in glob(expression.as_posix()) if os.path.isfile(path)]


async def analyze_product(product: Path) -> None:
    analyzers: List[ProductAnalyzer] = [
        GeoLocationAnalyzer(), LocationFormattingAnalyzer()]

    info = ProductInfo.from_path(product)
    parsed_product: Optional[Product] = None

    for analyzer in analyzers:
        if not analyzer.should_analyze_info(info):
            continue

        if parsed_product is None:
            parsed_product = Product.from_path(product)

        if not analyzer.should_analyze_product(info, parsed_product):
            continue

        try:
            logger.info(
                f"{analyzer.__class__.__name__} analyzing product {info.product_id:x}")
            await analyzer.analyze(info, parsed_product)
        except Exception as ex:
            logger.error(f"Failed to analyze product {info}: {ex}")


async def analyze_products(products: List[Path]) -> None:
    for product in products:
        asyncio.create_task(analyze_product(product))


async def analyze_incoming_products(root: Path) -> None:
    products_dir = root / "products"
    logger.info(
        f"watching directory for products: {products_dir}")

    def only_new_files(change_type: Change, path: str):
        return change_type == Change.added and os.path.isfile(path)

    products_dir.mkdir(exist_ok=True)
    async for changes in awatch(products_dir, watch_filter=only_new_files, recursive=True):
        try:
            new_products = [Path(path) for (change_type, path) in changes]
            logger.info(f"incoming products: {new_products}")
            asyncio.create_task(analyze_products(new_products))
        except Exception as ex:
            logger.error(ex)
