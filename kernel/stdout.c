#include <file.h>
#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <serial.h>

// Store the virtual address of the video memory buffer
static uint16_t *const framebuffer = (uint16_t *)(physical_to_virtual(0xb8000));
// VGA text mode has a fixed number of columns and rows
static const int NCOLUMNS = 80;
static const int NROWS = 25;

// Keep track of the current cursor position.
static int index = 0;

// The style part of a character is 1 byte: bits 0-4 are the
// foreground and the rest are background. Light Grey on Black.
static const uint16_t ENTRY_STYLE = 0x07;

// It will be useful to have a constant value for a blank character,
// so we don't need to calculate it every time we clear the screen.
// This is just a space.
static const uint16_t BLANK_ENTRY = ' ' | (ENTRY_STYLE << 8);

/**
 * @brief      Finds the current column on the cursor.
 *
 * @details    Finds the index of the cursor within the current row.
 *
 * @return     return Index of the current column.
 */
static inline int __current_column(void) { return index % NCOLUMNS; }

/**
 * @brief      Clear the screen.
 *
 * @details    Write spaces (' ') in each cell on the screen.
 */
static inline void __clear(void) {
  // Add blank entries to framebuffer
  for (int i = 0; i < (NCOLUMNS * NROWS); i++)
    framebuffer[i] = BLANK_ENTRY;
}

/**
 * @brief      Prints one character.
 *
 * @details    Process a signle ascii code and, if it is a visible
 *             character, prints it, or, if it is a control character,
 *             handles it.
 *
 * @param      c An ascii character code.
 *
 * @return     return 0 on success and -1 on failure.
 */
static int __putchar(int c) {
  // If this is a printable character (32-127), simply print to the
  // screen.
  if (c >= 32 && c <= 127) {
    framebuffer[index++] = (((uint16_t)c) | (ENTRY_STYLE << 8));
  }
  // Handle backspace. Simply move the cursor back on character. Don't
  // move back past to the previous line.
  else if (c == '\b') {
    if (__current_column() > 0)
      index--;
  }
  // Handle horizontal tabs. Move to the next tab stop (every 8
  // columns). If we are at the end of the current line, simply move
  // to the next.
  else if (c == '\t') {
    index++; // Always advance at least once
    if (__current_column() > 0)
      index += 7 - (__current_column() % 8);
  }
  // Handle newlines. Move to the beginning of the next line.
  else if (c == '\n') {
    index += NCOLUMNS - __current_column();
  }
  // Handle form feed. Clear the screen.
  else if (c == '\f') {
    __clear();
  }
  // Handle carriage returns. Move to the beginning of the current line.
  else if (c == '\r') {
    index -= __current_column();
  }
  // Handle invalid characters. Return -1.
  else {
    return -1;
  }

  // Now that index has been updated we need to check whether to
  // scroll the screen to make a new blank line.
  if (index == (NCOLUMNS * NROWS)) {
    // Set index to the start of the second to last row
    index = NCOLUMNS * (NROWS - 1);

    // Shift the framebuffer up one row.
    for (int i = 0; i < index; i++)
      framebuffer[i] = framebuffer[i + NCOLUMNS];

    // Clear the last line.
    for (int i = index; i < (NCOLUMNS * NROWS); i++)
      framebuffer[i] = BLANK_ENTRY;
  }

  return 0;
}

/**
 * @brief      Implements fwrite for stdout.
 *
 * @details    Writes the given data buffer as a string to the screen a
 *             video memory buffer assuming VGA text mode. Each
 *             element will be cast to an unsigned char before being
 *             written to the buffer and this function will return at
 *             the first invalid ascii code it finds. the location
 *             given by pointer to the stream.
 *
 * @param      ptr The location from which to read data.
 * @param      size The size of each elements should always be 1. This
 *             value is ignored.
 * @param      nmemb The number of elements to write.
 * @param      stream The FILE to write data to. This value is ignored.
 *
 * @return     The number of elements written.
 */
static size_t __stdout_fwrite(const void *ptr, size_t size, size_t nmemb,
                              FILE *stream) {
  nmemb = fwrite(ptr, size, nmemb, COM1);

  size_t ret = 0;

  const char *cur = (const char *)ptr;
  const char *end = (cur + nmemb);

  for (; cur < end; cur++) {
    const char c = *cur;
    if (__putchar((int)c) < 0) {
      break;
    }
    ret++;
  }

  return ret;
}

FILE __stdout = {.fwrite = __stdout_fwrite};

FILE *stdout = &__stdout;

/**
 * @brief      Prepares stdout for writing.
 *
 * @details    Clears the screen and disables the cursor.
 */
void stdout_init(void) {
  serial_init(COM1);

  __clear();

  // Disable cursor
  asm("pushf;"
      "push %rax;"
      "push %rdx;"

      "mov  $0x3d4,%dx;"
      "mov  $0xa,%al;"
      "out  %al,%dx;"

      "inc  %dx;"
      "mov  $0x20,%al;"
      "out  %al,%dx;"

      "pop  %rdx;"
      "pop  %rax;"
      "popf;");
}
