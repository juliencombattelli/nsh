#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_cmd_array.h>

using testing::Each;
using testing::FieldsAre;
using testing::StrEq;

static const char cmd_test_name[NSH_MAX_STRING_SIZE] = "test";
static int cmd_test_handler(int, char**)
{
    return 0;
}

TEST(NshCmdArrayInit, Success)
{
    nsh_cmd_array_t cmds;

    auto status = nsh_cmd_array_init(&cmds);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(cmds.count, 0);
    ASSERT_THAT(cmds.array, Each(FieldsAre(nullptr, StrEq(""))));
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