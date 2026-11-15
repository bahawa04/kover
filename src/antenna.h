#ifndef ANTENNA_H_
#define ANTENNA_H_

#include <stdbool.h>

#include "parsing.h"
#include "validation.h"

// Types
// -----

// An antenna
struct Antenna {
  // The identifier of the antenna
  char id[MAX_LENGTH_ID + 1];
  // The x coordinate of the antenna
  int x;
  // The y coordinate of the antenna
  int y;
  // The radius of the antenna
  int r;
};

// Loading
// -------

/**
 * Loads an antenna from a parsed line
 *
 * If the line does not start with "antenna", nothing is loaded. Otherwise,
 * the line is validated and an error is reported (and the program exits) if
 * it is not a well-formed antenna line.
 *
 * @param parsed_line  The parsed line
 * @param antenna      The resulting antenna
 * @param validate     Has the validate subcommand been used?
 * @return             true if and only if the line describes an antenna
 */
bool load_antenna_from_parsed_line(const struct ParsedLine* parsed_line,
                                   struct Antenna* antenna,
                                   bool validate);

// Accessors
// ---------

/**
 * Indicates if two antennas have the same position
 *
 * @param antenna1  The first antenna
 * @param antenna2  The second antenna
 * @return          true if and only if the positions are equal
 */
bool have_antennas_same_position(const struct Antenna* antenna1,
                                 const struct Antenna* antenna2);

/**
 * Indicates if a point is covered by an antenna
 *
 * A point is covered if it lies in the disk (borders included) determined by
 * the position and the radius of the antenna.
 *
 * @param antenna  The antenna
 * @param x        The x coordinate of the point
 * @param y        The y coordinate of the point
 * @return         true if and only if the point is covered
 */
bool antenna_covers_point(const struct Antenna* antenna,
                          long long x,
                          long long y);

/**
 * Prints an antenna on stdout
 *
 * @param antenna  The antenna to print
 */
void print_antenna(const struct Antenna* antenna);

#endif
