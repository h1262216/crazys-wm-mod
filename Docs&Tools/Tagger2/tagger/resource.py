from dataclasses import dataclass, field
from typing import List, Optional, Union, Dict
from pathlib import Path
from xml.etree import ElementTree as ET
FilePath = Union[Path, str]


_OPT_TO_BOOL = {
    "yes": True, "no": False
}


@dataclass
class ImageResource:
    file: str
    type: str
    pregnant: bool
    outfit: str
    fallback: bool = False
    source: Optional[str] = None
    comment: Optional[str] = None
    style: Optional[str] = None


@dataclass
class ResourcePack:
    path: Path
    comment: str = ''
    images: List[ImageResource] = field(default_factory=list)


@dataclass
class TagSpec:
    description: str
    tag: str
    display: str
    fallback: List[str] = field(default_factory=list)
    patterns: List[str] = field(default_factory=list)


def read_tag_specs(source: FilePath) -> Dict[str, TagSpec]:
    doc = ET.parse(source).getroot()

    specs = {}
    for image_type in doc.findall('Type'):
        print(image_type)
        name = image_type.attrib['Name'].lower()
        spec = TagSpec(tag=name, description="", display=name)
        for child in image_type:
            if child.tag == "Display":
                spec.display = child.text
            elif child.tag == "Fallback":
                spec.fallback.append(child.text)
            elif child.tag == "Pattern":
                spec.patterns.append(child.text)
        if len(spec.patterns) == 0:
            spec.patterns.append(spec.tag + ".*")
        specs[name] = spec
    return specs


def load_image(node) -> ImageResource:
    a = node.attrib
    return ImageResource(
        file=a["File"], type=a["Type"], pregnant=_OPT_TO_BOOL[a["Pregnant"]],
        fallback=_OPT_TO_BOOL[a.get("Fallback", "no")],
        source=a.get("Source", None), comment=a.get("Comment", None),
        style=a.get("Style", None), outfit=a.get("Outfit", "none")
    )


def load_image_pack(path) -> ResourcePack:
    doc = ET.parse(path).getroot()
    images = []
    for image in doc.findall('Image'):
        images.append(load_image(image))

    return ResourcePack(path=Path(path), images=images)


def guess_type_by_file_name(file_name: str, tags: dict):
    import re
    file_name = Path(file_name).name.lower()
    for tag in tags.values():
        for pattern in tag.patterns:
            p = re.compile(pattern)
            if p.match(file_name):
                return tag.tag
    return ""


def _image_resource_dict(image: ImageResource):
    d = {"File": image.file, "Type": image.type, "Pregnant": "yes" if image.pregnant else "no",
         "Outfit": image.outfit}
    if image.source is not None:
        d["Source"] = image.source
    if image.comment is not None:
        d["Comment"] = image.comment
    if image.style is not None:
        d["Style"] = image.style
    if image.fallback:
        d["Fallback"] = "yes"
    return d


def save_image_pack(path, pack: ResourcePack):
    root = ET.Element("Images")
    for image in pack.images:  # type: ImageResource
        im_el = ET.SubElement(root, "Image", _image_resource_dict(image))

    #tree = ET.ElementTree(root)
    # tree.indent(2)
    #tree.write(path)
    from xml.dom import minidom

    xmlstr = minidom.parseString(ET.tostring(root, encoding="unicode", xml_declaration=True)).toprettyxml(indent="   ")
    with open(path, "w") as f:
        f.write(xmlstr)


