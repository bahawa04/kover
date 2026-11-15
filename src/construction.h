#ifndef CONSTRUCTION_H_
#define CONSTRUCTION_H_

#include <stdbool.h>

#include "antenna.h"
#include "parsing.h"
#include "validation.h"

// Types
// -----

// A type of construction
enum ConstructionType {
  // A building
  BUILDING,
  // A house
  HOUSE
};

// A construction
struct Construction {
  // The identifier of the construction
  char id[MAX_LENGTH_ID + 1];
  // The type of the construction
  enum ConstructionType type;
  // The x coordinate of the construction
  int x;
  // The y coordinate of the construction
  int y;
  // The half width of the construction (half length in x direction)
  int w;
  // The half height of the construction (half length in y direction)
  int h;
};

// Loading
// -------

/**
 * Loads a construction from a parsed line
 *
 * If the line starts neither with "building" nor with "house", nothing is
 * loaded. Otherwise, the line is validated and an error is reported (and the
 * program exits) if it is not a well-formed construction line.
 *
 * @param parsed_line   The parsed line
 * @param construction  The resulting construction
 * @param validate      Has the validate subcommand been used?
 * @return              true if and only if the line describes a construction
 */
bool load_construction_from_parsed_line(const struct ParsedLine* parsed_line,
                                        struct Construction* construction,
                                        bool validate);

// Accessors
// ---------

/**
 * Returns the type of construction as a string
 *
 * @param construction  The construction
 * @return              The type
 */
const char* construction_type(const struct Construction* construction);

/**
 * Indicates if two open intervals overlap
 *
 * The intervals (a1, b1) and (a2, b2) overlap if their intersection has a
 * strictly positive length. In particular, an interval that contains the
 * other one overlaps it, and intervals that only share an endpoint do not
 * overlap.
 *
 * @param a1  The start of the first interval
 * @param b1  The end of the first interval
 * @param a2  The start of the second interval
 * @param b2  The end of the second interval
 * @return    true if and only if the intervals overlap
 */
bool are_intervals_overlapping(long long a1,
                               long long b1,
                               long long a2,
                               long long b2);

/**
 * Indicates if two constructions are overlapping
 *
 * Two constructions are overlapping if their intersection has a strictly
 * positive area.
 *
 * @param construction1  The first construction
 * @param construction2  The second construction
 * @return               true if and only if the constructions overlap
 */
bool are_constructions_overlapping(const struct Construction* construction1,
                                   const struct Construction* construction2);

/**
 * Returns the quality of the coverage of a construction
 *
 * The quality is 'A' if the four corners of the construction are covered by
 * at least one antenna, 'B' if exactly three are, 'C' if exactly two are, 'D'
 * if exactly one is and 'E' if none is.
 *
 * @param construction  The construction
 * @param antennas      The antennas
 * @param num_antennas  The number of antennas
 * @return              The quality
 */
char construction_quality(const struct Construction* construction,
                          const struct Antenna* antennas,
                          unsigned int num_antennas);

/**
 * Prints a construction on stdout
 *
 * @param construction  The construction to print
 */
void print_construction(const struct Construction* construction);

#endif
