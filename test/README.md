# Testing modm

modm includes a large number of example as well as an extensive unit test suite
which can be compiled and execute on hosted and embedded targets. We've written
our own small test harness to be able to fit the tests onto very small devices
like AVRs. As part of our Continuous Integration checks, all examples are
compiled and hundreds of unit tests are executed to help us catch regressions.

However, there is room for improvement in both our test coverage, as well as our
test execution. For example, while most important platform-independent code is
covered in our unit tests,  the HAL is not tested systematically but only
manually whenever a developer executes an example on their hardware or works
on the HAL. Nevertheless, the quality is still fairly high, mostly due to the
relative simplicity of our HAL driver code.

Here are some guidelines and tips on how to make the best of what testing
abilities we have available today.


## Manual Tests

There is plenty of opportunity to customize modm, either using lbuild's module
options or simply by using the HAL on your specific hardware. This means the
more examples we have of different configurations and use-cases the better we
can debug existing problems and prevent new ones.

So when working on a fix or a new feature, make sure to create a simple example,
so that others have an idea of the environment in which it was developed.
Test your new code on real hardware with the compiler toolchain we use and make
sure to document any caveats you discover.

You can compile all or parts of the examples locally using our helper script.

```sh
cd examples
../tools/scripts/examples_compile.py .
# Or only a subset of the examples
../tools/scripts/examples_compile.py generic/blinky
```


## Continuous Integration

modm uses [GitHub Actions for CI][gha] to compile all the examples and to run
the unit tests on Linux. It also generates a bunch of artifacts that you can
download to check what the CI has been doing.

If you create a new folder inside `modm/examples`, you need to add it to the
appropriate job in in `.github/workflows/linux.yml`:

```yml
      - name: Examples for new or existing families
        if: always()
        run: |
          (cd examples && ../tools/scripts/examples_compile.py your_example_folder)
```

[gha]: https://github.com/modm-io/modm/actions


## Unit Tests

Our unit tests are located in the `modm/test` directory and are generated using
lbuild as well. They are all submodules of `modm:test` and are all platform
independent. We've written a small Makefile for the most commonly used commands.

```sh
cd tests
# generates, compiles and executes the unit tests for hosted targets
make run-hosted-linux
make run-hosted-darwin
# generates and compiles the unit tests for embedded targets
make compile-nucleo-f411re
make compile-nucleo-f103rb_A
make compile-nucleo-f103rb_B
make compile-arduino-nano_A # to _H
# executes the unit tests on the embedded targets
make run-nucleo-f411re
make run-nucleo-f103rb_A
make run-nucleo-f103rb_B
make run-arduino-nano_A # to _H
```

The embedded test targets all use the `modm::Board` interface to initialize the
targets and output unit tests results via the default serial connection.

The unit test library we use is located in `modm/src/unittest` which corresponds
to the `modm:unittest` modules. See the existing unit tests for examples on how
to write your own.

Fitting all unit tests into one executable image is not possible on smaller AVR
and STM32 targets. For these targets multiple compile targets generate multiple
images with partial unit tests that must be executed manually. In the future we
would like to execute the unit tests as well as the examples automatically on
the target hardware as well.


## Test all Targets

Apart from unit tests, we also generate the HAL (all modules in `modm:platform:**`)
for ALL targets we support and compile it with a simple GPIO example.
This doesn't catch everything, but it does make sure that these modules are at
least compilable, which is particularly interesting when writing new peripheral
drivers which may have different implementations on other targets.

```sh
 $ cd test/all
 $ make run-avr # run for all AVRs
 $ make run-stm32 # run for all STM32, this may take a while
 $ python3 run_all.py stm32f4
  o  stm32f410t8y         75.9s
  o  stm32f410cbt         81.9s
  ...
  o  stm32f479zgt         19.7s
  o  stm32f479ngh         20.3s

SUCCESS 209
FAIL    0
------------------
Total:  209
 $ make run-failed # only run those that failed the last test
```
