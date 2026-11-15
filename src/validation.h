#ifndef VALIDATION_H_
#define VALIDATION_H_

#include <stdbool.h>

// Constants
// ---------

// The maximum length of an identifier
#define MAX_LENGTH_ID 10

// Validations
// -----------

/**
 * Indicates if a string is a valid identifier
 *
 * An identifier is valid if it completely matches the ERE
 *
 *   [a-zA-Z_][a-zA-Z0-9_]*
 *
 * and if it has at most MAX_LENGTH_ID characters.
 *
 * @param s  The string to verify
 * @return   true if and only if the string is a valid identifier
 */
bool is_valid_id(const char* s);

/**
 * Indicates if a string is a valid integer
 *
 * A string is a valid integer if it completely matches the ERE
 *
 *   0|([-]?[1-9][0-9]*)
 *
 * @param s  The string to verify
 * @return   true if and only if the string is a valid integer
 */
bool is_valid_integer(const char* s);

/**
 * Indicates if a string is a valid positive integer
 *
 * A string is a valid positive integer if it completely matches the ERE
 *
 *   [1-9][0-9]*
 *
 * @param s  The string to verify
 * @return   true if and only if the string is a valid positive integer
 */
bool is_valid_positive_integer(const char* s);

// Error reporting
// ---------------
//
// Every function below prints a message on stderr and exits with status 1.
// When the validate subcommand is used, "not ok" is first printed on stdout.

/**
 * Reports that a given object identifier is not unique
 *
 * @param object    The object
 * @param id        The identifier
 * @param validate  Has the validate subcommand been used?
 */
void report_error_non_unique_identifiers(const char* object,
                                         const char* id,
                                         bool validate);

/**
 * Reports that an identifier is invalid
 *
 * @param id           The identifier
 * @param line_number  The line number
 * @param validate     Has the validate subcommand been used?
 */
void report_error_invalid_identifier(const char* id,
                                     int line_number,
                                     bool validate);

/**
 * Reports that a string is not a valid integer (or does not fit in an int)
 *
 * @param s            The string
 * @param line_number  The line number
 * @param validate     Has the validate subcommand been used?
 */
void report_error_invalid_int(const char* s, int line_number, bool validate);

/**
 * Reports that a string is not a valid positive integer
 *
 * @param s            The string
 * @param line_number  The line number
 * @param validate     Has the validate subcommand been used?
 */
void report_error_invalid_positive_int(const char* s,
                                       int line_number,
                                       bool validate);

/**
 * Reports that the first line of a scene is invalid
 *
 * @param validate  Has the validate subcommand been used?
 */
void report_error_scene_first_line(bool validate);

/**
 * Reports that the last line of a scene is invalid
 *
 * @param validate  Has the validate subcommand been used?
 */
void report_error_scene_last_line(bool validate);

/**
 * Reports that a scene line is not recognized
 *
 * @param line_number  The number of the unrecognized line
 * @param validate     Has the validate subcommand been used?
 */
void report_error_unrecognized_line(int line_number, bool validate);

/**
 * Reports that a scene line contains nothing but blanks
 *
 * @param line_number  The number of the empty line
 * @param validate     Has the validate subcommand been used?
 */
void report_error_empty_line(int line_number, bool validate);

/**
 * Reports that a scene line is too long
 *
 * @param line_number  The number of the line
 * @param validate     Has the validate subcommand been used?
 */
void report_error_line_too_long(int line_number, bool validate);

/**
 * Reports that an object line has the wrong number of arguments
 *
 * @param object       The object on the line
 * @param line_number  The number of the line
 * @param validate     Has the validate subcommand been used?
 */
void report_error_line_wrong_arguments_number(const char* object,
                                              int line_number,
                                              bool validate);

/**
 * Reports that two objects are overlapping
 *
 * @param object1   The type of the first object
 * @param id1       The identifier of the first object
 * @param object2   The type of the second object
 * @param id2       The identifier of the second object
 * @param validate  Has the validate subcommand been used?
 */
void report_error_overlapping_objects(const char* object1,
                                      const char* id1,
                                      const char* object2,
                                      const char* id2,
                                      bool validate);

/**
 * Reports that two antennas have the same position
 *
 * @param id1       The identifier of the first antenna
 * @param id2       The identifier of the second antenna
 * @param validate  Has the validate subcommand been used?
 */
void report_error_same_position_antennas(const char* id1,
                                         const char* id2,
                                         bool validate);

/**
 * Reports that a memory allocation failed
 *
 * @param type      The type of object being allocated
 * @param validate  Has the validate subcommand been used?
 */
void report_error_allocation_failure(const char* type, bool validate);

/**
 * Reports that the subcommand is mandatory
 */
void report_error_mandatory_subcommand(void);

/**
 * Reports that the subcommand is unrecognized
 *
 * @param subcommand  The unrecognized subcommand
 */
void report_error_unrecognized_subcommand(const char* subcommand);

/**
 * Reports that the draw subcommand has no output file
 */
void report_error_missing_output_file(void);

/**
 * Reports that the output file of the draw subcommand cannot be written
 *
 * @param filename  The name of the output file
 */
void report_error_cannot_write_file(const char* filename);

#endif
