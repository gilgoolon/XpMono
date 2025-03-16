import base64
import io
from pathlib import Path
import enum
import struct
from PIL import Image

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


class TypedProductType(enum.Enum):
    RESERVED = 0,
    TEXT = 1
    RAW = 2
    PICTURE_PNG = 3
    PICTURE_BMP = 4

def parse_typed_product(serialized_raw_product: bytes) -> dict:
    # elif product_type == products.ProductType.IMAGE_PNG:
    FORMAT = "<I"
    product_type, = struct.unpack(FORMAT, serialized_raw_product[:struct.calcsize(FORMAT)])
    content = serialized_raw_product[struct.calcsize(FORMAT):]

    if product_type == TypedProductType.RESERVED:
        return {
            'type': 'Reserved',
            'data': ''
        }
    
    elif product_type == TypedProductType.TEXT:
        return {
            'type': 'Text',
            'data': content.decode('utf-8')
        }
    
    elif product_type == TypedProductType.RAW:
        SHOWING_BYTES = 16
        displayed_bytes = content[:SHOWING_BYTES].ljust(SHOWING_BYTES, b'\0')
        return {
            'type': 'Raw',
            f'value (first {SHOWING_BYTES} bytes)': f'0x{hex(displayed_bytes)}',
            'hex': f'0x{displayed_bytes:08X}',
        }
    
    elif product_type == TypedProductType.PICTURE_BMP:
        try:
            # Verify it's a valid BMP
            img = Image.open(io.BytesIO(serialized_raw_product))
            if img.format != 'BMP':
                raise ValueError("Not a BMP image")
            
            # Convert to base64 for frontend display
            img_buffer = io.BytesIO()
            img.save(img_buffer, format='BMP')
            img_base64 = base64.b64encode(img_buffer.getvalue()).decode('utf-8')
            
            return {
                'type': 'image/bmp',
                'data': f'data:image/bmp;base64,{img_base64}',
                'width': img.width,
                'height': img.height,
                'mode': img.mode
            }
        except Exception as e:
            raise ValueError(f"Invalid BMP image: {str(e)}")
    
    elif product_type == TypedProductType.PICTURE_PNG:
        try:
            # Verify it's a valid PNG
            img = Image.open(io.BytesIO(serialized_raw_product))
            if img.format != 'PNG':
                raise ValueError("Not a PNG image")
            
            # Convert to base64 for frontend display
            img_buffer = io.BytesIO()
            img.save(img_buffer, format='PNG')
            img_base64 = base64.b64encode(img_buffer.getvalue()).decode('utf-8')
            
            return {
                'type': 'image/png',
                'data': f'data:image/png;base64,{img_base64}',
                'width': img.width,
                'height': img.height,
                'mode': img.mode
            }
        except Exception as e:
            raise ValueError(f"Invalid PNG image: {str(e)}")
    return {}
    


