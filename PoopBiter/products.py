import io
import abc
import enum
import base64
import struct
from typing import Any, Dict
from pathlib import Path
from dataclasses import dataclass

from PIL import Image
from PIL.ImageFile import ImageFile


class ProductType(enum.IntEnum):
    COMMAND_ERROR = 0
    RAW = 1
    FIG_OPERATION_ERROR = 2
    FIG_PRODUCT = 3

PRODUCT_TYPE_TO_STRING = {
    ProductType.COMMAND_ERROR: "Command Error",
    ProductType.RAW: "Raw",
    ProductType.FIG_OPERATION_ERROR: "Fig Operation Error",
    ProductType.FIG_PRODUCT: "Fig Product",
}


@dataclass
class ProductInfo:
    id: str
    type: ProductType
    
    @classmethod
    def from_filename(cls, filename: str) -> "ProductInfo":
        PRODUCT_TYPE_SEPARATOR = '-'
        tokens = filename.split(PRODUCT_TYPE_SEPARATOR)
        raw_id, raw_type = tokens
        return ProductInfo(int.from_bytes(bytes.fromhex(raw_id)), ProductType(int(raw_type)))
    
    @classmethod
    def from_path(cls, path: Path) -> "ProductInfo":
        return cls.from_filename(path.stem)


class TypedProductType(enum.IntEnum):
    RESERVED = 0,
    TEXT = 1
    RAW = 2
    IMAGE_BMP = 3
    IMAGE_PNG = 4

TYPED_PRODUCT_TYPE_TO_STRING = {
    TypedProductType.RESERVED: "Reserved",
    TypedProductType.TEXT: "Text",
    TypedProductType.RAW: "Raw",
    TypedProductType.IMAGE_BMP: "BMP Image",
    TypedProductType.IMAGE_PNG: "PNG Image",
}


class TypedProduct(abc.ABC):
    def __init__(self, type: TypedProductType) -> None:
        super().__init__()
        self._type = type
    
    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        FORMAT = "<II"
        product_type, content_length = struct.unpack(
            FORMAT, data[:struct.calcsize(FORMAT)])
        product_type = TypedProductType(product_type)
        content = data[struct.calcsize(FORMAT): struct.calcsize(FORMAT) + content_length]
        
        products: Dict[TypedProductType, TypedProduct] = {
            TypedProductType.RAW: RawTypedProduct,
            TypedProductType.TEXT: TextTypedProduct,
            TypedProductType.IMAGE_BMP: ImageTypedProduct,
            TypedProductType.IMAGE_PNG: ImageTypedProduct,
        }
        
        if product_type not in products:
            raise ValueError(f"Invalid TypedProduct type: {product_type}")

        return products[product_type].from_raw_bytes(content)

    def displayable(self) -> dict[str, Any]:
        base_properties = {
            "type_suffix": f" ({TYPED_PRODUCT_TYPE_TO_STRING[self._type]})",
            "type": self._display_type
        }
        return {
            **base_properties,
            **self._displayable_properties
        }
    
    @property
    @abc.abstractmethod
    def _displayable_properties(self) -> dict[str, Any]:
        pass
    
    @property
    def _display_type(self) -> str:
        return TYPED_PRODUCT_TYPE_TO_STRING[self._type]


class RawTypedProduct(TypedProduct):
    def __init__(self, data: bytes) -> None:
        super().__init__(TypedProductType.RAW)
        self._data = data
    
    @property
    def _displayable_properties(self):
        SHOWING_BYTES = 16
        displayed_bytes = self._data[:SHOWING_BYTES].ljust(SHOWING_BYTES, b'\0')
        return {
            f"value (first {SHOWING_BYTES} bytes)": f"0x{hex(displayed_bytes)}",
            "hex": f"0x{displayed_bytes:08X}",
        }

    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        return cls(data)


class TextTypedProduct(TypedProduct):
    def __init__(self, text: str) -> None:
        super().__init__(TypedProductType.TEXT)
        self._text = text

    @property
    def _displayable_properties(self):
        return {
            "data": self._text
        }

    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        return cls(data.decode("utf-16"))


class ImageTypedProduct(TypedProduct):
    def __init__(self, image: ImageFile, product_type: TypedProductType) -> None:
        super().__init__(product_type)
        self._image = image

    @classmethod
    @abc.abstractmethod
    def _image_type(cls) -> str:
        pass

    @property
    def _displayable_properties(self):
        raw_image = io.BytesIO()
        self._image.save(raw_image)
        encoded_image = base64.b64encode(raw_image.getvalue()).decode("utf-8")

        return {
            "data": f"data:{self._display_type};base64,{encoded_image}",
            "width": self._image.width,
            "height": self._image.height,
            "mode": self._image.mode
        }
    
    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        try:
            image = Image.open(io.BytesIO(data))
            if image.format != BmpImageTypedProduct._image_type().upper():
                return BmpImageTypedProduct(image)
            if image.format != PngImageTypedProduct._image_type().upper():
                return PngImageTypedProduct(image)
            raise ValueError(
                f"Unsupported image type: {BmpImageTypedProduct._image_type()}")
        except Exception as e:
            raise ValueError(f"Invalid BMP image: {str(e)}")

    @property
    def _display_type(self) -> str:
        return f"image/{self._image_type()}"


class BmpImageTypedProduct(ImageTypedProduct):
    def __init__(self, image: ImageFile):
        super().__init__(image, TypedProductType.IMAGE_BMP)
        
    @classmethod
    def _image_type(cls) -> str:
        return "bmp"

class PngImageTypedProduct(ImageTypedProduct):
    def __init__(self, image: ImageFile):
        super().__init__(image, TypedProductType.IMAGE_PNG)
        
    @classmethod
    def _image_type(cls) -> str:
        return "png"
