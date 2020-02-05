# Documentation Generator

This module creates the documentation groups and hierarchy according to
the modm module structure into `modm/docs`.

The documentation can be generated using Doxygen or Doxypress.

## Doxygen

To generate the C++ documentation call Doxygen manually:

```
(cd modm/docs && doxygen doxyfile.cfg)
```

Then open `modm/docs/html/index.html` in your web browser of choice.

## Doxypress

[Install Doxypress.](https://www.copperspice.com/docs/doxypress/install-doxypress.html)

To generate the C++ documentation call Doxypress:

```
(cd modm/docs && doxypress doxypress.json)
```

Then open `modm/docs/html/index.html` in your web browser of choice.
