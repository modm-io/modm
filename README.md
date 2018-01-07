## CMSIS device headers for STM32

This folder includes the CMSIS device headers for all STM32 devices.
The files located here are part of the `STM32Cube` libraries and can be found inside the `STM32Cube*/Drivers/CMSIS/Device/ST/STM32F*xx/Include` folder.  

The files are copied and modified by converting all line endings from Windows to Unix style and removing all trailing whitespace (see `post_script.sh`). If you don't like that, stop complaining and do the work yourself.

Here is the list of the current device header version and release date as well as the Cube release version in braces:

- [L0: v1.7.1 created 25-November-2016 (CubeL0 v1.10.0)](http://www.st.com/en/embedded-software/stm32cubel0.html)
- [L1: v2.2.2 created 25-August-2017 (CubeL1 v1.8.0)](http://www.st.com/en/embedded-software/stm32cubel1.html)
- [L4: v1.4.0 created 25-August-2017 (CubeL4 v1.10.0)](http://www.st.com/en/embedded-software/stm32cubel4.html)
- [F0: v2.3.3 created 25-August-2017 (CubeF0 v1.9.0)](http://www.st.com/en/embedded-software/stm32cubef0.html)
- [F1: v4.1.0 created 29-April-2016 (CubeF1 v1.6.0)](http://www.st.com/en/embedded-software/stm32cubef1.html)
- [F2: v2.2.0 created 17-March-2017 (CubeF2 v1.7.0)](http://www.st.com/en/embedded-software/stm32cubef2.html)
- [F3: v2.3.2 created 23-June-2017 (CubeF3 v1.9.0)](http://www.st.com/en/embedded-software/stm32cubef3.html)
- [F4: v2.6.2 created 06-October-2017 (CubeF4 v1.18.0)](http://www.st.com/en/embedded-software/stm32cubef4.html)
- [F7: v1.2.1 created 25-August-2017 (CubeF7 v1.8.0)](http://www.st.com/en/embedded-software/stm32cubef7.html)
- [H7: v1.1.0 created 31-August-2017 (CubeH7 v1.1.0)](http://www.st.com/en/embedded-software/stm32cubeh7.html)

The Travis CI integration checks these versions daily and will fail, if they need updating: [![](https://travis-ci.org/modm-io/cmsis-header-stm32.svg?branch=master)](https://travis-ci.org/modm-io/cmsis-header-stm32)

The ST header files in this directory are available under the BSD 3-Clause License:
```
COPYRIGHT(c) {year} STMicroelectronics

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. Neither the name of STMicroelectronics nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
*
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
