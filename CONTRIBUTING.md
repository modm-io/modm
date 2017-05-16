# How to contribute

We love receiving pull requests. We use them ourselves for reviewing new features and fixing issues in modm.

To start contributing, please fork modm and then start a new branch on top of the `develop` branch.
We recommend prefixing your branch with
- `feature/` for new functionality, or
- `fix/` for a bug fix.

Please remember to have a look at [our coding conventions](https://github.com/roboterclubaachen/modm/blob/develop/doc/coding_convention.md).

Once you [open the pull request](https://github.com/roboterclubaachen/modm/pulls), we will review it and might ask you for changes.
Remember that you can continue to push to your `feature/*` or `fix/*` branches to update the corresponding pull requests!

Please note that your pull request will have to pass the CI tests in order to be merged.

Including unit tests is very welcomed and highly recommended. modm has its own simple and easy to use unit test framework. Unit tests are placed in the `test` subdirectory of every component (e.g. `modm/src/modm/math/filter/test`) and are run with `scons unittest`. [See here for more information about testing strategies in modm.](TESTING.md)

## When in doubt, ask

The dedicated maintainer of modm is [@salkinium](https://github.com/salkinium) and you can always ping him.

If you want to add a new platform, or other significant functionality, we [ask you to discuss that on the mailing list first](http://mailman.rwth-aachen.de/mailman/listinfo/modm-dev). We are very open to new suggestions, however, we might have different plans for a particular feature.

Communication is cheap, implementation is expensive and we don't want to frustrate you.
