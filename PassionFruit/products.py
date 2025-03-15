from pathlib import Path
import enum

class ProductType(enum.Enum):
    COMMAND_ERROR = 0
    RAW = 1
    FIG_OPERATION_ERROR = 2
    FIG_PRODUCT = 3


format_product_type = {
    ProductType.COMMAND_ERROR: 'Command Error',
    ProductType.RAW: 'Raw',
    ProductType.FIG_OPERATION_ERROR: 'Fig Operation Error',
    ProductType.FIG_PRODUCT: 'Fig Product'
}

def get_product_type(product_path: Path) -> ProductType:
    PRODUCT_TYPE_SEPARATOR = '-'
    return ProductType(int(product_path.stem.split(PRODUCT_TYPE_SEPARATOR)[-1]))

def get_product_id(product_path: Path) -> str:
    PRODUCT_TYPE_SEPARATOR = '-'
    return product_path.stem.split(PRODUCT_TYPE_SEPARATOR)[0]
