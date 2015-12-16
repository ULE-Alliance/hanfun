// =============================================================================
/*!
 * @file       tests/helpers/assert.h
 *
 * This file is used to make the used to provide an override to HAN-FUN assert
 * functionality.
 *
 * @version    1.4.0
 *
 * @copyright  Copyright &copy; &nbsp; 2015 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#ifndef HF_ASSERT_HELPER_H
#define HF_ASSERT_HELPER_H

namespace HF
{
   namespace Testing
   {
      /*!
       * Helper function to test defined assertions.
       *
       * @param [in] expr  string representation of the assert condition.
       * @param [in] file  file name the assert was declared.
       * @param [in] line  line number in the above file the assert was declared.
       */
      void Assert(const char *expr, const char *file, int line);

   } // namespace Testing

}  // namespace HF

#define HF_ASSERT(_expr, _block)                          \
   {                                                      \
      if (!(_expr))                                       \
      {                                                   \
         HF::Testing::Assert(#_expr, __FILE__, __LINE__); \
         _block                                           \
      }                                                   \
   }

#endif /* HF_ASSERT_HELPER_H */
