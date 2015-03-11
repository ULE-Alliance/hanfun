// =============================================================================
/*!
 * @file       tests/helpers/memory.h
 *
 * This file is used to make the used STL classes work with CppUTest memory leak
 * functionality.
 *
 * @version    1.2.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#ifndef HF_MEMORY_HELPER_H
#define HF_MEMORY_HELPER_H

#include <forward_list>

#ifdef HF_USE_EASTL
#include <EASTL/memory.h>
#include <EASTL/list.h>
#include <EASTL/set.h>
#include <EASTL/vector.h>
#endif

#include "cpputest/config.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"

#endif /* HF_MEMORY_HELPER_H */
