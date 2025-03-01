
from glob import glob
import os
from pathlib import Path
from typing import List

def get_client_products(root: Path, client_id: int) -> List[str]:
    expression = root / "products" / f"{client_id:x}" / "*" / "*"
    print(expression)
    return [Path(path).absolute().as_posix() for path in glob(expression.as_posix()) if os.path.isfile(path)]
