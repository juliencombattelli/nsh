#include <gtest/gtest.h>
#include <string.h>

char* getcwd(char* buf, size_t size)
{
    const char cwd[] = "./";
    if (sizeof(cwd) > size) {
        return NULL;
    }
    strncpy(buf, "./", size);
    return buf;
}

int mkdir(const char *path, mode_t mode) 
{
    return 0;
}

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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}