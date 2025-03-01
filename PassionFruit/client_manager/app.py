from flask import Flask, send_from_directory, request, jsonify
from flask_cors import CORS
import requests
import os

app = Flask(__name__, static_folder='frontend/build')
CORS(app)

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
        return jsonify(response.json()), response.status_code
    except requests.exceptions.RequestException as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/commands', methods=['POST'])
def send_command():
    try:
        response = requests.post(f'{CHERRY_URL}/send-command', json=request.json)
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
