#include "antenna.h"

#include <stdio.h>
#include <string.h>

bool load_antenna_from_parsed_line(const struct ParsedLine* parsed_line,
                                   struct Antenna* antenna,
                                   bool validate) {
  if (strcmp(parsed_line->tokens[0], "antenna") != 0)
    return false;
  int line_number = parsed_line->line_number;
  if (parsed_line->num_tokens != 5)
    report_error_line_wrong_arguments_number("antenna", line_number, validate);
  check_identifier(parsed_line->tokens[1], line_number, validate);
  memcpy(antenna->id, parsed_line->tokens[1], MAX_LENGTH_ID + 1);
  antenna->x = parse_integer(parsed_line->tokens[2], line_number, validate);
  antenna->y = parse_integer(parsed_line->tokens[3], line_number, validate);
  antenna->r =
    parse_positive_integer(parsed_line->tokens[4], line_number, validate);
  return true;
}

bool have_antennas_same_position(const struct Antenna* antenna1,
                                 const struct Antenna* antenna2) {
  return antenna1->x == antenna2->x && antenna1->y == antenna2->y;
}

bool antenna_covers_point(const struct Antenna* antenna,
                          long long x,
                          long long y) {
  // Doubles are exact here as long as coordinates stay below 2^26
  double dx = (double)(x - antenna->x);
  double dy = (double)(y - antenna->y);
  double r = (double)antenna->r;
  return dx * dx + dy * dy <= r * r;
}

void print_antenna(const struct Antenna* antenna) {
  printf("  antenna %s at %d %d with range %d\n", antenna->id, antenna->x,
         antenna->y, antenna->r);
}
