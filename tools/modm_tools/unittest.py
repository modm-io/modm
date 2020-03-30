#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Unittest

This tools scans a directory for files ending in `_test.hpp`, extracts their
test cases and generates a source file containing the test runner.

```sh
python3 modm/modm_tools/unittest.py path/containing/tests \\
                                    path/to/generated_runner.cpp
```

Note that the files containing unittests must contain *one* class that inherits
from the `unittest::TestSuite` class, and test case names must begin with
`test`:

```cpp
class TestClass : public unittest::TestSuite
{
public:
    void testCase1();
}
```
"""

import re
from pathlib import Path
from jinja2 import Environment, StrictUndefined
if __name__ == "__main__":
    import sys, os
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import find_files

# -----------------------------------------------------------------------------
TEMPLATE_UNITTEST = """\
#include <unittest/controller.hpp>

%% for test in tests
#include "{{test.include}}"
%% endfor

namespace
{
%% for test in tests
FLASH_STORAGE_STRING({{test.instance}}Name) = "{{test.file}}";
%% endfor
}

int unittest::Controller::run(unittest::Reporter reporter)
{
    using namespace modm::accessor;
    instance().setReporter(reporter);

%% for test in tests
    instance().nextTestSuite(asFlash({{test.instance}}Name));
    {
        {{test.class}} {{test.instance}};
    %% for test_case in test.test_cases

        {{test.instance}}.setUp();
        {{test.instance}}.{{test_case}}();
        {{test.instance}}.tearDown();
    %% endfor
    }
%% endfor

    return instance().getReporter().printSummary();
}
"""

# -----------------------------------------------------------------------------
def extract_tests(headers):
    tests = []
    for header in headers:
        header = Path(header)
        content = header.read_text()

        name = re.findall(r"(?!class|struct)\s+([A-Z]\w+?)\s+:\s+"
                          r"public\s+unittest::TestSuite", content)
        if not name:
            raise ValueError("Test class not found in {}!".format(header))
        name = name[0]

        functions = re.findall(
            r"void\s+(test[A-Z]\w*)\s*\([\svoid]*\)\s*;", content)
        if not functions:
            print("No tests found in {}!".format(header))

        tests.append({
            "include": str(header.absolute()),
            "file": header.stem,
            "class": name,
            "instance": name[0].lower() + name[1:],
            "test_cases": functions,
        })

    return sorted(tests, key=lambda t: t["file"])


def render_runner(headers, destination=None):
    env = Environment(undefined=StrictUndefined, extensions=['jinja2.ext.do'])
    env.line_statement_prefix = '%%'
    env.line_comment_prefix = '%#'

    tests = extract_tests(headers)
    content = env.from_string(TEMPLATE_UNITTEST).render({"tests": tests})

    if destination is not None:
        Path(destination).write_text(content)
    return content


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Generate a Unittest runner')
    parser.add_argument(
            dest="path",
            help="The path to search for unittests in.")
    parser.add_argument(
            "-o", "--output",
            dest="destination",
            default="unittest_runner.cpp",
            help="Generated runner file.")

    args = parser.parse_args()
    headers = find_files.scan(args.path, ["_test"+h for h in find_files.HEADER])
    render_runner(headers, args.destination)
