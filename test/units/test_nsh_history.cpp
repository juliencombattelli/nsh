#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_history.h>

using testing::ElementsAreArray;

static constexpr const char cmd_test_name[NSH_MAX_STRING_SIZE] = "test";
static int cmd_test_handler(int, char**)
{
    return 0;
}

TEST(NshHistoryReset, Success)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    ASSERT_EQ(hist.head, 0);
    ASSERT_EQ(hist.tail, 0);
    ASSERT_EQ(hist.size, 0);
}

TEST(NshHistoryAddEntry, SuccessOneElement)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    nsh_history_add_entry(&hist, new_entry);

    ASSERT_EQ(hist.size, 1);
    ASSERT_EQ(hist.head, 1);
    ASSERT_EQ(hist.tail, 0);
    ASSERT_STREQ(hist.entries[0], new_entry);
}

TEST(NshHistoryAddEntry, SuccessTwoElement)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry1[] = "new_entry1";
    const char new_entry2[] = "new_entry2";
    nsh_history_add_entry(&hist, new_entry1);
    nsh_history_add_entry(&hist, new_entry2);

    ASSERT_EQ(hist.size, 2);
    ASSERT_EQ(hist.head, 2);
    ASSERT_EQ(hist.tail, 0);
    ASSERT_STREQ(hist.entries[0], new_entry1);
    ASSERT_STREQ(hist.entries[1], new_entry2);
}

TEST(NshHistoryAddEntry, SuccessWhenOverriding)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    for (int i = 0; i < NSH_CMD_HISTORY_SIZE; i++) {
        nsh_history_add_entry(&hist, new_entry);
    }

    ASSERT_EQ(hist.size, NSH_CMD_HISTORY_SIZE);
    ASSERT_EQ(hist.head, 0);
    ASSERT_EQ(hist.tail, 0);

    const char entry_override[] = "overriden";
    nsh_history_add_entry(&hist, entry_override);

    ASSERT_EQ(hist.size, NSH_CMD_HISTORY_SIZE);
    ASSERT_EQ(hist.head, 1);
    ASSERT_EQ(hist.tail, 1);
    ASSERT_STREQ(hist.entries[0], entry_override);
    ASSERT_STREQ(hist.entries[1], new_entry);
}

TEST(NshHistoryEntryCount, SuccessAtInit)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    ASSERT_EQ(nsh_history_entry_count(&hist), 0);
}

TEST(NshHistoryEntryCount, SuccessWhenFull)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    for (int i = 0; i < NSH_CMD_HISTORY_SIZE; i++) {
        nsh_history_add_entry(&hist, new_entry);
    }

    ASSERT_EQ(nsh_history_entry_count(&hist), NSH_CMD_HISTORY_SIZE);
}

TEST(NshHistoryEntryCount, SuccessWhenOverriding)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    for (int i = 0; i < NSH_CMD_HISTORY_SIZE; i++) {
        nsh_history_add_entry(&hist, new_entry);
    }

    const char entry_override[] = "overriden";
    nsh_history_add_entry(&hist, entry_override);

    ASSERT_EQ(nsh_history_entry_count(&hist), NSH_CMD_HISTORY_SIZE);
}

TEST(NshHistoryIsEmpty, SuccessEmpty)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    ASSERT_EQ(nsh_history_is_empty(&hist), true);
}

TEST(NshHistoryIsEmpty, SuccessNotEmpty)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    nsh_history_add_entry(&hist, new_entry);

    ASSERT_EQ(nsh_history_is_empty(&hist), false);
}

TEST(NshHistoryGetEntry, SuccessFirstElement)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    nsh_history_add_entry(&hist, new_entry);

    char entry[NSH_LINE_BUFFER_SIZE] = {};
    auto status = nsh_history_get_entry(&hist, 0, entry);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_STREQ(entry, new_entry);
}

TEST(NshHistoryGetEntry, SuccessWhenOverriding)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    const char new_entry[] = "new_entry";
    for (int i = 0; i < NSH_CMD_HISTORY_SIZE; i++) {
        nsh_history_add_entry(&hist, new_entry);
    }

    const char entry_override[] = "overriden";
    nsh_history_add_entry(&hist, entry_override);

    char entry[NSH_LINE_BUFFER_SIZE] = {};
    auto status = nsh_history_get_entry(&hist, 1, entry);

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_STREQ(entry, new_entry);
}

TEST(NshHistoryGetEntry, FailureInvalidNegativeAge)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    char entry[NSH_LINE_BUFFER_SIZE] = {};
    auto status = nsh_history_get_entry(&hist, -1, entry);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}

TEST(NshHistoryGetEntry, FailureInvalidTooHighAge)
{
    nsh_history_t hist;
    nsh_history_reset(&hist);

    char entry[NSH_LINE_BUFFER_SIZE] = {};
    auto status = nsh_history_get_entry(&hist, 1, entry);

    ASSERT_EQ(status, NSH_STATUS_WRONG_ARG);
}