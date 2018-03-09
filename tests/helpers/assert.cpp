// =============================================================================
/*!
 * @file       tests/helpers/assert.cpp
 *
 * This file contains the implementation of the assert functionality in HAN-FUN
 *
 * @author     Filipe Alves <filipe.alves@bithium.com>
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2018 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"
#include "assert.h"

#include <cstdio>

__attribute__((weak))
void HF::Testing::Assert(const char *expr, const char *file, int line, const char *func)
{
   UNUSED(func);

   fprintf(stderr, "%s:%d: Assertion '%s' failed.\n", file, line, expr);
   abort();
}
