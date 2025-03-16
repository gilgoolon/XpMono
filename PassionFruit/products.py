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
    IMAGE_BMP = 3
    IMAGE_PNG = 4

def parse_typed_product(serialized_raw_product: bytes) -> dict:
    FORMAT = "<II"
    product_type, content_length = struct.unpack(FORMAT, serialized_raw_product[:struct.calcsize(FORMAT)])
    content = serialized_raw_product[struct.calcsize(FORMAT):]

    product_type = TypedProductType(product_type)

    print(f'got TypedProduct with type: {product_type}')
    if product_type == TypedProductType.RESERVED:
        return {
            'type': 'Reserved',
            'data': ''
        }
    
    if product_type == TypedProductType.TEXT:
        return {
            'type': 'Text',
            'data': content.decode('utf-8')
        }
    
    if product_type == TypedProductType.RAW:
        SHOWING_BYTES = 16
        displayed_bytes = content[:SHOWING_BYTES].ljust(SHOWING_BYTES, b'\0')
        return {
            'type': 'Raw',
            f'value (first {SHOWING_BYTES} bytes)': f'0x{hex(displayed_bytes)}',
            'hex': f'0x{displayed_bytes:08X}',
        }
    
    if product_type == TypedProductType.IMAGE_BMP:
        try:
            # Verify it's a valid BMP
            img = Image.open(io.BytesIO(content))
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
    
    if product_type == TypedProductType.IMAGE_PNG:
        try:
            # Verify it's a valid PNG
            img = Image.open(io.BytesIO(content))
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
    
    raise ValueError(f"Invalid TypedProduct type: {product_type}")
    


