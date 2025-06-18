
from dataclasses import dataclass
import re
from typing import Dict, List, Optional, Tuple
from more_itertools import peekable


def _line_not_blank(line: Optional[str]) -> bool:
    return line is not None and len(line) > 0


def _skip_if_has_next(lines: peekable) -> None:
    if lines.peek(None) is not None:
        next(lines)


@dataclass
class ProductObject:
    object_type: str
    fields: Dict[str, str]
    
    @classmethod
    def from_lines(cls, lines: peekable) -> "ProductObject":
        OBJECT_TYPE_PREFIX = '#'
        
        object_type_line = next(lines)
        if not object_type_line.startswith(OBJECT_TYPE_PREFIX):
            raise ValueError(f"Invalid object type line: {object_type_line}")
        
        object_type = object_type_line.removeprefix(OBJECT_TYPE_PREFIX)
        fields = {}
        
        while _line_not_blank(lines.peek(None)):
            key, value = cls._parse_field(next(lines))
            fields[key] = value
        
        _skip_if_has_next(lines)
        
        return cls(object_type, fields)
    
    @classmethod
    def _parse_field(cls, line: str) -> Tuple[str, str]:
        matches = re.match(r"^\s*-\s*(.+?):\s*(.*)$", line)

        if not matches or len(matches.groups()) != 2:
            raise ValueError(f"Invalid field line: {line}")
        
        key, value = matches.groups()
        return (key, value)


@dataclass
class ProductObjectsSection:
    name: str
    objects: List[ProductObject]
    
    @classmethod
    def from_lines(cls, lines: peekable) -> "ProductObjectsSection":
        section_name_line = next(lines)
        matches = re.search(r"\[(.*?)\]", section_name_line)
        
        if not matches or len(matches.groups()) != 1:
            raise ValueError(f"Invalid section line: {section_name_line}")
        
        section_name = matches.group(1)
        objects = []
        
        while _line_not_blank(lines.peek(None)):
            objects.append(ProductObject.from_lines(lines))
        
        _skip_if_has_next(lines)
        
        return cls(section_name, objects)
    

def parse_structured_product(data: str) -> List[ProductObjectsSection]:
    lines = peekable(data.splitlines())
    sections = []
    
    while lines.peek(None) is not None:
        sections.append(ProductObjectsSection.from_lines(lines))
    
    return sections
