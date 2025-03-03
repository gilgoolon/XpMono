from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
import requests
import os
import base64
import struct
import magic
import io
from PIL import Image
from pathlib import Path
import json

import products

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
        
    elif product_type == products.ProductType.IMAGE_PNG:
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
                **base_result,
                'type': 'image/png',
                'data': f'data:image/png;base64,{img_base64}',
                'width': img.width,
                'height': img.height,
                'mode': img.mode
            }
        except Exception as e:
            raise ValueError(f"Invalid PNG image: {str(e)}")

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
    templates_dir = os.path.join(os.path.dirname(__file__), 'command-templates')
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
    app.run(host='0.0.0.0', port=5000)
