# Visual Studio Code (VSCode)

[Visual Studio Code (VSCode)][VSCode] is a popular IDE with an integrated
debugger UI.

This module provides configuration files to integrate building, debugging,
uploading and code completion into the IDE:

- `.vscode/tasks.json`: If the `modm:build:scons` and `modm:build:make` module
  are included, this file wraps the building and uploading commands by profile.
  You can invoke them by Ctl+Shift+B (macOS: Cmd+Shift+B).
- `.vscode/c_cpp_properties.json`: Configures IntelliSense for better code
  completion and for using the correct system headers. You need the
  [C/C++ extension][] installed.
- `.vscode/launch.json`: Configures the [Cortex-Debug][] extension to start the
  debugger with a single click from the IDE.

Note that not all build system features are provided in the IDE, only the most
common. You can call all build system tools from the command line.
In addition, the `modm:build:cmake` module is supported natively by the
[CMake Tools][] extension and is therefore not wrapped.

We recommend adding this module from the command line so that you don't
accidentally overwrite your modified files later: `lbuild build -m ::vscode`.

Note that some configuration options may be specific to your environment (such
as the compiler path) and may not work in other environments.


[VSCode]: https://code.visualstudio.com/
[C/C++ extension]: https://github.com/Microsoft/vscode-cpptools
[CMake Tools]: https://github.com/microsoft/vscode-cmake-tools
[Cortex-Debug]: https://github.com/Marus/cortex-debug#readme
