import uvicorn
from Cherry.main import *


uvicorn.run(app, host="127.0.0.1", port=8000)
