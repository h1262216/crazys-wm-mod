# This script reads in all the traits from the given directory, and generates a C++ header
# defining these as constants from it

import sys
from pathlib import Path
import textwrap
from wmlib import TraitsParser, make_uc_identifier, update_file_if_changed

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

    all_trait_names = set()

    for traits_file in xml_file_list:
        try:
            parser.parse_file(traits_file)
        except Exception as E:
            print(f"Error parsing file '{traits_file}'", file=sys.stderr)
            raise

    for traits_file in parser.traits:
        out_text += f"\n\n    // {traits_file}\n"
        for trait in parser.traits[traits_file]:
            newline = "\n"
            if trait.description is not None:
                out_text += f"    /// {format_description(trait.description)}\n"
            out_text += f"    constexpr const char* {make_uc_identifier(trait.name)} = \"{trait.name}\";\n\n"
            lua_text += f"--- @field {make_uc_identifier(trait.name)} string {trait.description.replace(newline, ' ')}\n"

    # now the modifiers
    out_text += "    namespace modifiers {\n"

    for mod in sorted(parser.modifiers):
        if is_generic_modifier(mod):
            out_text += f"        constexpr const char* {make_uc_identifier(mod)} = \"{mod}\";\n"

    # finally, the groups
    out_text += "    }\n    namespace groups {\n"

    for group in sorted(parser.groups):
        if group != "":
            out_text += f"        constexpr const char* {make_uc_identifier(group)} = \"{group}\";\n"

    out_text += "    }\n}\n"

    update_file_if_changed(result_path, out_text)

    # for some reason, this helps to improve the autocomplete
    lua_text += "local t = {}\nwm.TRAITS = t"

    lua_path = Path(sys.argv[3])
    lua_path.write_text(lua_text)


if __name__ == "__main__":
    main()
