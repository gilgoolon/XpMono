from PassionFruit.backend.app import app
from PoopBiter import logger


logger.info(f"starting app...")
app.run(host='0.0.0.0', port=5000, debug=True)
