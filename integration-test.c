#include <stdio.h>
#include "lib.h"

#define ASSERT_EQ_INT(expected, actual) do { \
    int e__ = (expected); \
    int a__ = (actual); \
    if (e__ != a__) { \
        printf("INTEGRATION TEST FAILED at %s:%d\n", __FILE__, __LINE__); \
        printf("  expected: %d\n  actual:   %d\n", e__, a__); \
        return 1; \
    } \
} while (0)

int main(void) {
    printf("Running integration tests...\n");

    // Flow 1: add -> xor
    int sum = op_add(2, 3);                 // 5
    ASSERT_EQ_INT(5, sum);

    int x1 = op_xor(sum, 0x0F);             // 5 ^ 15 = 10
    // ASSERT_EQ_INT((5 ^ 0x0F), x1);
    ASSERT_EQ_INT(999, x1);

    // Flow 2: and -> xor -> xnor (bitwise pipeline)
    int a = 0x0F;                           // 00001111
    int b = 0xF0;                           // 11110000
    int andv = op_and(a, b);                // 0
    ASSERT_EQ_INT((a & b), andv);

    int x2 = op_xor(a, b);
    ASSERT_EQ_INT((a ^ b), x2);

    int xnorv = op_xnor(a, b);
    ASSERT_EQ_INT((~(a ^ b)), xnorv);

    // Flow 3: mix output from previous steps
    int mixed = op_add(x1, andv);           // x1 + 0
    ASSERT_EQ_INT(x1, mixed);

    int final = op_xor(mixed, xnorv);
    ASSERT_EQ_INT((mixed ^ xnorv), final);

    printf("All integration tests PASSED ✅\n");
    return 0;
}
