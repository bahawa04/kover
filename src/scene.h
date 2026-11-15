#ifndef SCENE_H_
#define SCENE_H_

#include <stdbool.h>

#include "antenna.h"
#include "construction.h"

// Types
// -----

// A scene, whose constructions and antennas are stored in dynamic arrays
struct Scene {
  // The number of constructions in the scene
  unsigned int num_constructions;
  // The capacity of the array of constructions
  unsigned int max_constructions;
  // The constructions of the scene, sorted by identifier
  struct Construction* constructions;
  // The number of antennas in the scene
  unsigned int num_antennas;
  // The capacity of the array of antennas
  unsigned int max_antennas;
  // The antennas of the scene, sorted by identifier
  struct Antenna* antennas;
};

// A rectangle enclosing all the objects of a scene
struct BoundingBox {
  // The smallest x coordinate
  long long xmin;
  // The largest x coordinate
  long long xmax;
  // The smallest y coordinate
  long long ymin;
  // The largest y coordinate
  long long ymax;
};

// Construction and destruction
// ----------------------------

/**
 * Initializes an empty scene
 *
 * @param scene  The scene to initialize
 */
void initialize_empty_scene(struct Scene* scene);

/**
 * Frees the memory used by a scene
 *
 * The scene is left empty, so that it can be freed or reused safely.
 *
 * @param scene  The scene to free
 */
void free_scene(struct Scene* scene);

/**
 * Loads a scene from the standard input
 *
 * If the stream is not a valid scene, an error is reported and the program
 * exits with 1.
 *
 * @param scene     The resulting scene
 * @param validate  Has the subcommand validate been invoked?
 */
void load_scene_from_stdin(struct Scene* scene, bool validate);

// Modifiers
// ---------

/**
 * Adds a construction to a scene
 *
 * The array of constructions grows when needed. If the identifier is already
 * used by another construction, an error is reported and the program exits.
 *
 * @param scene         The scene to which the construction is added
 * @param construction  The construction to add
 * @param validate      Has the validate subcommand been invoked?
 */
void add_construction(struct Scene* scene,
                      const struct Construction* construction,
                      bool validate);

/**
 * Adds an antenna to a scene
 *
 * The array of antennas grows when needed. If the identifier is already used
 * by another antenna, an error is reported and the program exits.
 *
 * @param scene     The scene to which the antenna is added
 * @param antenna   The antenna to add
 * @param validate  Has the validate subcommand been invoked?
 */
void add_antenna(struct Scene* scene,
                 const struct Antenna* antenna,
                 bool validate);

// Validation
// ----------

/**
 * Checks if a scene is valid
 *
 * If the scene is invalid, an error is reported and the program exits with 1.
 *
 * @param scene     The scene to validate
 * @param validate  Has the validate subcommand been invoked?
 */
void validate_scene(const struct Scene* scene, bool validate);

// Accessors
// ---------

/**
 * Indicates if a scene is empty
 *
 * @param scene  The scene to check
 * @return       true if and only if the given scene is empty
 */
bool scene_is_empty(const struct Scene* scene);

/**
 * Returns the number of buildings in a scene
 *
 * @param scene  The scene
 * @return       The number of buildings
 */
int scene_num_buildings(const struct Scene* scene);

/**
 * Returns the number of houses in a scene
 *
 * @param scene  The scene
 * @return       The number of houses
 */
int scene_num_houses(const struct Scene* scene);

/**
 * Computes the bounding box of a scene
 *
 * The bounding box encloses every construction and the disk of every
 * antenna.
 *
 * @param scene  The scene
 * @param box    The resulting bounding box (unchanged if the scene is empty)
 * @return       true if and only if the scene is not empty
 */
bool scene_bounding_box(const struct Scene* scene, struct BoundingBox* box);

// Printing
// --------

/**
 * Prints the quality of the scene to stdout
 *
 * @param scene  The scene whose quality is printed
 */
void print_scene_quality(const struct Scene* scene);

/**
 * Prints a summary of the scene on stdout
 *
 * @param scene  The scene to print
 */
void print_scene_summary(const struct Scene* scene);

/**
 * Prints the constructions of the scene to stdout
 *
 * @param scene  The scene whose constructions are printed
 */
void print_scene_constructions(const struct Scene* scene);

/**
 * Prints the antennas of the scene to stdout
 *
 * @param scene  The scene whose antennas are printed
 */
void print_scene_antennas(const struct Scene* scene);

/**
 * Prints the bounding box of the scene to stdout
 *
 * @param scene  The scene whose bounding box is printed
 */
void print_scene_bounding_box(const struct Scene* scene);

#endif
