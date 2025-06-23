import requests


PASSIONFRUIT_API_URL = "http://localhost:5000/api"


def notify_new_client_product(client_id: int, product_id: int) -> None:
    requests.post(f"{PASSIONFRUIT_API_URL}/new-product/{client_id:x}?product_id={product_id:x}")
