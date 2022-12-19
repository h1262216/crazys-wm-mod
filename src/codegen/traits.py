# This script reads in all the traits from the given directory, and generates a C++ header
# defining these as constants from it

import sys
from pathlib import Path
from xml.etree import ElementTree as ET
from collections import namedtuple
import textwrap

TraitSpec = namedtuple("TraitSpec", ["name", "description", "modifiers"])


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


def name_as_identifier(trait_name: str):
    return trait_name.upper().replace(" ", "_").replace("-", "_").replace(".", "_").replace(":", "_")


# TODO what about tag:?
_MODIFIER_PREFIXED = ["stat:", "skill:", "enjoy:", "fetish:", "skill-cap:", "sex:"]


def is_generic_modifier(modifier: str):
    for p in _MODIFIER_PREFIXED:
        if modifier.startswith(p):
            return False
    return True


def format_description(description: str):
    description = textwrap.wrap(description, width=80)
    description = "\n".join(description)
    return description.replace("\n", "\n    /// ")


def main():
    result_path = Path(sys.argv[1])
    search_path = Path(sys.argv[2])
    result_path.parent.mkdir(exist_ok=True, parents=True)

    parser = TraitsParser()

    lua_text = "--- This file is automatically generated. Do not change, modifications will be overwritten.\n\n"

    out_text = "/// This file is automatically generated. Do not change, modifications will be overwritten.\n\n"
    out_text += "#pragma once\n\n"
    out_text += "namespace traits {\n"
    lua_text += "--- @class wm.TRAITS\n"

    xml_file_list = sorted(search_path.glob("*.xml"))

    for traits_file in xml_file_list:
        try:
            parser.parse_file(traits_file)
        except Exception as E:
            print(f"Error parsing file '{traits_file}'")
            raise


    for traits_file in parser.traits:
        out_text += f"\n\n    // {traits_file}\n"
        for trait in parser.traits[traits_file]:
            newline = "\n"
            if trait.description is not None:
                out_text += f"    /// {format_description(trait.description)}\n"
            out_text += f"    constexpr const char* {name_as_identifier(trait.name)} = \"{trait.name}\";\n\n"
            lua_text += f"--- @field {name_as_identifier(trait.name)} string {trait.description.replace(newline, ' ')}\n"

    # now the modifiers
    out_text += "    namespace modifiers {\n"

    for mod in sorted(parser.modifiers):
        if is_generic_modifier(mod):
            out_text += f"        constexpr const char* {name_as_identifier(mod)} = \"{mod}\";\n"

    # finally, the groups
    out_text += "    }\n    namespace groups {\n"

    for group in sorted(parser.groups):
        if group != "":
            out_text += f"        constexpr const char* {name_as_identifier(group)} = \"{group}\";\n"

    out_text += "    }\n}\n"

    # only write to the file if anything actually changes.
    if not result_path.exists() or result_path.read_text() != out_text:
        result_path.write_text(out_text)

    # for some reason, this helps to improve the autocomplete
    lua_text += "local t = {}\nwm.TRAITS = t"

    lua_path = Path(sys.argv[3])
    lua_path.write_text(lua_text)


if __name__ == "__main__":
    main()
