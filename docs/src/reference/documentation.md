# Documentation

Since modm is so highly configurable with its many options and selectable modules,
it can generate wildly different code for different project configurations.
However we've taken great care to keep modm discoverable by making documentation
a first-class citizen of our code generation engine `lbuild`.

modm's documentation is split into two parts:

1. The module documentation available during library discovery via `lbuild discover`.
2. The Doxygen API documentation available after generating the library with the
   `modm:docs` module via `lbuild build`.


## Modules

The modm modules and their options are [shown by `lbuild discover`](../../guide/discover).
These descriptions advertise the modules capabilities and gives a
high-level overview of how to use it.

You can search for keywords in all modules using `lbuild search QUERIES [QUERIES ...]`:

```
 $ lbuild search animation
Parser(lbuild)
╰── Repository(modm @ ../../..)   modm: a barebone embedded library generator
    ╰── Module(modm:ui)   User interface
        ├── Module(modm:ui:animation)   Animators
        ╰── Module(modm:ui:led)   LED Animation and Gamma Correction


>> modm:ui  [Module]

  4  Contains code for Graphics, Buttons, LEDs, Animations, Menu Structures.


>> modm:ui:led  [Module]

  1  # LED Animation and Gamma Correction
  6  `modm:ui:animation` module and providing look-up tables for performing
 12  wraps an 8-bit `modm::ui::Animation` and a 8-bit value.
 38  Depending on how smooth you require your animation to be, you may call the
 42  `modm::ui::KeyFrameAnimation` class and it's derivatives, to animate it's
 83  use the `modm::ui::table22_16_256` table to map the linear LED animation values
```

To make discovery even easier we have created a superimposed view of all
available modules on *this homepage*. Try searching for your keywords in the
search bar above.

Note that in this superimposed view, some Modules and Options are only available
for certain targets. These are then labels as "*{Module, Option, Collector} only
available for {target}*". Additionally the view does not show peripheral
instance submodule to reduce visual noise.


## Doxypress / Doxygen

The generated C/C++ API is documented using Doxypress and therefore only
available *after* code generation.

To view this documentation, include the `modm:docs` module in your `project.xml`
configuration and run `lbuild build`. A `modm/docs` folder is created containing
the `doxypress.json` for modm as well as all the `@defgroup`s mirroring the
module structure.

You must then run Doxypress manually, which can take several minutes, and
compiles the target-specific documentation into `modm/docs/html`:

```sh
lbuild build -m "modm:docs"

# With Doxypress
(cd modm/docs && doxypress doxypress.json)

# then open: modm/docs/html/index.html
```

The generated Doxypress/Doxygen documentation contains the original module
documentation *including the option choices you made*. This makes it easier to
map the lbuild options to changes in the generated source code.


## Online API Docs

API documentation is available at [docs.modm.io](https://docs.modm.io/) for a
large and representative number of targets with all modules enabled.
