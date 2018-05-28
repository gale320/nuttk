
#include "nutcommon.h"


/**
 * String comparator function.
 *
 * @param[in] str1 first string
 * @param[in] str2 second string
 *
 * @return
 */
int nut_common_cmp_str(const void *str1, const void *str2)
{
    return strcmp((const char*) str1, (const char*) str2);
}

/**
 * Pointer comparator function.
 *
 * @param[in] ptr1 first pointer
 * @param[in] ptr2 second pointer
 *
 * @return
 */
int nut_common_cmp_ptr(const void *ptr1, const void *ptr2)
{
    if (ptr1 < ptr2)
        return -1;
    else if (ptr1 > ptr2)
        return 1;
    return 0;
}
