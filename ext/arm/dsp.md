# ARM CMSIS-DSP

This module provides the CMSIS-DSP library.
Please [see the API documentation][docs] for details.

Note that this module splits up the library into submodule to speed up
compilation by only including the math functions you really need.
You can enable 16-bit floating point support separately via option.


## Configuration

You can define your own configuration in the `<arm_math_local.h>` file which
will automatically be included at the *beginning* of the `arm_math_types.h` file
which is included by all CMSIS-DSP source files. This way changes to your
CMSIS-DSP configuration will only affect its source files and not recompile your
whole project.

Example `<arm_math_local.h>` configuration:

```c
// Check the input and output sizes of matrices
#define ARM_MATH_MATRIX_CHECK 1
// enable manual loop unrolling in DSP functions
#define ARM_MATH_LOOPUNROLL 1
```


[docs]: https://arm-software.github.io/CMSIS-DSP/latest/
