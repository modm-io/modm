# How to contribute

We love receiving pull requests. We use them ourselves for reviewing new features and fixing issues in MODM.

To start contributing, please fork MODM and then start a new branch on top of the `develop` branch.
We recommend prefixing your branch with
- `feature/` for new functionality, or
- `fix/` for a bug fix.

Please remember to have a look at [our coding conventions](docs/coding_convention.md).

Once you [open the pull request][pulls], we will review it and might ask you for changes.
Remember that you can continue to push to your `feature/*` or `fix/*` branches to update the corresponding pull requests!

Please note that your pull request will have to pass the CI tests in order to be merged.

Including unit tests is very welcomed and highly recommended. MODM has its own simple and easy to use unit test framework. Unit tests are placed in the `test` subdirectory (e.g. `test/modm/math/filter`) and are run with `make test-{hosted|stm32|avr}`. [See here for more information about testing strategies in MODM](docs/TESTING.md).

## When in doubt, ask

The dedicated maintainer of MODM is [@salkinium](https://github.com/salkinium) and you can always ping him.

If you want to add a new platform, or other significant functionality, we [ask you to discuss that in an issue first][issues]. We are very open to new suggestions, however, we might have different plans for a particular feature.

Communication is cheap, implementation is expensive and we don't want to frustrate you.

[issues]: https://github.com/modm-io/modm/issues
[pulls]: https://github.com//modm-io/modm/pulls