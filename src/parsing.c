#include "parsing.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "validation.h"

// The characters that separate tokens
#define BLANKS " \t"

/**
 * Converts a string made of digits (and an optional sign) to an int
 *
 * @param s        The string, already known to be a valid integer
 * @param result   The resulting integer
 * @return         true if and only if the value fits in an int
 */
static bool convert_to_int(const char* s, int* result) {
  errno = 0;
  long value = strtol(s, NULL, 10);
  if (errno == ERANGE || value < INT_MIN || value > INT_MAX)
    return false;
  *result = (int)value;
  return true;
}

void parse_line(const char* line,
                struct ParsedLine* parsed_line,
                int line_number) {
  char line_copy[MAX_LINE_LENGTH + 1];
  memset(parsed_line, 0, sizeof(*parsed_line));
  strncpy(line_copy, line, MAX_LINE_LENGTH);
  line_copy[MAX_LINE_LENGTH] = '\0';
  for (char* token = strtok(line_copy, BLANKS); token != NULL;
       token = strtok(NULL, BLANKS)) {
    if (parsed_line->num_tokens < MAX_NUM_TOKENS)
      strcpy(parsed_line->tokens[parsed_line->num_tokens], token);
    ++parsed_line->num_tokens;
  }
  parsed_line->line_number = line_number;
}

void check_identifier(const char* token, int line_number, bool validate) {
  if (!is_valid_id(token))
    report_error_invalid_identifier(token, line_number, validate);
}

int parse_integer(const char* token, int line_number, bool validate) {
  int value = 0;
  if (!is_valid_integer(token) || !convert_to_int(token, &value))
    report_error_invalid_int(token, line_number, validate);
  return value;
}

int parse_positive_integer(const char* token, int line_number, bool validate) {
  int value = 0;
  if (!is_valid_positive_integer(token) || !convert_to_int(token, &value))
    report_error_invalid_positive_int(token, line_number, validate);
  return value;
}
