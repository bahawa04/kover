#include "construction.h"

#include <stdio.h>
#include <string.h>

// Private functions
// =================

/**
 * Returns the number of corners of a construction covered by at least one
 * antenna
 *
 * @param construction  The construction
 * @param antennas      The antennas
 * @param num_antennas  The number of antennas
 * @return              The number of covered corners (between 0 and 4)
 */
static int num_corners_covered(const struct Construction* construction,
                               const struct Antenna* antennas,
                               unsigned int num_antennas) {
  int num_covered = 0;
  for (int dx = -1; dx <= 1; dx += 2)
    for (int dy = -1; dy <= 1; dy += 2) {
      long long cx = construction->x + (long long)dx * construction->w;
      long long cy = construction->y + (long long)dy * construction->h;
      for (unsigned int a = 0; a < num_antennas; ++a)
        if (antenna_covers_point(antennas + a, cx, cy)) {
          ++num_covered;
          break;
        }
    }
  return num_covered;
}

// Public functions
// ================

bool load_construction_from_parsed_line(const struct ParsedLine* parsed_line,
                                        struct Construction* construction,
                                        bool validate) {
  const char* keyword = parsed_line->tokens[0];
  if (strcmp(keyword, "building") != 0 && strcmp(keyword, "house") != 0)
    return false;
  int line_number = parsed_line->line_number;
  if (parsed_line->num_tokens != 6)
    report_error_line_wrong_arguments_number(keyword, line_number, validate);
  check_identifier(parsed_line->tokens[1], line_number, validate);
  memcpy(construction->id, parsed_line->tokens[1], MAX_LENGTH_ID + 1);
  construction->type = strcmp(keyword, "building") == 0 ? BUILDING : HOUSE;
  construction->x =
    parse_integer(parsed_line->tokens[2], line_number, validate);
  construction->y =
    parse_integer(parsed_line->tokens[3], line_number, validate);
  construction->w =
    parse_positive_integer(parsed_line->tokens[4], line_number, validate);
  construction->h =
    parse_positive_integer(parsed_line->tokens[5], line_number, validate);
  return true;
}

const char* construction_type(const struct Construction* construction) {
  switch (construction->type) {
    case BUILDING: return "building";
    case HOUSE:    return "house";
  }
  return "??";
}

bool are_intervals_overlapping(long long a1,
                               long long b1,
                               long long a2,
                               long long b2) {
  return a1 < b2 && a2 < b1;
}

bool are_constructions_overlapping(const struct Construction* construction1,
                                   const struct Construction* construction2) {
  long long x1 = construction1->x, y1 = construction1->y;
  long long w1 = construction1->w, h1 = construction1->h;
  long long x2 = construction2->x, y2 = construction2->y;
  long long w2 = construction2->w, h2 = construction2->h;
  return are_intervals_overlapping(x1 - w1, x1 + w1, x2 - w2, x2 + w2) &&
         are_intervals_overlapping(y1 - h1, y1 + h1, y2 - h2, y2 + h2);
}

char construction_quality(const struct Construction* construction,
                          const struct Antenna* antennas,
                          unsigned int num_antennas) {
  switch (num_corners_covered(construction, antennas, num_antennas)) {
    case 0: return 'E';
    case 1: return 'D';
    case 2: return 'C';
    case 3: return 'B';
    case 4: return 'A';
  }
  return '?';
}

void print_construction(const struct Construction* construction) {
  printf("  %s %s at %d %d with dimensions %d %d\n",
         construction_type(construction), construction->id, construction->x,
         construction->y, construction->w, construction->h);
}
