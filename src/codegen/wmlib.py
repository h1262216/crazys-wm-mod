from pathlib import Path
from xml.etree import ElementTree as ET
from collections import namedtuple

TraitSpec = namedtuple("TraitSpec", ["name", "description", "modifiers"])


def parse_trait(trait: ET.Element):
    name = trait.attrib["Name"]
    description = None
    modifiers = set()

    for child in trait:
        if child.tag == "description":
            if description is None:
                description = child.text
            else:
                print(f"Warning: Duplicate description for trait {name}")
        elif child.tag == "Modifiers":
            for mod in child:
                modifiers.add(parse_modifier(mod))
        elif child.tag == "Excludes":
            pass
        elif child.tag == "Properties":
            pass
        else:
            print(f"Warning: Unknown element {child.tag} for trait {name}")

    return TraitSpec(name=name, description=description, modifiers=modifiers)


def parse_modifier(modifier: ET.Element):
    return modifier.attrib["Name"]


def parse_group(group: ET.Element):
    return group.attrib.get("Name", "")


class TraitsParser:
    def __init__(self):
        self.traits = dict()
        self.groups = set()
        self.modifiers = set()

    def parse_file(self, traits_file: Path):
        doc = ET.fromstring(traits_file.read_text())
        traits = []
        for child in doc:
            if child.tag == "Trait":
                trait = parse_trait(child)
                traits.append(trait)
                self.modifiers.update(trait.modifiers)
            elif child.tag == "Default":
                pass
            elif child.tag == "TraitGroup":
                self.groups.add(parse_group(child))
            elif child.tag == "Modifier":
                self.modifiers.add(parse_modifier(child))
            else:
                print("Warning: Unknown tag ", child.tag)

        self.traits[traits_file.name] = traits


def prepare_xml(xml: str):
    result = []
    quoted = False
    in_element = False
    for character in xml:
        if not in_element:
            result.append(character)
            if character == "<":
                in_element = True
        elif quoted:
            if character == "<":
                result.append("&lt;")
            elif character == ">":
                result.append("&gt;")
            else:
                result.append(character)
            if character == '"':
                quoted = False
        else:
            if character == ">":
                in_element = False
            if character == '"':
                quoted = True
            result.append(character)

    return "".join(result)