import re
import argparse
from pathlib import Path
from dataclasses import asdict
from collections import OrderedDict
from typing import Dict, Optional

from PoopBiter.fig import FIGS_METADATA_PATH, RELEASES_PATH, Fig
from PoopBiter.utils import dump_pretty_json, format_exception, is_int, to_snake_case


ERROR_CODE_FILENAME = "error_code.py"
FIG_CODE_FILENAME = "fig_code.py"


def find_api_files(root: Path):
    return [p for p in root.rglob("Api.hpp")]


def get_error_code_path(root: Path) -> Path:
    return root / "Framework" / "Include" / "ErrorCode.hpp"


def get_fig_code_path(root: Path) -> Path:
    return root / "Fig" / "Include" / "FigApi.hpp"


def strip_comments(data: str) -> str:
    CPP_COMMENTED_LINE_PREFIX = "//"
    return "\n".join(line for line in data.splitlines() if not line.strip().startswith(CPP_COMMENTED_LINE_PREFIX))


def parse_enum(block: str, enum_name: str) -> Dict[int, str]:
    enum_match = re.search(
        rf'enum class {enum_name}'r'\s*:\s*.*?\s*{(.*?)};', block, re.DOTALL)

    if not enum_match:
        raise ValueError(f"failed to find enum {enum_name} in code block")

    enum_body = enum_match.group(1)
    entries = OrderedDict()
    current_value = 0

    for line in strip_comments(enum_body).split(','):
        line = line.strip()
        if not line:
            continue

        if '=' in line:
            name, value = map(str.strip, line.split('='))
            if not is_int(value):
                raise ValueError(
                    f"cannot parse enum value '{line}' for {enum_name}")
            current_value = int(value)
        else:
            name = line

        entries[current_value] = name
        current_value += 1

    return entries


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

    result = {
        "name": fig_name,
        "dll_path": str(RELEASES_PATH / f"{fig_name}.dll")
    }

    for value, regex in vars_regex.items():
        match = re.search(regex, block)
        if match:
            result[value] = int(match.group(1))
        else:
            print(f"{value} not found, skipping {fig_name}")
            return None

    try:
        enum_fields = parse_enum(block, "OperationType")
    except Exception as ex:
        print(
            f"failed to parse enum OperationType, skipping {fig_name}: {format_exception(ex)}")
        return None

    result["operations"] = enum_fields

    return Fig.from_dict(result)


def format_python_enum(name: str, fields: Dict[int, str]) -> str:
    return "\n\t".join([f"import enum\n\n\nclass {name}(enum.Enum):", *[f"{value} = {key}" for key, value in fields.items()]]) + "\n"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        "Parse C++ files and generate a json for interpreting raw metadata")

    parser.add_argument("--root", type=Path, default=Path("."),
                        help="where to look for figs")
    parser.add_argument("-o", "--output", type=Path, default=FIGS_METADATA_PATH.parent,
                        help="output folder for metadata files")

    return parser.parse_args()


def main():
    args = parse_args()
    figs_json_ouput = args.output / FIGS_METADATA_PATH.name
    figs_py_ouput = (args.output / FIGS_METADATA_PATH.name).with_suffix(".py")
    error_code_ouput = args.output / ERROR_CODE_FILENAME
    fig_code_ouput = args.output / FIG_CODE_FILENAME

    figs = list(map(parse_api_file, find_api_files(args.root)))
    result = {
        "figs": [
            asdict(fig) for fig in figs if fig is not None
        ]
    }

    json_output = dump_pretty_json(result)
    python_output = ''.join(
        [f"{to_snake_case(fig.name).upper()}_ID = {fig.fig_id}\n" for fig in figs])

    figs_json_ouput.write_text(json_output)
    print(f"figs json written to '{figs_json_ouput}'")

    figs_py_ouput.write_text(python_output)
    print(f"figs python written to '{figs_py_ouput}'")

    ERROR_CODE_ENUM_NAME = "ErrorCode"
    error_code_python_output = format_python_enum(ERROR_CODE_ENUM_NAME, parse_enum(
        get_error_code_path(args.root).read_text(), ERROR_CODE_ENUM_NAME))

    error_code_ouput.write_text(error_code_python_output)
    print(f"error code python written to '{error_code_ouput}'")

    FIG_CODE_ENUM_NAME = "FigCode"
    error_code_python_output = format_python_enum(FIG_CODE_ENUM_NAME, parse_enum(
        get_fig_code_path(args.root).read_text(), FIG_CODE_ENUM_NAME))

    fig_code_ouput.write_text(error_code_python_output)
    print(f"fig code python written to '{fig_code_ouput}'")


if __name__ == "__main__":
    main()
