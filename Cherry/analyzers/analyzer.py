import abc
from PoopBiter.products import Product, ProductInfo


class ProductAnalyzer(abc.ABC):
    @abc.abstractmethod
    def should_analyze_info(self, product_info: ProductInfo) -> bool:
        pass

    @abc.abstractmethod
    def should_analyze_product(self, product_info: ProductInfo, product: Product) -> bool:
        pass

    @abc.abstractmethod
    def analyze(self, product_info: ProductInfo, product: Product) -> None:
        pass
