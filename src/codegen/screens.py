# This script reads the interfaces from a directory, and generates helper headers
import sys
from pathlib import Path
from typing import Union

from wmlib import prepare_xml, make_camel_identifier, update_file_if_changed
from xml.etree import ElementTree as ET


header_template = """
class {class_name} : public {base_class} 
{{
    public:
        {class_name}();
        
        static std::shared_ptr<cInterfaceWindow> create();
        
    private:
        void process() override {{ }};
        void set_ids() override;
        
        virtual void setup_callbacks() = 0;

    protected:
{dcl_ids_code}
}};
"""


impl_template = """
{class_name}::{class_name}() : {base_class}("{file_name}") {{ }};

void {class_name}::set_ids() {{
{set_ids_code}

    setup_callbacks();
}}
"""


game_screens = {
    "TransferGirls", "PreparingGameScreen", "PrisonScreen"
}


def read_interface_file(file: Union[str, Path]):
    file = Path(file)
    xml = prepare_xml(file.read_text())
    doc = ET.fromstring(xml)

    elements = {}

    class_base_name = make_camel_identifier(file.stem)
    class_name = "c" + class_base_name + "Base"

    base_class = "cGameWindow" if class_base_name in game_screens else "cInterfaceWindowXML"

    for child in doc:
        if child.tag == "Window":
            continue

        if "Name" in child.attrib:
            name = child.attrib["Name"]
            if name == "" or name.startswith("_"):
                continue

            ident = make_camel_identifier(name)
            ident = ident.replace("Button", "Btn")
            elements["m_" + ident + "_id"] = name

    dcl_ids_code = "\n".join(map(lambda x: f'        int {x:20} = -1;', sorted(elements.keys())))
    set_ids_code = "\n".join(map(lambda x: f'    {x[0]:20} = get_id("{x[1]}");', sorted(elements.items())))

    class_header = header_template.format(class_name=class_name, file_name=file.name, dcl_ids_code=dcl_ids_code,
                                          base_class=base_class)

    class_impl = impl_template.format(class_name=class_name, file_name=file.name, set_ids_code=set_ids_code,
                                      base_class=base_class)

    return class_header, class_impl


read_interface_file("/home/erik/CLionProjects/cwm/crazys-wm-mod/Resources/Interface/Light-16-9/player_office_screen.xml")


def main():
    result_path = Path(sys.argv[1])
    search_path = Path(sys.argv[2])
    result_path.parent.mkdir(exist_ok=True, parents=True)

    xml_file_list = sorted(search_path.glob("*.xml"))

    header_file = "/// This file is automatically generated. Do not change, modifications will be overwritten.\n\n"
    header_file += "#pragma once\n\n"
    header_file += '#include "interface/cInterfaceWindowXML.h"\n'
    header_file += '#include "cGameWindow.h"\n'
    header_file += '#include <memory>\n\n'
    header_file += "namespace screens {\n"

    impl_file = "/// This file is automatically generated. Do not change, modifications will be overwritten.\n\n"
    impl_file += '#include "ScreenBase.h"\n\n'
    impl_file += "using namespace screens;\n\n"

    for screen in xml_file_list:
        header, impl = read_interface_file(screen)
        header_file += "    ".join(header.splitlines(keepends=True))
        impl_file += impl

    header_file += "}\n"

    update_file_if_changed(result_path, impl_file)

    header_path = result_path.with_suffix(".h")
    update_file_if_changed(header_path, header_file)


if __name__ == "__main__":
    main()
