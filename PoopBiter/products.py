from datetime import datetime
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

from PoopBiter.fig import format_operation_name, get_fig
from PoopBiter.utils import unhex


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
    client_id: int
    command_id: int
    product_id: int
    product_type: ProductType
    creation_time: datetime

    @classmethod
    def from_path(cls, path: Path) -> "ProductInfo":
        PRODUCT_TYPE_SEPARATOR = '-'
        raw_product_id, raw_product_type = path.stem.split(
            PRODUCT_TYPE_SEPARATOR)
        command_id = unhex(path.parent.stem)
        client_id = unhex(path.parent.parent.stem)
        creation_time = datetime.fromtimestamp(path.stat().st_ctime)
        return ProductInfo(client_id, command_id, unhex(raw_product_id), ProductType(int(raw_product_type)), creation_time)

    def displayable(self) -> Dict[str, Any]:
        return {
            "id": f"{self.product_id:x}",
        }


class Product(abc.ABC):
    def __init__(self, info: ProductInfo) -> None:
        super().__init__()
        self._info = info

    @property
    def info(self) -> ProductInfo:
        return self._info

    @classmethod
    def from_path(cls, path: Path) -> "Product":
        product_info = ProductInfo.from_path(path)

        products: Dict[ProductType, type] = {
            ProductType.COMMAND_ERROR: CommandErrorProduct,
            ProductType.RAW: RawProduct,
            ProductType.FIG_OPERATION_ERROR: FigOperationErrorProduct,
            ProductType.FIG_PRODUCT: FigProduct,
        }

        if product_info.product_type not in products:
            raise ValueError(
                f"Unsupported product type: {product_info.product_type}")

        content = path.read_bytes()
        return products[product_info.product_type].from_data(product_info, content)

    @classmethod
    @abc.abstractmethod
    def from_data(cls, info: ProductInfo, data: bytes) -> "Product":
        pass

    def displayable(self) -> dict[str, Any]:
        base_properties = {
            "id": f"{self._info.product_id:x}",
            "creation_time": self._info.creation_time,
            "formatted_type": self._formatted_type,
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
        return PRODUCT_TYPE_TO_STRING[self._info.product_type]

    @property
    def _formatted_type(self) -> str:
        return PRODUCT_TYPE_TO_STRING[self._info.product_type]


class CommandErrorProduct(Product):
    def __init__(self, info: ProductInfo, error_code: int) -> None:
        super().__init__(info)
        self._error_code = error_code

    @classmethod
    def from_data(cls, info: ProductInfo, data: bytes):
        value, = struct.unpack("<I", data)
        return CommandErrorProduct(info, value)

    @property
    def _displayable_properties(self) -> Dict[str, Any]:
        return {
            "value": self._error_code,
            "hex": f"0x{self._error_code:08X}",
            "binary": f"0b{self._error_code:032b}"
        }


class RawProduct(Product):
    def __init__(self, info: ProductInfo, error_code: int) -> None:
        super().__init__(info)
        self._error_code = error_code

    @classmethod
    def from_data(cls, info: ProductInfo, data: bytes):
        return RawProduct(info, data)

    @property
    def _displayable_properties(self) -> Dict[str, Any]:
        return {
            "value": self._error_code,
            "hex": f"0x{self._error_code:08X}",
            "binary": f"0b{self._error_code:032b}"
        }


class FigOperationErrorProduct(Product):
    def __init__(self, info: ProductInfo, fig_id: int, operation_id: int, fig_specific_code: int) -> None:
        super().__init__(info)
        self._fig_id = fig_id
        self._operation_id = operation_id
        self._fig_specific_code = fig_specific_code

    @classmethod
    def from_data(cls, info: ProductInfo, data: bytes):
        fig_id, operation_id, fig_specific_code, = struct.unpack(
            '<III', data)
        return FigOperationErrorProduct(info, fig_id, operation_id, fig_specific_code)

    @property
    def _displayable_properties(self) -> Dict[str, Any]:
        return {
            'fig id': self._fig_id,
            'operation id': self._operation_id,
            'fig specific code': self._fig_specific_code
        }


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

    @property
    def type(self) -> None:
        return self._type

    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        FORMAT = "<II"
        product_type, content_length = struct.unpack(
            FORMAT, data[:struct.calcsize(FORMAT)])
        product_type = TypedProductType(product_type)
        content = data[struct.calcsize(FORMAT): struct.calcsize(FORMAT) + content_length]
        
        products: Dict[TypedProductType, type] = {
            TypedProductType.RAW: RawTypedProduct,
            TypedProductType.TEXT: TextTypedProduct,
            TypedProductType.IMAGE_BMP: ImageTypedProduct,
            TypedProductType.IMAGE_PNG: ImageTypedProduct,
        }
        
        if product_type not in products:
            raise ValueError(f"Invalid TypedProduct type: {product_type}")

        return products[product_type].from_raw_bytes(content)

    @property
    @abc.abstractmethod
    def _displayable_properties(self) -> dict[str, Any]:
        pass
    
    @property
    def _display_type(self) -> str:
        return TYPED_PRODUCT_TYPE_TO_STRING[self._type]

    @property
    def _formatted_type(self) -> str:
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
    def text(self) -> str:
        return self._text

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
    def _image_format(cls) -> str:
        pass

    @property
    def _displayable_properties(self):
        raw_image = io.BytesIO()
        self._image.save(raw_image, format=self._image_format().upper())
        encoded_image = base64.b64encode(raw_image.getvalue()).decode("utf-8")

        return {
            "data": f"data:{self._display_type};base64,{encoded_image}",
            "width": self._image.width,
            "height": self._image.height,
            "mode": self._image.mode
        }
    
    @classmethod
    def from_raw_bytes(cls, data: bytes) -> "TypedProduct":
        image_formats = {
            BmpImageTypedProduct._image_format().upper(): BmpImageTypedProduct,
            PngImageTypedProduct._image_format().upper(): PngImageTypedProduct,
        }

        try:
            image = Image.open(io.BytesIO(data))
        except Exception as e:
            raise ValueError(f"Failed to parse image: {e}")

        if image.format not in image_formats:
            raise ValueError(
                f"Unsupported image format: {BmpImageTypedProduct._image_format()}")

        return image_formats[image.format](image)

    @property
    def _display_type(self) -> str:
        return f"image/{self._image_format()}"


class BmpImageTypedProduct(ImageTypedProduct):
    def __init__(self, image: ImageFile):
        super().__init__(image, TypedProductType.IMAGE_BMP)
        
    @classmethod
    def _image_format(cls) -> str:
        return "bmp"

class PngImageTypedProduct(ImageTypedProduct):
    def __init__(self, image: ImageFile):
        super().__init__(image, TypedProductType.IMAGE_PNG)
        
    @classmethod
    def _image_format(cls) -> str:
        return "png"


class FigProduct(Product):
    def __init__(self, info: ProductInfo, fig_id: int, operation_id: int, operation_type: int, typed_product: TypedProduct) -> None:
        super().__init__(info)
        self._fig_id = fig_id
        self._operation_id = operation_id
        self._operation_type = operation_type
        self._typed_product = typed_product

    @property
    def product(self) -> TypedProduct:
        return self._typed_product

    @property
    def fig_id(self) -> int:
        return self._fig_id

    @property
    def fig_operation_id(self) -> int:
        return self._operation_id

    @property
    def fig_operation_type(self) -> int:
        return self._operation_type

    @classmethod
    def from_data(cls, info: ProductInfo, data: bytes) -> "FigProduct":
        FORMAT = "<III"
        fig_header_size = struct.calcsize(FORMAT)
        fig_id, operation_id, operation_type = struct.unpack(
            FORMAT, data[:fig_header_size])
        return FigProduct(info, fig_id, operation_id, operation_type, TypedProduct.from_raw_bytes(data[fig_header_size:]))

    @property
    def _displayable_properties(self) -> Dict[str, Any]:
        return {
            **self._typed_product._displayable_properties,
            "fig": f"{get_fig(self._fig_id).name} ({self._fig_id})",
            "operation type": f"{format_operation_name(self._fig_id, self._operation_type)} ({self._operation_type})",
            "operation id": f"{self._operation_id:x}",
        }

    @property
    def _formatted_type(self) -> str:
        return f"{PRODUCT_TYPE_TO_STRING[self._info.product_type]} - {format_operation_name(self.fig_id, self.fig_operation_type)} ({self._typed_product._formatted_type})"

    @property
    def _display_type(self) -> str:
        return self._typed_product._display_type
