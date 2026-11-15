#include "validation.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Validation
// ----------

bool is_valid_id(const char* s) {
  if (*s == '\0' || strlen(s) > MAX_LENGTH_ID)
    return false;
  if (!isalpha((unsigned char)*s) && *s != '_')
    return false;
  while (*s != '\0') {
    if (!isalnum((unsigned char)*s) && *s != '_')
      return false;
    ++s;
  }
  return true;
}

bool is_valid_integer(const char* s) {
  if (strcmp(s, "0") == 0)
    return true;
  if (*s == '-')
    ++s;
  return is_valid_positive_integer(s);
}

bool is_valid_positive_integer(const char* s) {
  if (*s < '1' || *s > '9')
    return false;
  ++s;
  while (*s != '\0') {
    if (!isdigit((unsigned char)*s))
      return false;
    ++s;
  }
  return true;
}

// Error reporting
// ---------------

/**
 * Prints "not ok" on stdout if the validate subcommand is used
 *
 * @param validate  Has the validate subcommand been used?
 */
static void announce_failure(bool validate) {
  if (validate)
    printf("not ok\n");
}

void report_error_non_unique_identifiers(const char* object,
                                         const char* id,
                                         bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: %s identifier %s is non unique\n", object, id);
  exit(1);
}

void report_error_invalid_identifier(const char* id,
                                     int line_number,
                                     bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: invalid identifier \"%s\" (line #%d)\n", id,
          line_number);
  exit(1);
}

void report_error_invalid_int(const char* s, int line_number, bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: invalid integer \"%s\" (line #%d)\n", s, line_number);
  exit(1);
}

void report_error_invalid_positive_int(const char* s,
                                       int line_number,
                                       bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: invalid positive integer \"%s\" (line #%d)\n", s,
          line_number);
  exit(1);
}

void report_error_scene_first_line(bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: first line must be exactly 'begin scene'\n");
  exit(1);
}

void report_error_scene_last_line(bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: last line must be exactly 'end scene'\n");
  exit(1);
}

void report_error_unrecognized_line(int line_number, bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: unrecognized line (line #%d)\n", line_number);
  exit(1);
}

void report_error_empty_line(int line_number, bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: line has no token (line #%d)\n", line_number);
  exit(1);
}

void report_error_line_too_long(int line_number, bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: line is too long (line #%d)\n", line_number);
  exit(1);
}

void report_error_line_wrong_arguments_number(const char* object,
                                              int line_number,
                                              bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: %s line has wrong number of arguments (line #%d)\n",
          object, line_number);
  exit(1);
}

void report_error_overlapping_objects(const char* object1,
                                      const char* id1,
                                      const char* object2,
                                      const char* id2,
                                      bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: %s %s and %s %s are overlapping\n", object1, id1,
          object2, id2);
  exit(1);
}

void report_error_same_position_antennas(const char* id1,
                                         const char* id2,
                                         bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: antennas %s and %s have the same position\n", id1,
          id2);
  exit(1);
}

void report_error_allocation_failure(const char* type, bool validate) {
  announce_failure(validate);
  fprintf(stderr, "error: memory allocation failed for %s\n", type);
  exit(1);
}

void report_error_mandatory_subcommand(void) {
  fprintf(stderr, "error: subcommand is mandatory\n");
  exit(1);
}

void report_error_unrecognized_subcommand(const char* subcommand) {
  fprintf(stderr, "error: subcommand '%s' is not recognized\n", subcommand);
  exit(1);
}

void report_error_missing_output_file(void) {
  fprintf(stderr, "error: subcommand 'draw' requires an output file\n");
  exit(1);
}

void report_error_cannot_write_file(const char* filename) {
  fprintf(stderr, "error: cannot write file '%s'\n", filename);
  exit(1);
}
