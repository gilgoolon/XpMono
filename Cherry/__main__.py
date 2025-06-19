import uvicorn
from Cherry.main import *
from PoopBiter import logger


logger.info(f"starting app...")
uvicorn.run(app, host="127.0.0.1", port=8000)
