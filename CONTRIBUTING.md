# How to contribute

We love receiving pull requests. We use them ourselves for reviewing new features and fixing issues in xpcc.

To start contributing, please fork xpcc and then start a new branch on top of the `develop` branch.
We recommend prefixing your branch with
- `feature/` for new functionality, or
- `fix/` for a bug fix.

Please remember to have a look at [our](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.rst)
[coding](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.cpp)
[conventions](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.hpp).

Once you [open the pull request](https://github.com/roboterclubaachen/xpcc/pulls), we will review it and might ask you for changes.
Remember that you can continue to push to your `feature/*` or `fix/*` branches to update the corresponding pull requests!

Please note that your pull request will have to pass the CI tests in order to be merged.

Including unit tests is very welcomed and highly recommended. xpcc has its own simple and easy to use unit test framework. Unit tests are placed in the `test` subdirectory of every component (e.g. `xpcc/src/xpcc/math/filter/test`) and are run with `scons unittest`. [See here for more information about testing strategies in xpcc.](TESTING.md)

## When in doubt, ask

If you want to add a new platform, or other significant functionality, we [ask you to discuss that on the mailing list first](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev). We are very open to new suggestions, however, we might have different plans for a particular feature.

Communication is cheap, implementation is expensive and we don't want to frustrate you.
