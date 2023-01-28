/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2021 Niklas Hauser

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_ERROR_HANDLER_EXTERNAL_INCLUDED
#define ETL_ERROR_HANDLER_EXTERNAL_INCLUDED

/* Include the original header from ETL */
#include "error_handler_internal.h"

/* Only redefine errors if necessary */
#if !defined(ETL_NO_CHECKS) && !defined(ETL_THROW_EXCEPTIONS) && !defined(ETL_LOG_ERRORS)

/* Undefine the error handlers */
#undef ETL_ASSERT
#undef ETL_ASSERT_AND_RETURN
#undef ETL_ASSERT_AND_RETURN_VALUE
#undef ETL_ALWAYS_ASSERT
#undef ETL_ALWAYS_ASSERT_AND_RETURN
#undef ETL_ALWAYS_ASSERT_AND_RETURN_VALUE
#undef ETL_ERROR

/* Define the error handlers to use modm_assert */
#include <modm/architecture/interface/assert.h>

#define ETL_ASSERT(b, e)                         {         modm_assert((b), "etl", (e)); }
#define ETL_ASSERT_AND_RETURN(b, e)              { if (not modm_assert_continue_ignore((b), "etl", (e))) return; }
#define ETL_ASSERT_AND_RETURN_VALUE(b, e, v)     { if (not modm_assert_continue_ignore((b), "etl", (e))) return (v); }

#define ETL_ALWAYS_ASSERT(e)                     ETL_ASSERT(false, e)
#define ETL_ALWAYS_ASSERT_AND_RETURN(e)          ETL_ASSERT_AND_RETURN(false, e)
#define ETL_ALWAYS_ASSERT_AND_RETURN_VALUE(e, v) ETL_ASSERT_AND_RETURN_VALUE(false, e, v)

#include <modm/architecture/utils.hpp>

#if defined(ETL_VERBOSE_ERRORS)
  #define ETL_ERROR(e) (__FILE__ ":" MODM_STRINGIFY(__LINE__) " -> \"" MODM_STRINGIFY(e) "\"")
#else
  #define ETL_ERROR(e) (MODM_STRINGIFY(__LINE__) " -> \"" MODM_STRINGIFY(e) "\"")
#endif

#endif /* ETL_NO_CHECKS */

#endif /* ETL_ERROR_HANDLER_EXTERNAL_INCLUDED */
