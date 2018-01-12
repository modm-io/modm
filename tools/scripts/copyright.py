#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2017, Niklas Hauser
# Copyright (c) 2017, Fabian Greif
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from collections import defaultdict
import subprocess
import sys
import re
import os
import argparse

comment_styles = {
    "*":    ("/*\n * ", " * ", "\n */"),
    "#":    ("# ", "# ", ""),
    "%#":   ("%# ", "%# ", ""),
    "//":   ("// ", "// ", ""),
    "<!--": ("<!--\n", "", "\n-->"),
    "..":   (".. ", "   ", ""),

    ".c": "*",
    ".h": ".c",
    ".cpp": ".c",
    ".hpp": ".c",
    ".c.in": ".c",
    ".h.in": ".c",
    ".cpp.in": ".c",
    ".hpp.in": ".c",

    ".py": "#",
    ".cfg": ".py",
    ".lb": ".py",
    "SConstruct": ".py",
    "SConscript": ".py",
    "SConstruct.in": ".py",
    "SConscript.in": ".py",
    "Makefile": ".py",

    ".html": "<!--",
    ".xml": ".html",
    ".svg": ".html",
    ".md": ".html",

    ".rst": "..",
}

def resolve_comment_style(style_or_filename):
    style = None
    if style_or_filename in comment_styles:
        style = style_or_filename
    else:
        for cstyle in comment_styles:
            if style_or_filename.endswith(cstyle):
                style = cstyle
                break
    if style is not None:
        # resolve until done
        while 1:
            style = comment_styles[style]
            if isinstance(style, tuple): return style;
            if style not in comment_styles: break;
    print("Error: Unknown style or filename '{}'!".format(style_or_filename))
    exit(1)
    return None

ignored_endings = [
    ".pyc",
    ".jpg",
    ".png",
    ".gnuplot",
    ".gaphor",
    ".mm",
    ".graphml",
    ".pic",
    ".xcf",
    ".css",
    ".dox",
    ".pdf",
    ".svg",
    ".pbm",
    ".font",
    ".xml",

    "README",
    "TODO",
]
ignored_beginnings = [
    ".",
    "AUTHORS",
    "LICENSE",
    "doc/examples/SConstruct.template",
    "tools/copyright.py",
    "doc/doxyfile",
    "build/",
    "templates",
    "ext",
    "tools/openocd/xpcc",
    "doc/doxygen",
    "tools/syntax-highlighter",
    "tools/font_creator",
    "doc/api",
]

# We only care about MPL-2.0 at the moment
license_texts = {
    "MPL-2.0": """\
This file is part of the modm project.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.\
""",
}
copyright_format = "Copyright (c) {years}, {author}"

# some commits contaminate the history by doing large scale file moves or changes
ignored_shas = {
    # "2814009": ["Fabian Greif"], # Merge changes from xpcc 'develop' into modm
    "b791589": [], # Move all new files since to modm paths.
    "8e4a47f": [], # Add 'ext/ros_lib/'
    "e1f056a": [], # [ext/st] Update STM32 CMSIS headers.
    "f8e478b": [], # Update copyright headers.
    "026b772": [], # Merge commit 'xxx' as 'ext/st/stm32'
    "91803e5": [], # Move all new files since to modm paths.
    "ab05ea7": [], # Rename platform module files to 'module.lb'.
    "5007bee": [], # Move 'board' and 'platform' to the top level.
    "7c3cf3e": [], # Update the top level directory name to 'modm'.
    "28db910": [], # Move test files out of the source tree.
    "6fd26d8": [], # Change namespace from 'xpcc' to 'modm'.
    "d774c57": [], # Avoid usage of double underscore.
    "f412a4e": [], # Move overview files into their corresponding module.
    "e9922ec": [], # Update include path to 'modm'
    "b0f1089": [], # Remove trailing whitespace.
    "11b01ca": ["Fabian Greif"], # Merge changes from xpcc 'develop' into modm
    "9df6acf": [], # Access target properties as a dictionary.
    "c21da1a": [], # Update license and copyright information.
    "7aeb51e": [], # Convert files to UTF-8.
    # "": [], #
    # "": [], #
}

included_merges = {
    "7d4731c": ["Niklas Hauser"],
}

def get_copyright_comment_style(content):
    cmatch = re.search("(?P<style>\.\.|<!--|\*|//|\%#|#)[ \n\r]*?Copyright[^{}]*?\n", content, re.DOTALL)
    if not cmatch and "Copyright" in content: return "Unknown";
    return cmatch.group("style") if cmatch else None

