from PassionFruit.backend.app import socketio, app
from PoopBiter import logger


logger.info(f"starting app...")
socketio.run(app, host='0.0.0.0', port=5000, debug=True)
