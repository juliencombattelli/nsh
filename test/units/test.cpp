#include "gtest/gtest.h"

int add(int op1, int op2)
{
    return op1 + op2;
}

int sub(int op1, int op2)
{
    return op1 - op2;
}

TEST(CalcTest, Add)
{
    ASSERT_EQ(2, add(1, 1));
    ASSERT_EQ(5, add(3, 2));
    ASSERT_EQ(10, add(7, 3));
}

TEST(CalcTest, Sub)
{
    ASSERT_EQ(3, sub(5, 2));
    ASSERT_EQ(-10, sub(5, 15));
}