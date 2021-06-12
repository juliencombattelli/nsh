#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nsh/nsh_line_buffer.hpp>

TEST(NshLineBufferReset, Success)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(line.size, 0);
}

TEST(NshLineBufferAppendChar, SuccessOneChar)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    auto status = nsh_line_buffer_append_char(&line, 'a');

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(line.size, 1);
    ASSERT_EQ(line.buffer[0], 'a');
}

TEST(NshLineBufferAppendChar, SuccessThreeChar)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(nsh_line_buffer_append_char(&line, 'n'), NSH_STATUS_OK);
    ASSERT_EQ(nsh_line_buffer_append_char(&line, 's'), NSH_STATUS_OK);
    ASSERT_EQ(nsh_line_buffer_append_char(&line, 'h'), NSH_STATUS_OK);

    ASSERT_EQ(line.size, 3);
    ASSERT_EQ(line.buffer[0], 'n');
    ASSERT_EQ(line.buffer[1], 's');
    ASSERT_EQ(line.buffer[2], 'h');
}

TEST(NshLineBufferAppendChar, SuccessMaxChar)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE - 1; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    auto status = nsh_line_buffer_append_char(&line, 'z');

    ASSERT_EQ(status, NSH_STATUS_OK);
    ASSERT_EQ(line.size, NSH_LINE_BUFFER_SIZE);
    ASSERT_EQ(line.buffer[NSH_LINE_BUFFER_SIZE - 1], 'z');
}

TEST(NshLineBufferAppendChar, FailureFull)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    auto status = nsh_line_buffer_append_char(&line, 'z');

    ASSERT_EQ(status, NSH_STATUS_BUFFER_OVERFLOW);
    ASSERT_EQ(line.size, NSH_LINE_BUFFER_SIZE);
}

TEST(NshLineBufferAppendNull, Success)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    nsh_line_buffer_append_null(&line);

    ASSERT_EQ(line.size, 1);
    ASSERT_EQ(line.buffer[0], 0);
}

TEST(NshLineBufferAppendNull, SuccessEndOfBuffer)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE - 1; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }
    nsh_line_buffer_append_null(&line);

    ASSERT_EQ(line.size, NSH_LINE_BUFFER_SIZE);
    ASSERT_EQ(line.buffer[NSH_LINE_BUFFER_SIZE - 2], 'a');
    ASSERT_EQ(line.buffer[NSH_LINE_BUFFER_SIZE - 1], '\0');
}

TEST(NshLineBufferAppendNull, SuccessLastCharOverwritten)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    ASSERT_EQ(line.size, NSH_LINE_BUFFER_SIZE);
    ASSERT_EQ(line.buffer[NSH_LINE_BUFFER_SIZE - 1], 'a');

    nsh_line_buffer_append_null(&line);

    ASSERT_EQ(line.size, NSH_LINE_BUFFER_SIZE);
    ASSERT_EQ(line.buffer[NSH_LINE_BUFFER_SIZE - 1], '\0');
}

TEST(NshLineBufferEraseLastChar, Success)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    nsh_line_buffer_append_char(&line, 'a');

    ASSERT_EQ(line.size, 1);
    nsh_line_buffer_erase_last_char(&line);
    ASSERT_EQ(line.size, 0);
}

TEST(NshLineBufferEraseLastChar, SuccessEmpty)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(line.size, 0);
    nsh_line_buffer_erase_last_char(&line);
    ASSERT_EQ(line.size, 0);
}

TEST(NshLineBufferIsFull, Success)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE - 1; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    ASSERT_EQ(nsh_line_buffer_is_full(&line), false);
}

TEST(NshLineBufferIsFull, SuccessFull)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    ASSERT_EQ(nsh_line_buffer_is_full(&line), true);
}

TEST(NshLineBufferIsFull, SuccessEmpty)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(nsh_line_buffer_is_full(&line), false);
}

TEST(NshLineBufferIsEmpty, Success)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);

    ASSERT_EQ(nsh_line_buffer_is_empty(&line), false);
}

TEST(NshLineBufferIsEmpty, SuccessFull)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    for (unsigned int i = 0; i < NSH_LINE_BUFFER_SIZE; i++) {
        ASSERT_EQ(nsh_line_buffer_append_char(&line, 'a'), NSH_STATUS_OK);
    }

    ASSERT_EQ(nsh_line_buffer_is_empty(&line), false);
}

TEST(NshLineBufferIsEmpty, SuccessEmpty)
{
    nsh_line_buffer_t line;
    nsh_line_buffer_reset(&line);

    ASSERT_EQ(nsh_line_buffer_is_empty(&line), true);
}