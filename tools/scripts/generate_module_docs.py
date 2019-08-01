#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os, sys, re, shutil
import graphviz as gv

from pathlib import Path
from jinja2 import Environment
from collections import defaultdict

def repopath(path):
    return (Path(os.path.abspath(__file__)).parents[2] / path)
def relpath(path):
    return os.path.relpath(path, str(repopath(".")))

sys.path.append(str(repopath("ext/modm-devices/tools/generator")))
sys.path.append(str(repopath("ext/modm-devices/tools/device")))
from dfg.device_tree import DeviceTree
from modm_devices.device_identifier import *
sys.path.append(str(repopath("../lbuild")))
import lbuild


def get_modules(builder, limit=None):
    builder._load_repositories(repopath("repo.lb"))
    option = builder.parser.find_option(":target")

    targets = list(set(option.values) - set(d for d in repopath("test/all/ignored.txt").read_text().strip().splitlines() if "#" not in d))
    if limit is not None:
        targets = targets[:limit]
    targets = sorted(targets)

    # Prime the repositories and get all module files
    mfiles = []
    option.value = targets[0]
    for repo in builder.parser._findall(builder.parser.Type.REPOSITORY):
        repo.prepare()
        mfiles.extend([(repo, file) for file in repo._module_files])

    num_modules = []
    num_options = []

    print("Querying for {} targets...".format(len(targets)))
    mtargets = []
    for target in targets:
        option.value = target
        target = option.value._identifier

        imodules = [m for (repo, mfile) in mfiles for m in lbuild.module.load_module_from_file(repo, mfile)]
        imodules = [m for m in imodules if m.available]
        imodules = sorted(imodules, key=lambda m: m.fullname.count(":"))

        num_modules.append(len(imodules))
        print("{}: {}".format(target, len(imodules)))

        modules = {"modm": DeviceTree("modm", target)}
        modules["modm"].addSortKey(lambda mo: (mo.name, mo["filename"]))
        for init in imodules:
            if init.name.isdigit(): continue;
            m = modules[init.parent].addChild(init.fullname.split(":")[-1])
            modules[init.fullname] = m
            m.addSortKey(lambda mo: (mo.name, m["filename"], mo.get("value", ""), mo.get("option", "")))
            m.setAttribute("type", "Module")
            m.setAttribute("filename", relpath(init.filename))
            m.setAttribute("_description", init.description)

            for d in init._dependencies:
                dep = m.addChild("dependency")
                dep.setValue(("modm" + d).replace("modmmodm", "modm"))

            num_options.append(len(init._options))
            for o in init._options:
                op = m.addChild(o.fullname.split(":")[-1])
                op.addSortKey(lambda oo: (oo.name, oo["type"], oo.get("value", "")))
                op.setAttribute("type", type(o).__name__)
                op.setAttribute("_description", o._description)
                opval = op.addChild("default")
                opval.setValue(o.format_value())
                opvals = op.addChild("inputs")
                opvals.setValue(o.format_values())

                if o._dependency_handler:
                    for valin in o.values:
                        valout = o._dependency_handler(o._in(valin))
                        if valout:
                            opdep = op.addChild("dependency")
                            opdep.setValue("{} -> {}".format(valin, ("modm" + valout).replace("modmmodm", "modm")))

            for c in init._collectors:
                cp = m.addChild(c.fullname.split(":")[-1])
                cp.addSortKey(lambda cc: (cc.name))
                cp.setAttribute("type", type(c).__name__)
                cp.setAttribute("_description", c._description)
                opvals = cp.addChild("inputs")
                opvals.setValue(c.format_values())
            for q in init._queries:
                qp = m.addChild(q.fullname.split(":")[-1])
                qp.addSortKey(lambda qq: (qq.name))
                qp.setAttribute("type", type(c).__name__)
                qp.setAttribute("_description", q._description)


        mtargets.append(modules["modm"])

    print("Merging module tree...")
    mfinal = mtargets[0]
    for mtarg in mtargets[1:]:
        mfinal.merge(mtarg)
    print("Sorting module tree...")
    mfinal._sortTree()

    return mfinal, (num_modules, num_options)


def split_description(descr):
    if descr is None: return "", "";
    lines = descr.strip(" \n").splitlines() + [""]
    return lines[0].replace("#", "").strip(), "\n".join(lines[1:])

def extract(text, key):
    return re.search(r"# {0}\n(.*?)\n# /{0}".format(key), text, flags=re.DOTALL | re.MULTILINE).group(1)

def replace(text, key, content):
    return re.sub(r"# {0}.*?# /{0}".format(key), "# {0}\n{1}\n# /{0}".format(key, content), text, flags=re.DOTALL | re.MULTILINE)

def url_name(name):
    for c in ":., ({": name = name.replace(c, "-");
    for c in "})":     name = name.replace(c, "");
    return name

def ref_name(name):
    return url_name(name).replace("-", "_")

def node_name(name):
    return name.replace(":", ":\n")

def get_platform_families(mids):
    dids = defaultdict(set)
    for did in mids:
        dids[did.platform].add(did.family)
    return dids

