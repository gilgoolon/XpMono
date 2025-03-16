from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
import requests
import os
import base64
import struct
import io
from PIL import Image
from pathlib import Path
import json

import products

CNC_ROOT = "../CornCake"
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

def parse_product_content(product_id: str, product_type: products.ProductType, content: bytes):
    """Parse the product content based on its format.
    Currently handles uint32 values and PNG images."""
    base_result = {
        'id': product_id,
        'formatted_type': products.format_product_type[product_type]
    }
    
    if product_type == products.ProductType.COMMAND_ERROR:
        value, = struct.unpack('<I', content)
        return {
            **base_result,
            'type': 'Command Error (uint32)',
            'value': value,
            'hex': f'0x{value:08X}',  # Hexadecimal representation
            'binary': f'0b{value:032b}'  # Binary representation
        }
        
    if product_type == products.ProductType.RAW:
        SHOWING_BYTES = 16
        displayed_bytes = content[:SHOWING_BYTES].ljust(SHOWING_BYTES, b'\0')
        return {
            **base_result,
            'type': 'Raw',
            f'value (first {SHOWING_BYTES} bytes)': f'0x{hex(displayed_bytes)}',
            'hex': f'0x{displayed_bytes:08X}',
        }
    
    if product_type == products.ProductType.FIG_OPERATION_ERROR:
        fig_id, operation_id, fig_specific_code, = struct.unpack('<III', content)
        return {
            **base_result,
            'type': 'Fig Operation Error',
            'fig id': fig_id,
            'operation id': operation_id,
            'fig specific code': fig_specific_code
        }
    
    if product_type == products.ProductType.FIG_PRODUCT:
        FORMAT = '<II'
        fig_id, operation_id, = struct.unpack(FORMAT, content[:struct.calcsize(FORMAT)])
        typed_product = content[struct.calcsize(FORMAT):]
        result = {
            **base_result,
            'fig id': fig_id,
            'operation id': operation_id,
        }
        result.update(products.parse_typed_product(typed_product))
        print(list(result.keys()))
        return result

    raise ValueError(f"Unsupported product type: {product_type}")


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
        
        # Parse each product's content
        if 'products' in client_data:
            parsed_products = {}
            product_paths = {}  # Map product IDs to their paths
            
            # Handle products as a list
            for product_path in client_data['products']:
                try:
                    path = Path(product_path)
                    data = path.read_bytes()
                    product_type = products.get_product_type(path)
                    product_id = products.get_product_id(path)
                    
                    parsed_products[product_id] = parse_product_content(product_id, product_type, data)
                    product_paths[product_id] = product_path
                except Exception as e:
                    print(f"Exception while processing product with type {product_type}: {str(e)}")  # Debug log
                    parsed_products[product_id] = {
                        'type': 'error',
                        'error': f'Error processing product: {str(e)}'
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

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
