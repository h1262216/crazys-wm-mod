import sys
from collections import defaultdict
from pathlib import Path
from xml.etree import ElementTree as ET
from wmlib import TraitsParser, prepare_xml

data_path = Path(sys.argv[1])

parser = TraitsParser()
traits_file_list = sorted((data_path / "Traits").glob("*.xml"))

for traits_file in traits_file_list:
    try:
        parser.parse_file(traits_file)
    except Exception as E:
        print(f"Error parsing file '{traits_file}'", file=sys.stderr)
        raise

all_trait_names = set()
for traits in parser.traits.values():
    for trait in traits:
        all_trait_names.add(trait.name)
print(all_trait_names)


def get_amount_of_trait(el):
    amount = 0
    for amount_el in el.iter("TraitChangeAmount"):
        amount += int(amount_el.attrib["Value"]) * int(amount_el.attrib["Chance"])
    return amount / 100


def get_optional_singleton(parent, node):
    el = parent.findall(node)
    if len(el) > 1:
        raise RuntimeError(f"More than on <{node}> element")
    elif len(el) == 1:
        return el[0]
    return None


def handle_job(job):
    job_data = {"Name": job.attrib["ShortName"], "GainTraits": {}, "LoseTraits": {}, "Wages": "-",
                "PrimaryAction": "-", "SecondaryAction": "-", "BaseEnjoyment": "0",
                "Obedience": "0", "Fear": "0", "Dignity": "0"}
    perf_el = job.findall("Performance")
    if len(perf_el) > 1:
        raise RuntimeError("More than on <Performance> element")
    elif len(perf_el) == 1:
        perf_el = perf_el[0]
        for trait in perf_el.iter("Trait"):
            if trait.attrib["Name"] not in all_trait_names:
                raise RuntimeError(f"Invalid trait: '{trait.attrib['Name']}'")

    gains_el = get_optional_singleton(job, "Gains")
    if gains_el is not None:
        for trait in gains_el.iter("LoseTrait"):
            job_data["LoseTraits"][trait.attrib["Trait"]] = get_amount_of_trait(trait)
        for trait in gains_el.iter("GainTrait"):
            job_data["GainTraits"][trait.attrib["Trait"]] = get_amount_of_trait(trait)

    config_el = get_optional_singleton(job, "Config")
    if config_el is not None:
        job_data["Wages"] = config_el.attrib.get("BaseWages", "-")
        enjoyment_el = get_optional_singleton(config_el, "Enjoyment")
        if enjoyment_el is not None:
            job_data["PrimaryAction"] = enjoyment_el.attrib.get("PrimaryAction", "-")
            job_data["SecondaryAction"] = enjoyment_el.attrib.get("SecondaryAction", "-")
            job_data["BaseEnjoyment"] = enjoyment_el.attrib.get("BaseEnjoyment", "0")

        obedience_el = get_optional_singleton(config_el, "Obedience")
        if obedience_el is not None:
            job_data["Obedience"] = obedience_el.attrib.get("Difficulty", "10")
            job_data["Dignity"] = obedience_el.attrib.get("Dignity", "100")
            job_data["Fear"] = obedience_el.attrib.get("Fear", "0")

    return job_data


trait_info_changes = defaultdict(dict)


def format_trait_change(data):
    result = '<table class="inner">'
    result += "".join(f"<tr><td>{t}</td><td>{round(1000 / v)}</td></tr>" for t, v in data.items())
    result += "</table>"
    return result


out_file = """<html>
<head><style>
th {
    padding: 5px;
}
.outer td {
    padding: 5px;
}
.inner td {
    padding: 1px;
}
</style></head>
"""


def make_table_header(columns: list):
    result = "<tr>\n"
    for c in columns:
        result += f"  <th>{c}</th>\n"
    return result + "</tr>\n"


def make_table_row(contents: list):
    result = "<tr>\n"
    for c in contents:
        result += f"  <td>{c}</td>\n"
    return result + "</tr>\n"


out_file += '<table class="outer">' + make_table_header(["File", "Code", "Trait Gains", "Trait Losses", "Wages",
                                                         "Primary Act.", "Secondary Act.", "Enjoy", "Obd.",
                                                         "Dig.", "Fear"])
job_file_list = sorted((data_path / "Jobs").glob("*.xml"))
for job_file in job_file_list:
    doc = ET.fromstring(prepare_xml(job_file.read_text()))
    try:
        data = handle_job(doc)
    except Exception:
        print(f"Error in {job_file}")
        raise

    for t, v in data["GainTraits"].items():
        trait_info_changes[t][job_file.stem] = v
    for t, v in data["LoseTraits"].items():
        trait_info_changes[t][job_file.stem] = -v

    gains = format_trait_change(data["GainTraits"])
    losses = format_trait_change(data["LoseTraits"])
    line = make_table_row([job_file.name, data['Name'], gains, losses, data['Wages'],
                           data["PrimaryAction"], data["SecondaryAction"], data["BaseEnjoyment"],
                           data["Obedience"], data["Dignity"], data["Fear"]])
    out_file += "\n" + line
out_file += "</table></html>"

Path("Docs&Tools/jobs.html").write_text(out_file)


out_file = """<html>
<head><style>
th {
    padding: 5px;
}
.outer td {
    padding: 5px;
}
.inner td {
    padding: 1px;
}
</style></head>
"""

out_file += '<table class="outer"><tr><th>Trait</th><<th>Gain Jobs</th><th>Lose Jobs</th></tr>'
for key in sorted(trait_info_changes.keys()):
    gains = {k: v for k, v in trait_info_changes[key].items() if v > 0}
    losses = {k: -v for k, v in trait_info_changes[key].items() if v < 0}
    line = f"<tr><td>{key}</td><td>{format_trait_change(gains)}</td><td>{format_trait_change(losses)}</td></tr>"
    out_file += "\n" + line
out_file += "</table></html>"

Path("Docs&Tools/traits.html").write_text(out_file)
