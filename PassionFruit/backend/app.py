# fmt: off
import eventlet
eventlet.monkey_patch()

from collections import OrderedDict
from typing import Dict, Set

from flask import request
from flask_socketio import SocketIO
from flask_cors import CORS
from flask import Flask, send_from_directory, request, jsonify

import requests
import os
import base64
from pathlib import Path
import json

from PoopBiter.releases import list_releases
from PoopBiter.templates import list_templates
from PoopBiter.utils import format_exception
from PoopBiter import logger
from PoopBiter.fig import list_figs
from PoopBiter.products import PRODUCT_TYPE_TO_STRING, Product, ProductInfo

from PassionFruit.backend import transformer

# fmt: on


CNC_ROOT = "CornCake"

app = Flask(__name__, static_folder='frontend/build')
CORS(app, resources={
    r"/*": {
        "origins": "*",
        "methods": ["GET", "POST", "OPTIONS", "DELETE"],
        "allow_headers": ["Content-Type", "Authorization"]
    }
})
socketio = SocketIO(app, cors_allowed_origins="*")

client_id_to_sids: Dict[str, Set[str]] = {}
sid_to_client_id: Dict[str, str] = {}


CHERRY_URL = 'http://localhost:8000'


@socketio.on('register')
def handle_register(data):
    client_id = data.get('client_id')
    sid = request.sid

    if not client_id:
        return

    if client_id in client_id_to_sids:
        client_id_to_sids[client_id].add(sid)
    client_id_to_sids[client_id] = {sid}
    sid_to_client_id[sid] = client_id
    print(f"Registered client {client_id} with sid {sid}")


@socketio.on('unregister')
def handle_unregister(data):
    client_id = data.get('client_id')
    sid = request.sid

    if not client_id:
        return

    if client_id in client_id_to_sids and sid in client_id_to_sids:
        client_id_to_sids[client_id].remove(sid)
    sid_to_client_id.pop(sid, None)
    print(f"Registered client {client_id} with sid {sid}")


@socketio.on('connect')
def on_connect():
    print('Client connected')


@socketio.on('disconnect')
def on_disconnect():
    sid = request.sid
    client_id = sid_to_client_id.pop(sid, None)
    if client_id in client_id_to_sids:
        if sid in client_id_to_sids[client_id]:
            client_id_to_sids[client_id].remove(sid)
        print(f"Client {client_id} disconnected")


@app.route('/api/liver/<client_id>/new-product', methods=['POST'])
def notify_client_product(client_id):
    LOCAL_SERVICES = "127.0.0.1"
    if request.remote_addr != LOCAL_SERVICES:
        logger.warning(
            f"someone tried to notify clients from {request.remote_addr}")
        return

    product_id = request.args.get("product_id", None)

    for sid in client_id_to_sids.get(client_id, []):
        logger.info(f"notifying sid {sid}")
        socketio.emit('new_product', {"product_id": product_id}, to=sid)
    return {'status': 'success'}


@app.route('/api/liver/<client_id>', methods=['GET'])
def get_client(client_id):
    try:
        response = requests.get(f'{CHERRY_URL}/get-client/{client_id}')
        client_data = response.json()
        
        client_data['location'] = client_data.get('location', None)
        client_data['nickname'] = client_data.get('nickname', None)

        return jsonify(client_data), response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/liver/<client_id> failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/liver/<client_id>/products', methods=['GET'])
def get_products(client_id):
    try:
        response = requests.get(f'{CHERRY_URL}/get-client/{client_id}')
        client_data = response.json()

        product_paths = client_data.get("products", [])
        parsed_products = {}

        for product_path in product_paths:
            path = Path(product_path)
            try:
                info = ProductInfo.from_path(path)
                formatted_product_id = f"{info.product_id:x}"
                parsed_products[formatted_product_id] = info.displayable()
            except Exception as ex:
                logger.error(
                    f"failed to parse product info '{product_path}': {format_exception(ex)}")
                continue

            try:
                product = Product.from_path(path)
                parsed_products[formatted_product_id].update(
                    product.displayable())
            except Exception as ex:
                error_message = f"failed to parse product {info} at '{product_path}': {format_exception(ex)}"
                logger.error(error_message)
                parsed_products[formatted_product_id] = {
                    **info.displayable(),
                    f"formatted_type": f"{PRODUCT_TYPE_TO_STRING[info.product_type]} (Invalid)",
                    f"error": error_message
                }

        parsed_products = OrderedDict([product for product in sorted(
            parsed_products.items(), key=lambda product: product[1]["creation_time"], reverse=True)])

        client_data["products"] = parsed_products

        return jsonify(client_data), response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/liver/<client_id>/products failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/liver/<client_id>/set-nickname', methods=['POST'])
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
        error_message = f"endpoint /api/liver/<client_id>/set-nickname failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/liver/<client_id>/delete-product', methods=['DELETE'])
def delete_product(client_id):
    try:
        product_name = request.args.get('product_name', None)
        command_id = request.args.get('command_id', None)
        if product_name is None or command_id is None:
            raise ValueError(
                "pass product details through query parameters 'product_name' and 'command_id'")

        response = requests.post(
            f'{CHERRY_URL}/delete-product/{client_id}?command_id={command_id}&product_name={product_name}')
        return jsonify({}), response.status_code
    except Exception as ex:
        error_message = f"endpoint /api/liver/<client_id>/delete-product failed for client id {client_id}: {format_exception(ex)}"
        logger.error(error_message)
        return jsonify({"error": error_message}), 500


@app.route('/api/liver/<client_id>/send-command', methods=['POST', 'OPTIONS'])
def send_command(client_id):
    try:
        command_data = request.json.get('command_data', '')

        parsed_command_data = json.loads(command_data)
        transformer.transform_command(parsed_command_data)

        command_data = json.dumps(parsed_command_data)

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


@app.route('/api/livers', methods=['GET'])
def get_clients():
    try:
        response = requests.get(f'{CHERRY_URL}/get-clients')
        return jsonify(response.json()), response.status_code
    except requests.exceptions.RequestException as e:
        return jsonify({'error': str(e)}), 500


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
