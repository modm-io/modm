# Build Systems

For modm, build systems are not special and are treated as just another module
that *generates* the build system configuration as part of your custom library.

The required data for this is a list of generated files provided by lbuild itself,
and lots of metadata like header include paths, compile flags, preprocessor definitions,
libraries to be linked, that is collected from each module.

This information is then assembled by the [`modm:build` module](../module/modm-build)
which generates generic support files and data for providing compilation,
uploading and debugging functionality.

This alone is not enough to compile your project though, so you have to select
one or more `modm:build:*` submodules, that translates this data for the
specific build system.

!!! warning "Build systems do not interoperate"
	While you can generate and use multiple build systems at the same time, they
	do not have the same feature set and do not interoperate. So you cannot
	compile your program with one build system and expect to upload the
	firmware to your target with another.

!!! info "Add your build system"
	Most of the difficult work is done by the `modm:build` module, so adding
	another build system generator is a manageable feat. Feel free to open up
	an PR that adds support for your build system and we'll give you lots of
	tips during the review.

!!! tip "Enable parallel builds"
    Remember to add the `-j8` flag to your `scons/make build` call to enable
    parallel builds, which are significantly faster than sequential builds. To
    make this a permanent setting you can add `export SCONSFLAGS="-j8"` or
    `export MAKEFLAGS="-j8"` to your `.bashrc`!


### SCons

The `modm:build:scons` build system generator is our preferred and recommended
one and features many very useful modm- and embedded-specific tools.
However, it is more optimized for terminal use and not for integration into IDEs.

[See the `modm:build:scons` documentation](../module/modm-build-scons).


### Make

The `modm:build:make` generator creates a stand-alone, but feature-complete,
Makefile build system. The embedded-specific tooling is provided by wrapping
separate Python3 scripts in `.PHONY` targets. It is intended as a simple,
hackable build system without standardized IDE integration.

[See the `modm:build:make` documentation](../module/modm-build-make).


### CMake

The `modm:build:cmake` module generates a CMake build script, which you can
import into a lot of IDEs and compile it from there.

[See the `modm:build:cmake` documentation](../module/modm-build-cmake).


## Customization

All build system modules have many options, which you can set in your `project.xml`
configuration file. However, there are also lbuild collectors, which are the
mechanism used by the library modules themselves to communicate the required
metadata to the build modules.

You can specify additional collector values in your `project.xml` file to
customize your build! For example, if you disagree with our compile flags you
can extend them like so:

```xml
<library>
  <collectors>
  	<!-- Warn about strict ISO C and C++ usage -->
  	<collect name="modm:build:ccflags">-Wpedantic</collect>
  	<!-- Don't warn about double promotion, I know what I'm doing (no, you don't) -->
  	<collect name="modm:build:ccflags">-Wno-double-promotion</collect>
  </collectors>
</library>
```

This can be significantly easier than manually editing the generated build
scripts, and is also persistent across library regeneration.
Please see the [`modm:build` documentation](../module/modm-build/#collectors)
for the available collectors and their inputs.