def get_all_files_recursive(root_dir, sha_since):
    output = subprocess.Popen("git ls-files .", shell=True, stdout=subprocess.PIPE).stdout.read().decode("utf-8")
    all_files = []
    for filename in output.splitlines():
        if (any(filename.endswith(pat) for pat in ignored_endings) or
            any(filename.startswith(pat)for pat in ignored_beginnings) or
            "/." in filename):
            continue
        all_files.append(filename)
    all_modified_files = []
    output = subprocess.Popen("git diff --name-only HEAD {}".format(sha_since), shell=True, stdout=subprocess.PIPE).stdout.read().decode("utf-8")
    for filename in output.splitlines():
        if (any(filename.endswith(pat) for pat in ignored_endings) or
            any(filename.startswith(pat)for pat in ignored_beginnings) or
            "/." in filename):
            continue
        all_modified_files.append(filename)
    all_modified_files = [f for f in all_modified_files if f in all_files]
    return all_modified_files

def get_all_copyright_styles(root_dir, sha_since):
    styles = defaultdict(list)
    for filename in get_all_files_recursive(root_dir, sha_since):
        with open(filename, "r", errors="replace") as filec:
            content = filec.read()
            styles[get_copyright_comment_style(content)].append(filename)
    return styles

def find_copyright_header(filename):
    header, license, style = None, None, None
    start, end = -1, -1
    with open(filename, "r", errors="replace") as filec:
        content = filec.read()
        # get comment style first
        style = get_copyright_comment_style(content)
        if style in [None, "Unknown"]: return license, header, start, end, style;
        # build regex match pattern
        header_pattern = {
            "*": ("(?P<header>/\*.*?", ".*?\*/)"),
            "#": ("(\-\*\-.*?)?(?P<header># ", ")\r?\n"),
            "%#": ("(?P<header>\%# ", ")\r?\n"),
            "//": ("(?P<header>// ?", ")\r?\n"),
            "<!--": ("(?P<header>\<\!\-\-.*?", ".*?\-\-\>)\r?\n"),
            "..": ("(?P<header>\.\..*?", ")\r?\n")
        }
        # Search for the license header
        regex = "{}Copyright.*?(DAMAGE\.|code\.|MPL/2\.0/\.|licenses/\>\.|02111-1307  USA){}"
        regex = regex.format(header_pattern[style][0], header_pattern[style][1])
        hmatch = re.search(regex, content, re.DOTALL)
        license = "Unknown"
        if not hmatch: return license, header, start, end, style;
        header = hmatch.group("header")
        # classify licenses
        if "3-clause BSD" in header: license = "BSD-3-Clause";
        elif "DAMAGE" in header: license = "BSD-3-Clause";
        elif "MPL" in header: license = "MPL-2.0";
        elif "GPL version 3" in header: license = "GPL-3.0";
        elif "License version 2.1" in header: license = "LGPL-2.1";
        # find license line span
        lmatch = re.search("(?P<prelude>.*?)" + re.escape(header), content, re.DOTALL)
        if not lmatch: return license, header, start, end, style
        start = lmatch.group("prelude").count("\n")
        end = start + header.count("\n")
        # done
        return license, header, start, end, style

def parse_copyright_authors(header):
    authors = {}
    if header is None: return authors;
    for line in header.splitlines():
        match = re.search("Copyright.*?(?P<years>([0-9]{4}[\-,]? ?)+) (?![0-9])(?P<author>.*)", line)
        if not match: continue;
        years = []
        for year in [y for y in match.group("years").split(",") if y != ""]:
            if "-" in year:
                year = [int(y) for y in year.split("-")]
                years.extend([y for y in range(year[0], year[1] + 1)])
            else:
                years.append(int(year))

        authors[match.group("author")] = sorted(years)
    return authors

def get_commits_for_file(filename, m=0.5):
    command = "git log --follow --format=\"%h %aN %p %aI %s\" -M{}% -- {}".format(int(m * 100), filename)
    output = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).stdout.read().decode("utf-8").splitlines()
    command = "git log -m --follow --format=\"%h %aN %p %aI %s\" -M{}% -- {}".format(int(m * 100), filename)
    output += subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).stdout.read().decode("utf-8").splitlines()

    commits = defaultdict(tuple)
    for line in output:
        match = re.search("(?P<sha>[a-f0-9]{9}) (?P<author>.*?) (?P<parents>([a-f0-9]{9} ?)*?) (?P<year>[0-9]{4})-[0-9]{2}", line)
        sha, author, parents, year = match.group("sha")[:7], match.group("author"), match.group("parents").split(" "), match.group("year")
        commits[sha] = (author, year, [p[:7] for p in parents])

    return commits

def get_git_history_authors(filename, m=0.5):
    authors = defaultdict(list)
    commits = get_commits_for_file(filename, m)
    print(filename)

    for sha, (author, year, parents) in commits.items():
        if (sha in ignored_shas and
            (len(ignored_shas[sha]) == 0 or author in ignored_shas[sha])): continue;
        if (len(parents) >= 2) and not (sha in included_merges and
            (len(included_merges[sha]) == 0 or author in included_merges[sha])):
                continue;
        authors[author].append(int(year))
    # return sorted and unique years
    return {a:sorted(list(set(y))) for a, y in authors.items()}

