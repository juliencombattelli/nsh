#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_cmd.h>

using testing::ElementsAreArray;

TEST(NshCmdInitEmpty, Success)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init_empty(&cmd);

    ASSERT_EQ(status, NSH_STATUS_OK);
    char expected_name[NSH_MAX_STRING_SIZE] = { '\0' };
    ASSERT_THAT(cmd.name, ElementsAreArray(expected_name));
}

TEST(NshCmdInitEmpty, FailureNullCmd)
{
    auto status = nsh_cmd_init_empty(nullptr);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

static const char cmd_test_name[NSH_MAX_STRING_SIZE] = "test";
static int cmd_test_handler(int, char**)
{
    return 0;
}

TEST(NshCmdInit, Success)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_THAT(cmd.name, ElementsAreArray(cmd_test_name));
    ASSERT_EQ(cmd.handler, &cmd_test_handler);
}

TEST(NshCmdInit, SuccessNullHandler)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, cmd_test_name, nullptr);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_THAT(cmd.name, ElementsAreArray(cmd_test_name));
    ASSERT_EQ(cmd.handler, nullptr);
}

TEST(NshCmdInit, FailureNullCmd)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(nullptr, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshCmdInit, FailureNullName)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, nullptr, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}