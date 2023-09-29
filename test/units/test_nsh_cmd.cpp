#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_cmd.h>

using testing::ElementsAreArray;

static constexpr const char cmd_test_name[NSH_MAX_STRING_SIZE] = "test";
static nsh_status_t cmd_test_handler(const nsh_t*, unsigned int, char**)
{
    return NSH_STATUS_OK;
}

TEST(NshCmdInitEmpty, Success)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init_empty(&cmd);

    ASSERT_EQ(status, NSH_STATUS_OK);
    char expected_name[NSH_MAX_STRING_SIZE] = { '\0' };
    ASSERT_THAT(cmd.name, ElementsAreArray(expected_name));
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

TEST(NshCmdInit, FailureEmptyName)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, "", &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshCmdInit, FailureNameTooLong)
{
    constexpr size_t size_too_long = NSH_MAX_STRING_SIZE + 1;

    char name_too_long[size_too_long + 1] = { 0 }; // Keep one char for '\0'
    memset(name_too_long, 'a', size_too_long);

    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, name_too_long, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshCmdCopy, Success)
{
    nsh_cmd_t cmd_from;
    ASSERT_EQ(nsh_cmd_init(&cmd_from, cmd_test_name, &cmd_test_handler), NSH_STATUS_OK);

    nsh_cmd_t cmd_to;
    ASSERT_EQ(nsh_cmd_init_empty(&cmd_to), NSH_STATUS_OK);

    ASSERT_STRNE(cmd_to.name, cmd_from.name);
    ASSERT_NE(cmd_to.handler, cmd_from.handler);

    nsh_cmd_copy(&cmd_to, &cmd_from);

    ASSERT_STREQ(cmd_to.name, cmd_from.name);
    ASSERT_EQ(cmd_to.handler, cmd_from.handler);
}

TEST(NshCmdSwap, Success)
{
    nsh_cmd_t cmd_from;
    ASSERT_EQ(nsh_cmd_init(&cmd_from, cmd_test_name, &cmd_test_handler), NSH_STATUS_OK);

    nsh_cmd_t cmd_to;
    ASSERT_EQ(nsh_cmd_init_empty(&cmd_to), NSH_STATUS_OK);

    nsh_cmd_swap(&cmd_to, &cmd_from);

    ASSERT_STREQ(cmd_to.name, cmd_test_name);
    ASSERT_EQ(cmd_to.handler, &cmd_test_handler);
    ASSERT_STREQ(cmd_from.name, "");
    ASSERT_EQ(cmd_from.handler, nullptr);
}
