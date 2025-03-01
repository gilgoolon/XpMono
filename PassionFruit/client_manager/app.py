from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
import requests
import os
import base64
import struct
import magic
import io
from PIL import Image
import json
from pathlib import Path

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

def parse_product_content(product_type: int, content: bytes):
    """Parse the product content based on its format.
    Currently handles uint32 values, but can be extended for other formats."""
    if product_type == 0:
        value, = struct.unpack('<I', content)
        return {
            'type': 'Command Error (uint32)',
            'value': value,
            'hex': f'0x{value:08X}',  # Hexadecimal representation
            'binary': f'0b{value:032b}'  # Binary representation
        }

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
            # Handle products as a list
            for product in client_data['products']:
                print(f"Processing product: {product}")  # Debug log
                # Get product content from FastAPI
                try:
                    path = Path(product)
                    data = path.read_bytes()
                    PRODUCT_TYPE_SEPARATOR = '-'
                    product_type = int(path.stem.split(PRODUCT_TYPE_SEPARATOR)[-1])
                    parsed_products[product] = parse_product_content(product_type, data)
                except Exception as e:
                    print(f"Exception while processing {product}: {str(e)}")  # Debug log
                    parsed_products[product] = {
                        'type': 'error',
                        'error': f'Error processing product: {str(e)}'
                    }
            
            client_data['parsed_products'] = parsed_products
            print(f"Final parsed products: {parsed_products}")  # Debug log
        
        return jsonify(client_data), response.status_code
    except requests.exceptions.RequestException as e:
        print(f"Request exception: {str(e)}")  # Debug log
        return jsonify({'error': str(e)}), 500

@app.route('/api/products/<path:product_path>', methods=['GET'])
def get_product(product_path):
    try:
        # Get the product path from FastAPI
        response = requests.get(f'{CHERRY_URL}/get-product-path/{product_path}')
        if response.status_code != 200:
            return jsonify({'error': 'Product not found'}), 404
            
        file_path = response.json().get('path')
        if not file_path or not os.path.exists(file_path):
            return jsonify({'error': 'Product file not found'}), 404

        # Use python-magic to detect file type
        mime = magic.Magic(mime=True)
        content_type = mime.from_file(file_path)

        # Handle different file types
        if content_type.startswith('image/'):
            # Process image files
            try:
                with Image.open(file_path) as img:
                    # Convert to RGB if necessary
                    if img.mode in ('RGBA', 'LA'):
                        background = Image.new('RGB', img.size, 'white')
                        background.paste(img, mask=img.split()[-1])
                        img = background
                    elif img.mode != 'RGB':
                        img = img.convert('RGB')
                    
                    # Save to bytes
                    img_io = io.BytesIO()
                    img.save(img_io, 'JPEG', quality=85)
                    img_io.seek(0)
                    
                    return send_file(
                        img_io,
                        mimetype='image/jpeg',
                        as_attachment=False
                    )
            except Exception as e:
                return jsonify({'error': f'Error processing image: {str(e)}'}), 500

        elif content_type == 'application/json' or content_type.startswith('text/'):
            # Process text and JSON files
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                if content_type == 'application/json':
                    # Validate and format JSON
                    try:
                        parsed = json.loads(content)
                        return jsonify(parsed)
                    except json.JSONDecodeError:
                        return jsonify({'error': 'Invalid JSON file'}), 400
                else:
                    # Return text content with proper content type
                    return content, 200, {'Content-Type': content_type}
            except Exception as e:
                return jsonify({'error': f'Error reading file: {str(e)}'}), 500

        else:
            # For binary files, return with proper content type
            try:
                return send_file(
                    file_path,
                    mimetype=content_type,
                    as_attachment=True,
                    download_name=os.path.basename(file_path)
                )
            except Exception as e:
                return jsonify({'error': f'Error sending file: {str(e)}'}), 500

    except requests.exceptions.RequestException as e:
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
