#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2017, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import subprocess, locale
from collections import defaultdict
import argparse

author_handles = {
    "Andre Gilerson": "AndreGilerson",
    "Antal Szabó": "Sh4rK",
    "Arjun Sarin": None,
    "Carl Treudler": "cajt",
    "Christian Menard": "chrism333",
    "Christoph Rüdi": None,
    "Christopher Durand": "chris-durand",
    "Daniel Krebs": "daniel-k",
    "David Hebbeker": "dhebbeker",
    "Fabian Greif": "dergraaf",
    "Georgi Grinshpun": "georgi-g",
    "Hans Schily": "RzwoDzwo",
    "Julia Gutheil": None,
    "Kevin Läufer": "ekiwi",
    "Marten Junga": "Maju-Ketchup",
    "Martin Esser": "Scabber",
    "Martin Rosekeit": "thundernail",
    "Michael Thies": "mhthies",
    "Nick Sarten": "genbattle",
    "Niclas Rohrer": None,
    "Niklas Hauser": "salkinium",
    "Raphael Lehmann": "rleh",
    "Sascha Schade": "strongly-typed",
    "Tarik TIRE": "7Kronos",
    "Thorsten Lajewski": "TheTh0r",
    "Tomasz Chyrowicz": "tomchy",
    "Álan Crístoffer": "acristoffers",
}

def get_author_log(since = None, until = None, handles = True, count = False):
    sl_command = "git shortlog -sn"
    if since is not None:
        sl_command += " --since=\"{}\"".format(since)
    if until is not None:
        sl_command += " --until=\"{}\"".format(until)
    # get the shortlog summary
    output = subprocess.Popen(sl_command, shell=True, stdout=subprocess.PIPE)\
            .stdout.read().decode(locale.getpreferredencoding())
    # parse the shortlog
    shortlog = defaultdict(int)
    for line in output.splitlines():
        commits, author = line.split("\t")
        shortlog[author] += int(commits)

    # convert to list of tuples for sorting
    commit_tuples = [(c, a) for a, c in shortlog.items()]
    if count:
        # sort by number of commits, then alphabetically by author
        commit_tuples.sort(key=lambda a: (-a[0], a[1]))
    else:
        # sort by name
        commit_tuples.sort(key=lambda a: a[1])

    output = []
    for (commits, author) in commit_tuples:
        out = author
        if handles and author in author_handles and author_handles[author] is not None:
            out += u" (@{})".format(author_handles[author])
        if count:
            out = u"{:4}  {}".format(commits, out)
        output.append(out)
    return output


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Author statistics of modm.")
    parser.add_argument("--handles", dest="with_handles", action="store_true",
                        help="adds the GitHub handle to the author if known")
    parser.add_argument("--count", dest="with_count", action="store_true",
                        help="adds and sorts authors by commit count")
    parser.add_argument("--shoutout", dest="with_shoutout", action="store_true",
                        help="annotates first time contributers")
    parser.add_argument("--since", dest="since",
                        help="evaluates the git history from this date until present")

    args = parser.parse_args()
    since_date = args.since if args.since else None
    log_authors = get_author_log(since=since_date, handles=args.with_handles, count=args.with_count)

    new_authors = []
    if args.with_shoutout and since_date:
        previous_authors = get_author_log(until=since_date, handles=False, count=False)
        new_authors = get_author_log(since=since_date, handles=False, count=False)
        new_authors = [a for a in new_authors if a not in previous_authors]

    authors = []
    for author in log_authors:
        if any(a in author for a in new_authors):
            author += u" 🎉🎊"
        authors.append(author)
    print("\n".join(authors))
