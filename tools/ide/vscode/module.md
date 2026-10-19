# Visual Studio Code (VSCode)

[Visual Studio Code (VSCode)][VSCode] is a popular IDE with an integrated
debugger UI.

This module provides configuration files to integrate building, debugging,
uploading and code completion into the IDE:

- `.vscode/tasks.json`: If the `modm:build:scons` and `modm:build:make` module
  are included, this file wraps the building, uploading and cleaning commands
  by profile. The first build task is the default build task, so
  Ctl+Shift+B (macOS: Cmd+Shift+B) builds the release profile, and uploading
  builds the selected profile first. If both build systems are included, the
  tasks are also distinguished by their build system.
- `.vscode/c_cpp_properties.json`: Configures IntelliSense for better code
  completion and for using the correct system headers. You need the
  [C/C++ extension][] installed.
- `.vscode/launch.json`: Configures the [Cortex-Debug][] extension to start the
  debugger with a single click from the IDE. It builds the selected profile
  before flashing and passes the toolchain path, so that the debugger is found
  even when it is not on the `PATH` of VSCode.

All generated files are valid JSON, so that other editors and tools do not
report errors.

Note that not all build system features are provided in the IDE, only the most
common. You can call all build system tools from the command line.
In addition, the `modm:build:cmake` module is supported natively by the
[CMake Tools][] extension and is therefore not wrapped.

We recommend adding this module from the command line so that you don't
accidentally overwrite your modified files later: `lbuild build -m ::vscode`.

Note that some configuration options may be specific to your environment (such
as the compiler toolchain path) and may not work in other environments.


[VSCode]: https://code.visualstudio.com/
[C/C++ extension]: https://github.com/Microsoft/vscode-cpptools
[CMake Tools]: https://github.com/microsoft/vscode-cmake-tools
[Cortex-Debug]: https://github.com/Marus/cortex-debug#readme