all_dids = None
all_targets = None
def target_diff(targets, compare=None):
    global all_dids, all_targets
    if all_dids is None:
        all_dids = get_platform_families(all_targets)
    if compare is None:
        compare = all_targets

    if compare == targets:
        return ""

    if len(targets) == 1:
        return list(targets.ids)[0].string

    output = []
    for p, fs in get_platform_families(targets).items():
        if fs != all_dids[p]:
            if len(fs) == 1:
                p += list(fs)[0]
            else:
                p += "{" + ",".join(sorted(fs)) + "}"
        output.append(p)
    return ", ".join(sorted(set(output)))


def render_dependency_graphs(node):
    graph = gv.Digraph(name=node["name"],
                       format="svg",
                       graph_attr={"rankdir": "BT"},
                       node_attr={"style": "filled,solid", "shape": "box"})
    graph.node(ref_name(node["name"]), node_name(node["name"]), style="filled,bold")

    for mod in sorted(node["dependencies"].keys()):
        graph.node(ref_name(mod), node_name(mod),
                   style="filled" if node["dependencies"][mod] else "filled,dashed")
        graph.edge(ref_name(node["name"]), ref_name(mod),
                   style="solid" if node["dependencies"][mod] else "dashed")

    return graph.pipe().decode("utf-8")

def format_module(modules, node):
    fullname = node.name
    nname = node.parent
    while(nname):
        fullname = "{}:{}".format(nname.name, fullname)
        nname = nname.parent

    # ident = node.ids.string if (node.parent and node.parent.ids != node.ids) else ""

    title, descr = split_description(node["_description"])
    mprops = {
        "name": fullname,
        "title": title,
        "description": descr,
        "targets": target_diff(node.ids, compare=node.parent.ids),
        "is_limited": node.ids != all_targets,
        "url": url_name(fullname),
        "dependencies": {},
        "options": [], "collectors": [], "queries": []
    }

    for deps in (c for c in node.children if c.name == "dependency"):
        mprops["dependencies"][deps["value"]] = deps.ids == node.ids

    for option in (c for c in node.children if "Option" in c.get("type", "")):
        op = {"name": option.name,
              "description": option["_description"].strip().strip("#").strip(),
              "targets": target_diff(option.ids, compare=node.ids),
              "default": {c["value"]: target_diff(c.ids, compare=option.ids) for c in option.children if c.name == "default"},
              "inputs": {c["value"]: target_diff(c.ids, compare=option.ids) for c in option.children if c.name == "inputs"},
              "dependencies": {c["value"]: target_diff(c.ids, compare=option.ids) for c in option.children if c.name == "dependency"},
        }
        # print(fullname, op["dependencies"])
        for name, targets in op["dependencies"].items():
            name = name.split("-> ")[1]
            mprops["dependencies"][name] = mprops["dependencies"].get(name, False)
        mprops["options"].append(op)

    for collector in (c for c in node.children if "Collector" in c.get("type", "")):
        op = {"name": collector.name,
              "description": collector["_description"].strip().strip("#").strip(),
              "targets": target_diff(collector.ids, compare=node.ids),
              "inputs": {c["value"]: target_diff(c.ids, compare=collector.ids) for c in collector.children if c.name == "inputs"},
        }
        mprops["collectors"].append(op)

    for query in (c for c in node.children if "Query" in c.get("type", "")):
        op = {"name": query.name,
              "description": query["_description"].strip().strip("#").strip(),
              "targets": target_diff(query.ids, compare=node.ids),
        }
        mprops["queries"].append(op)

    mprops["graph"] = render_dependency_graphs(mprops)
    modules[fullname].append(mprops)

    for child in [c for c in node.children if "filename" in c]:
        format_module(modules, child)



if __name__ == "__main__":
    # lbuild.logger.configure_logger(2)
    lbuild.format.PLAIN = True
    builder = lbuild.api.Builder()

    module_tree, mlens = get_modules(builder)
    all_targets = module_tree.ids

    print(f"Total: {sum(mlens[0])} modules and {sum(mlens[1])} options generated.\n"
          f"Between {min(mlens[0])} and {max(mlens[0])} modules per target.\n"
          f"Up to {max(mlens[1])} options per module.")

    modules = defaultdict(list)
    for c in module_tree.children:
        format_module(modules, c)

    env = Environment()
    env.line_statement_prefix = '%%'
    module_path = repopath("docs/src/reference/module/")
    shutil.rmtree(module_path, ignore_errors=True)
    module_path.mkdir()

    modtable = []
    for name, tmodules in modules.items():
        for m in tmodules:
            nname = name
            url = url_name(nname)
            if len(tmodules) > 1:
                nname += " ({})".format(m["targets"]);
                url += url_name("-" + m["targets"])
            rendered = env.from_string(repopath("docs/module.md.in").read_text()).render({"module": m})
            (module_path / "{}.md".format(url)).write_text(rendered)
            modtable.append("      - \"{}\": \"reference/module/{}.md\"".format(nname.replace("modm:", ":"), url))

    config_path = Path(repopath("docs/mkdocs.yml"))
    modtable = "\n".join(sorted(modtable))
    config = config_path.read_text()
    if extract(config, "moduletable") != modtable:
        config = replace(config, "moduletable", modtable)
        config_path.write_text(config)
