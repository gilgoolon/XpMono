import argparse
from dataclasses import asdict, dataclass
import os
import re
import json
from pathlib import Path
from collections import OrderedDict
from typing import Dict, List, Optional

from PoopBiter.utils import dump_pretty_json, is_int


@dataclass
class Fig:
    name: str
    fig_id: int
    version_major: int
    version_minor: int
    operations: Dict[int, str]


def find_api_files(root: Path):
    return [p for p in root.rglob("Api.hpp")]


def parse_api_file(api_path: Path) -> Optional[Fig]:
    content = api_path.read_text()
    fig_name = api_path.parent.stem

    # Look inside the Api namespace
    api_namespace = re.search(r'namespace\s+Api\s*{(.*)}', content, re.DOTALL)
    if not api_namespace:
        return None

    block = api_namespace.group(1)

    # Extract variables
    vars_regex = {
        "fig_id": r'Fig::FigId FIG_ID\s*=\s*(\d+);',
        "version_major": r'Fig::VersionMajor VERSION_MAJOR\s*=\s*(\d+);',
        "version_minor": r'Fig::VersionMinor VERSION_MINOR\s*=\s*(\d+);',
    }

    result = {"name": fig_name}

    for var, regex in vars_regex.items():
        match = re.search(regex, block)
        if match:
            result[var] = int(match.group(1))
        else:
            print(f"{value} not found, skipping {fig_name}")
            return None

    # Extract enum class OperationType
    enum_match = re.search(
        r'enum class OperationType\s*:\s*Fig::OperationType\s*{(.*?)};', block, re.DOTALL)
    if not enum_match:
        print(f"OperationType enum not found, skipping {fig_name}")
        return None

    enum_body = enum_match.group(1)
    entries = OrderedDict()
    current_value = 0

    for line in enum_body.split(','):
        line = line.strip()
        if not line:
            continue

        if '=' in line:
            name, value = map(str.strip, line.split('='))
            if not is_int(value):
                print(
                    f"cannot parse enum value '{line}' skipping {fig_name}")
                return None
            current_value = int(value)
        else:
            name = line

        entries[current_value] = name
        current_value += 1

    result["operations"] = entries

    return Fig(**result)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        "Parse C++ Figs and generate a json for interpreting raw fig metadata")

    parser.add_argument("--root", type=Path, default=Path("."),
                        help="where to look for figs")
    parser.add_argument("-o", "--output", type=Path, default=Path("PoopBiter") / "figs.json",
                        help="where to look for figs")

    return parser.parse_args()


def main():
    args = parse_args()

    figs = map(parse_api_file, find_api_files(args.root))
    result = {
        "figs": [
            asdict(fig) for fig in figs if fig is not None
        ]
    }

    output = dump_pretty_json(result)
    args.output.write_text(output)
    print(f"output written to '{args.output.absolute()}'")


if __name__ == "__main__":
    main()
