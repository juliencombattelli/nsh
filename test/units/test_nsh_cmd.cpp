#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_cmd.h>

using testing::Each;
using testing::ElementsAreArray;
using testing::FieldsAre;
using testing::StrEq;

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
    auto status = nsh_cmd_init(nullptr, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshCmdInit, FailureNullName)
{
    nsh_cmd_t cmd;
    auto status = nsh_cmd_init(&cmd, nullptr, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
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

// bool operator==(const nsh_cmd_t& a, const nsh_cmd_t& b) const {
//     return a.name
// }

TEST(NshCmdArrayInit, Success)
{
    nsh_cmd_array_t cmds;

    auto status = nsh_cmd_array_init(&cmds);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(cmds.count, 0);
    ASSERT_THAT(cmds.array, Each(FieldsAre(nullptr, StrEq(""))));
}

TEST(NshCmdArrayInit, FailureNullCmdArray)
{
    auto status = nsh_cmd_array_init(nullptr);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshCmdArrayRegister, Success1Element)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    auto status = nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(cmds.count, 1);
    ASSERT_STREQ(cmds.array[0].name, cmd_test_name);
    ASSERT_EQ(cmds.array[0].handler, &cmd_test_handler);
}

TEST(NshCmdArrayRegister, SuccessMaxElement)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    for (auto i = 0u; i < NSH_CMD_MAX_COUNT; i++) {
        ASSERT_EQ(nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler), NSH_STATUS_OK);
    }

    auto status = nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_MAX_CMD_NB_REACH);
}