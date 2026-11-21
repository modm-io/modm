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
python3 -m modm_tools.unit_test path/containing/tests path/to/generated_runner.cpp
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
from jinja2 import Environment

from . import find_files


# -----------------------------------------------------------------------------
TEMPLATE_UNITTEST = r"""\
#include <unittest/reporter.hpp>

{% for test in tests %}
#include "{{test.include}}"
{% endfor %}

namespace
{
{% for test in tests -%}
FLASH_STORAGE_STRING({{test.instance}}Name) = "{{test.file[:-5]}}";
{%- if functions %}
{% for test_case in test.test_cases -%}
FLASH_STORAGE_STRING({{test.instance}}_{{test_case}}Name) = "{{test_case[4:]}}";
{% endfor -%}
{% endif -%}
{% endfor %}
// Not inlined, so that LTO cannot merge the stack frames of all test suites
{% for test in tests -%}
modm_noinline void run_{{test.instance}}()
{
    using namespace modm::accessor;

    unittest::reporter.nextTestSuite(asFlash({{test.instance}}Name));
    {{test.class}} {{test.instance}};
    {%- for test_case in test.test_cases %}

    {% if functions -%}
    unittest::reporter.nextTestFunction(asFlash({{test.instance}}_{{test_case}}Name));
    {% endif -%}
    {{test.instance}}.setUp();
    {{test.instance}}.{{test_case}}();
    {{test.instance}}.tearDown();
    {%- endfor %}
}
{% endfor -%}
}

int run_modm_unit_test()
{
{% for test in tests -%}
    run_{{test.instance}}();
{% endfor %}
    return unittest::reporter.printSummary();
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
            r"void\s+(test[_a-zA-Z]\w*)\s*\([\svoid]*\)\s*;", content)
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


def render_runner(headers, destination=None, functions=False):
    tests = extract_tests(headers)
    content = Environment().from_string(TEMPLATE_UNITTEST)
    content = content.render({"tests": tests, "functions": functions})

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
    parser.add_argument(
            "-fn", "--with-function-names",
            dest="functions",
            default=False,
            action="store_true",
            help="Generate with test function name.")

    args = parser.parse_args()
    headers = find_files.scan(args.path, ["_test"+h for h in find_files.HEADER])
    render_runner(headers, args.destination)
