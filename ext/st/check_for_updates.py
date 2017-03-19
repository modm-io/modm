# Script is tested on OS X 10.12
# YOUR MILEAGE MAY VARY

from urllib.request import urlopen
import re

cube_urls = {
    "f0": "http://www.st.com/en/embedded-software/stm32cubef0.html",
    "f1": "http://www.st.com/en/embedded-software/stm32cubef1.html",
    "f2": "http://www.st.com/en/embedded-software/stm32cubef2.html",
    "f3": "http://www.st.com/en/embedded-software/stm32cubef3.html",
    "f4": "http://www.st.com/en/embedded-software/stm32cubef4.html",
    "f7": "http://www.st.com/en/embedded-software/stm32cubef7.html",
    "l0": "http://www.st.com/en/embedded-software/stm32cubel0.html",
    "l1": "http://www.st.com/en/embedded-software/stm32cubel1.html",
    "l4": "http://www.st.com/en/embedded-software/stm32cubel4.html",
}
cube_current_versions = {}
cube_new_versions = {}
cube_dl_urls = {}

# parse the versions directly from the README
with open("README.md", "r") as readme:
    content = readme.read()
    for family in cube_urls:
        regex = "\(Cube{} v(?P<version>[0-9]+\.[0-9]+\.[0-9]+)\)".format(family.upper())
        match = re.search(regex, content)
        if match:
            cube_current_versions[family] = match.group('version')
        else:
            print("No version match for", family)
            exit(1)

# parse the versions and download links from ST's website
for family, url in cube_urls.items():
    response = urlopen(url)
    html = response.read().decode('utf-8')

    dlmatch = re.search("data-download-path=\"(?P<dlurl>/content/ccc/resource/.*?\.zip)\"", html)
    if dlmatch:
        cube_dl_urls[family] = "http://www.st.com" + dlmatch.group('dlurl')
    else:
        print("No download link for", family)
        exit(1)

    vmatch = re.search("    (?P<version>[0-9]+\.[0-9]+\.[0-9]+)", html)
    if dlmatch:
        cube_new_versions[family] = vmatch.group('version')
    else:
        print("No version match in html for", family)
        exit(1)

# compare all versions and print a status page
update_required = False
for family in sorted(cube_urls):
    current, new = cube_current_versions[family], cube_new_versions[family]
    print("{}: v{} -> v{}\t{}".format(family, current, new, "update!" if current != new else "ok"))
    if current != new:
        update_required = True

# if an update is required, fail this "test"
if update_required:
    exit(1)
