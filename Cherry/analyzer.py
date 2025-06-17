import asyncio
from glob import glob
import logging
import os
from pathlib import Path
from typing import List

from watchfiles import awatch, Change

def get_client_products(root: Path, client_id: str) -> List[str]:
    expression = root / "products" / client_id / "*" / "*"
    return [Path(path).absolute().as_posix() for path in glob(expression.as_posix()) if os.path.isfile(path)]


async def analyze_products(products: List[Path]) -> None:
    pass


async def analyze_incoming_products(root: Path, logger: logging.Logger) -> None:
    products_dir = root / "products"
    logger.info(
        f"watching directory for products: {str(products_dir.absolute())}")

    def only_new_files(change_type: Change, path: str):
        return change_type == Change.Added and os.path.isfile(path)

    products_dir.mkdir(exist_ok=True)
    async for changes in awatch(products_dir, watch_filter=only_new_files, recursive=True):
        try:
            new_products = list(
                map(lambda change_type, path: Path(path), changes))
            logger.info(f"incoming products: {new_products}")
            asyncio.create_task(analyze_products(new_products))
        except Exception as ex:
            logger.exception(ex)
