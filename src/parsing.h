#ifndef PARSING_H_
#define PARSING_H_

#include <stdbool.h>

// Constants
// ---------

// The maximum length of a line in a scene stream (without the newline)
#define MAX_LINE_LENGTH 80
// The maximum number of tokens stored for a line
#define MAX_NUM_TOKENS 6

// Types
// -----

// A line split into blank-separated tokens
struct ParsedLine {
  // The number of tokens found on the line (may exceed MAX_NUM_TOKENS)
  unsigned int num_tokens;
  // The first MAX_NUM_TOKENS tokens of the line
  char tokens[MAX_NUM_TOKENS][MAX_LINE_LENGTH + 1];
  // The number of the line in the stream
  int line_number;
};

// Parsing
// -------

/**
 * Splits a line into tokens
 *
 * Tokens are separated by blanks (spaces or tabs). The line must not be
 * longer than MAX_LINE_LENGTH characters.
 *
 * @param line         The line to parse
 * @param parsed_line  The resulting parsed line
 * @param line_number  The number of the line
 */
void parse_line(const char* line,
                struct ParsedLine* parsed_line,
                int line_number);

/**
 * Checks that a token is a valid identifier
 *
 * Reports an error and exits if it is not.
 *
 * @param token        The token
 * @param line_number  The number of the line the token comes from
 * @param validate     Has the validate subcommand been used?
 */
void check_identifier(const char* token, int line_number, bool validate);

/**
 * Converts a token to an integer
 *
 * Reports an error and exits if the token is not a valid integer that fits
 * in an int.
 *
 * @param token        The token
 * @param line_number  The number of the line the token comes from
 * @param validate     Has the validate subcommand been used?
 * @return             The integer
 */
int parse_integer(const char* token, int line_number, bool validate);

/**
 * Converts a token to a strictly positive integer
 *
 * Reports an error and exits if the token is not a valid strictly positive
 * integer that fits in an int.
 *
 * @param token        The token
 * @param line_number  The number of the line the token comes from
 * @param validate     Has the validate subcommand been used?
 * @return             The integer
 */
int parse_positive_integer(const char* token, int line_number, bool validate);

#endif
