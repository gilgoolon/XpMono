from pathlib import Path

from PoopBiter.parsing import parse_structured_product
from PoopBiter.products import FigProduct, TypedProduct


path = Path("CornCake") / "products" / "f7b1b753" / "4c96db12" / "33d76792-3"

product = FigProduct.from_path(path)

text = product.product.text
sections = parse_structured_product(text)
print(sections)
