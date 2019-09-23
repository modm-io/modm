# CompilationDB Generator

This module generates a Python script that generates a CompilationDB file, which
allows you to import your project into a code editor like CLion or Qt Creator
with working code completion and enhanced refactoring.

!!! note "CompilationDB is not a build system replacement"
	CompilationDB support is currently only implemented as an import mechanism
	in all IDEs, it cannot be used to compile *AND LINK* your application
	correctly. Use a proper build system for that purpose.

Since the CompilationDB requires absolute paths making it difficult to share
with other users, this module instead generates a script that you can call
whenever you need to generate a new `compile_commands.json` file.

The script takes are argument the application folder or files that it should
include. The search is recursive and looks only for source files while
ignoring any generated files:

```sh
# When calling this from where project.xml is located
# adding all sources contained in the current folder
python3 modm/tools/builder_compilation_db.py .
```

To generate the CompilationDB for debug mode, use the `--debug` option:

```sh
# You can also add individual files
python3 modm/tools/builder_compilation_db.py --debug main.cpp
```

## SCons integration

When including this module together with the `modm:build:scons` module, a new
command is added to SCons, which wraps the above command line invocation.

```sh
# This command safely wraps the generation script
scons compilation_db profile=release
scons compilation_db profile=debug
```
