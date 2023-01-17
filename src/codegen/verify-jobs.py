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


def handle_job(job):
    job_data = {"Name": job.attrib["ShortName"], "GainTraits": {}, "LoseTraits": {}}
    perf_el = job.findall("Performance")
    if len(perf_el) > 1:
        raise RuntimeError("More than on <Performance> element")
    elif len(perf_el) == 1:
        perf_el = perf_el[0]
        for trait in perf_el.iter("Trait"):
            if trait.attrib["Name"] not in all_trait_names:
                raise RuntimeError(f"Invalid trait: '{trait.attrib['Name']}'")

    gains_el = job.findall("Gains")
    if len(gains_el) > 1:
        raise RuntimeError("More than on <Gains> element")
    elif len(gains_el) == 1:
        gains_el = gains_el[0]
        for trait in gains_el.iter("LoseTrait"):
            job_data["LoseTraits"][trait.attrib["Trait"]] = get_amount_of_trait(trait)
        for trait in gains_el.iter("GainTrait"):
            job_data["GainTraits"][trait.attrib["Trait"]] = get_amount_of_trait(trait)

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

out_file += '<table class="outer"><tr><th>File</th><th>Code</th><th>Trait Gains</th><th>Trait Losses</th></tr>'
job_file_list = sorted((data_path / "Jobs").glob("*.xml"))
for job_file in job_file_list:
    doc = ET.fromstring(prepare_xml(job_file.read_text()))
    data = handle_job(doc)

    for t, v in data["GainTraits"].items():
        trait_info_changes[t][job_file.stem] = v
    for t, v in data["LoseTraits"].items():
        trait_info_changes[t][job_file.stem] = -v

    gains = format_trait_change(data["GainTraits"])
    losses = format_trait_change(data["LoseTraits"])
    line = f"<tr><td>{job_file.name}</td><td>{data['Name']}</td><td>{gains}</td><td>{losses}</td></tr>"
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