def compactify_years(years):
    # if len(years) == 1: return map(str, years);
    streaks = []
    streak = []
    for year in years:
        if len(streak) == 0 or year == (streak[-1] + 1):
            streak.append(year)
        else:
            streaks.append(streak)
            streak = [year]
    streaks.append(streak)
    streaks = [(s[0], s[-1]) for s in streaks]
    years = [("{}-{}".format(s[0], s[1]) if s[0] != s[1] else str(s[0])) for s in streaks]
    return years

def format_copyright_header(authors, style):
    lines = []
    # sort names by start of copyright date, then alphabetically
    for author in sorted(authors.keys(), key=lambda a: (min(authors[a]), max(authors[a]), a)):
        years = compactify_years(authors[author])
        string = copyright_format.format(years=", ".join(years), author=author)
        lines.append(string)
    # add a newline after copyright
    lines.append("")
    # add license
    lines.extend(license_texts["MPL-2.0"].splitlines())
    # wrap in style
    slines = []
    for i, line in enumerate(lines):
        if i == 0:
            slines.append(style[0] + line);
        elif i == (len(lines) - 1):
            slines.append(style[1] + line + style[2])
        else:
            slines.append(style[1] + line)
    # strip all trailing whitespace
    lines = [l.rstrip() for l in slines]
    # return the text
    return "\n".join(lines)

def print_copyright_header(filename):
    license, header, start, end, _ = find_copyright_header(filename)
    lines = ""
    if start > 0 and end > 0: lines = " @ {}:{}".format(start, end);
    string = "{} -> {}{}\n{}\n".format(filename, license, lines, header)
    print(string)

def print_copyright_git_authors(filename):
    authors = get_git_history_authors(filename)
    nauth = "{} author{}".format(len(authors), "s" if len(authors) > 1 else "") if len(authors) else "None"
    authors = "{}\n".format(authors) if len(authors) else ""
    string = "{} -> {}\n{}".format(filename, nauth, authors)
    print(string)

def print_copyright_authors(filename):
    license, header, _, _, style = find_copyright_header(filename)
    hauthors = parse_copyright_authors(header)
    gauthors = get_git_history_authors(filename)

    def authN(authors):
        return "{} author{}".format(len(authors), "s" if len(authors) > 1 else "") if len(authors) else "None"
    def authF(authors):
        names = sorted(authors.keys())
        return " ".join(["{}:{}".format(n, ",".join(map(str, authors[n]))) for n in names])

    hauth = "({})\n".format(authF(hauthors)) if len(hauthors) else ""
    string = "{} -> {} ({})\n{}\n{}".format(filename, authN(gauthors), authN(hauthors), authF(gauthors), hauth)
    nheader = format_copyright_header(gauthors, resolve_comment_style(filename if style in [None, "Unknown"] else style))
    if header and len(hauthors): string += header + "\n";
    string += nheader + "\n"
    # print(string)
    return license, header, nheader

def print_all_files_of_style(style, sha_since):
    styles = get_all_copyright_styles(".", sha_since)
    for filename in styles[style]:
        print(filename)

def print_all_copyright_headers(sha_since):
    for filename in get_all_files_recursive(".", sha_since):
        print_copyright_header(filename)

def print_all_copyright_authors(sha_since):
    for filename in get_all_files_recursive(".", sha_since):
        print_copyright_authors(filename)

def replace_all_copyright_authors(sha_since):
    for filename in get_all_files_recursive(".", sha_since):
        # print(filename)
        license, header, nheader = print_copyright_authors(filename)
        if license not in ["BSD-3-Clause", "MPL-2.0", None]:
            continue

        with open(filename, "r+", errors="replace") as filec:
            content = filec.read()
            if header is None:
                content = nheader + "\n\n" + content
            else:
                content = content.replace(header, nheader)
            filec.seek(0)
            filec.write(content)
            filec.truncate()


if __name__ == "__main__":

    # print(print_copyright_authors("src/modm/communication/ros/ros_xpcc_hardware.hpp"))
    # exit(1)

    parser = argparse.ArgumentParser(description="Copyright Update Script")

    parser.add_argument("sha", type=str,
                        help="SHA of the first commit to consider")
    parser.add_argument("-a", "--authors",
                        dest="authors", action="store_true", default=False,
                        help="Update authors")
    parser.add_argument("-r", "--replace",
                        dest="replace", action="store_true", default=False,
                        help="Replace header")

    args = parser.parse_args()

    if args.authors:
        if args.replace:
            replace_all_copyright_authors(args.sha)
        else:
            print_all_copyright_authors(args.sha)
    else:
        print_all_copyright_headers(args.sha)
    # print_all_files_of_style(None, sys.argv[1])