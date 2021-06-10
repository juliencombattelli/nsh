#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_cmd_array.h>

using testing::Each;
using testing::FieldsAre;
using testing::StrEq;

static constexpr const char cmd_test_name[NSH_MAX_STRING_SIZE] = "test";
static nsh_status_t cmd_test_handler(unsigned int, char**)
{
    return NSH_STATUS_OK;
}

TEST(NshCmdArrayInit, Success)
{
    nsh_cmd_array_t cmds;

    auto status = nsh_cmd_array_init(&cmds);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(cmds.count, 0);
    ASSERT_THAT(cmds.array, Each(FieldsAre(nullptr, StrEq(""))));
}

TEST(NshCmdArrayRegister, SuccessOneElement)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    auto status = nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(cmds.count, 1);
    ASSERT_STREQ(cmds.array[0].name, cmd_test_name);
    ASSERT_EQ(cmds.array[0].handler, &cmd_test_handler);
}

TEST(NshCmdArrayRegister, SuccessMaxElements)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    for (auto i = 0u; i < NSH_CMD_MAX_COUNT; i++) {
        ASSERT_EQ(nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler), NSH_STATUS_OK);
        ASSERT_EQ(cmds.count, i + 1);
        ASSERT_STREQ(cmds.array[i].name, cmd_test_name);
        ASSERT_EQ(cmds.array[i].handler, &cmd_test_handler);
    }
}

TEST(NshCmdArrayRegister, FailureTooManyElements)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    for (auto i = 0u; i < NSH_CMD_MAX_COUNT; i++) {
        ASSERT_EQ(nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler), NSH_STATUS_OK);
    }

    auto status = nsh_cmd_array_register(&cmds, cmd_test_name, &cmd_test_handler);

    ASSERT_EQ(status, NSH_STATUS_MAX_CMD_NB_REACH);
}

TEST(NshCmdArrayFindMatching, Success)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd1_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd2_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd3_test", &cmd_test_handler), NSH_STATUS_OK);

    static constexpr const char searched_for[] = "cmd2_";
    auto* cmd = nsh_cmd_array_find_matching(&cmds, searched_for, sizeof(searched_for) - 1);

    ASSERT_NE(cmd, nullptr);
    ASSERT_STREQ(cmd->name, "cmd2_test");
}

TEST(NshCmdArrayFindMatching, FailureEmpty)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    static constexpr const char searched_for[] = "cmd2_";
    auto* cmd = nsh_cmd_array_find_matching(&cmds, searched_for, sizeof(searched_for));

    ASSERT_EQ(cmd, nullptr);
}

TEST(NshCmdArrayFindMatching, FailurePopulated)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd1_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd2_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd3_test", &cmd_test_handler), NSH_STATUS_OK);

    static constexpr const char searched_for[] = "cmd2_";
    auto* cmd = nsh_cmd_array_find_matching(&cmds, searched_for, sizeof(searched_for));

    ASSERT_EQ(cmd, nullptr);
}

TEST(NshCmdArrayFind, Success)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd1_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd2_test", &cmd_test_handler), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd3_test", &cmd_test_handler), NSH_STATUS_OK);

    static constexpr const char searched_for[] = "cmd2_test";
    auto* cmd = nsh_cmd_array_find(&cmds, searched_for);

    ASSERT_NE(cmd, nullptr);
    ASSERT_STREQ(cmd->name, "cmd2_test");
}

static nsh_status_t cmd1(unsigned int, char**)
{
    return static_cast<nsh_status_t>(1);
}
static nsh_status_t cmd2(unsigned int, char**)
{
    return static_cast<nsh_status_t>(2);
}
static nsh_status_t cmd3(unsigned int, char**)
{
    return static_cast<nsh_status_t>(3);
}

TEST(NshCmdArrayLexicographicSort, Success)
{
    nsh_cmd_array_t cmds;
    ASSERT_EQ(nsh_cmd_array_init(&cmds), NSH_STATUS_OK);

    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd3_test", &cmd3), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd1_test", &cmd1), NSH_STATUS_OK);
    ASSERT_EQ(nsh_cmd_array_register(&cmds, "cmd2_test", &cmd2), NSH_STATUS_OK);

    auto status = nsh_cmd_array_lexicographic_sort(&cmds);
    ASSERT_EQ(status, NSH_STATUS_OK);

    ASSERT_STREQ(cmds.array[0].name, "cmd1_test");
    ASSERT_EQ(cmds.array[0].handler, &cmd1);
    ASSERT_STREQ(cmds.array[1].name, "cmd2_test");
    ASSERT_EQ(cmds.array[1].handler, &cmd2);
    ASSERT_STREQ(cmds.array[2].name, "cmd3_test");
    ASSERT_EQ(cmds.array[2].handler, &cmd3);
}