#include "scene.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing.h"
#include "validation.h"

// Private constants and functions
// ===============================

// The capacity of an array the first time it grows
#define INITIAL_CAPACITY 4

/**
 * Makes room for one more item in a dynamic array
 *
 * The capacity of the array is doubled when the array is full.
 *
 * @param items      The array (may be NULL)
 * @param count      The number of items in the array
 * @param capacity   The capacity of the array, updated if the array grows
 * @param item_size  The size of an item, in bytes
 * @param type       The type of the items, for error reporting
 * @param validate   Has the validate subcommand been invoked?
 * @return           The array, possibly moved
 */
static void* ensure_room(void* items,
                         unsigned int count,
                         unsigned int* capacity,
                         size_t item_size,
                         const char* type,
                         bool validate) {
  if (count < *capacity)
    return items;
  unsigned int new_capacity = *capacity == 0 ? INITIAL_CAPACITY : 2 * *capacity;
  void* new_items = realloc(items, new_capacity * item_size);
  if (new_items == NULL)
    report_error_allocation_failure(type, validate);
  *capacity = new_capacity;
  return new_items;
}

/**
 * Reads one line from the standard input, without its line terminator
 *
 * @param line         The buffer receiving the line
 * @param line_number  The number of the line, for error reporting
 * @param validate     Has the validate subcommand been invoked?
 * @return             true if and only if a line was read
 */
static bool read_line(char line[MAX_LINE_LENGTH + 2],
                      int line_number,
                      bool validate) {
  if (fgets(line, MAX_LINE_LENGTH + 2, stdin) == NULL)
    return false;
  size_t length = strcspn(line, "\r\n");
  if (line[length] == '\0' && length > MAX_LINE_LENGTH)
    report_error_line_too_long(line_number, validate);
  line[length] = '\0';
  return true;
}

/**
 * Loads a line that is neither the first nor the last one of a scene
 *
 * @param line         The line
 * @param line_number  The number of the line
 * @param scene        The scene in which the line is loaded
 * @param validate     Has the validate subcommand been invoked?
 */
static void load_object_line(const char* line,
                             int line_number,
                             struct Scene* scene,
                             bool validate) {
  struct ParsedLine parsed_line;
  struct Construction construction;
  struct Antenna antenna;
  parse_line(line, &parsed_line, line_number);
  if (parsed_line.num_tokens == 0)
    report_error_empty_line(line_number, validate);
  if (load_construction_from_parsed_line(&parsed_line, &construction, validate))
    add_construction(scene, &construction, validate);
  else if (load_antenna_from_parsed_line(&parsed_line, &antenna, validate))
    add_antenna(scene, &antenna, validate);
  else
    report_error_unrecognized_line(line_number, validate);
}

/**
 * Extends a bounding box so that it encloses a rectangle
 *
 * @param box    The bounding box
 * @param left   The smallest x coordinate of the rectangle
 * @param right  The largest x coordinate of the rectangle
 * @param down   The smallest y coordinate of the rectangle
 * @param up     The largest y coordinate of the rectangle
 */
static void extend_bounding_box(struct BoundingBox* box,
                                long long left,
                                long long right,
                                long long down,
                                long long up) {
  box->xmin = left < box->xmin ? left : box->xmin;
  box->xmax = right > box->xmax ? right : box->xmax;
  box->ymin = down < box->ymin ? down : box->ymin;
  box->ymax = up > box->ymax ? up : box->ymax;
}

// Public functions
// ================

// Construction and destruction
// ----------------------------

void initialize_empty_scene(struct Scene* scene) {
  scene->num_constructions = 0;
  scene->max_constructions = 0;
  scene->constructions = NULL;
  scene->num_antennas = 0;
  scene->max_antennas = 0;
  scene->antennas = NULL;
}

void free_scene(struct Scene* scene) {
  free(scene->constructions);
  free(scene->antennas);
  initialize_empty_scene(scene);
}

void load_scene_from_stdin(struct Scene* scene, bool validate) {
  initialize_empty_scene(scene);
  char line[MAX_LINE_LENGTH + 2];
  bool first_line = true, last_line = false;
  int line_number = 1;
  while (read_line(line, line_number, validate)) {
    last_line = false;
    if (first_line) {
      if (strcmp(line, "begin scene") != 0)
        report_error_scene_first_line(validate);
      first_line = false;
    } else if (strcmp(line, "end scene") == 0) {
      last_line = true;
    } else {
      load_object_line(line, line_number, scene, validate);
    }
    ++line_number;
  }
  if (!last_line)
    report_error_scene_last_line(validate);
}

// Modifiers
// ---------

void add_construction(struct Scene* scene,
                      const struct Construction* construction,
                      bool validate) {
  scene->constructions = ensure_room(scene->constructions,
                                     scene->num_constructions,
                                     &scene->max_constructions,
                                     sizeof(struct Construction),
                                     "construction", validate);
  unsigned int c = 0;
  while (c < scene->num_constructions &&
         strcmp(construction->id, scene->constructions[c].id) > 0)
    ++c;
  if (c < scene->num_constructions &&
      strcmp(construction->id, scene->constructions[c].id) == 0)
    report_error_non_unique_identifiers(construction_type(construction),
                                        construction->id, validate);
  memmove(scene->constructions + c + 1, scene->constructions + c,
          (scene->num_constructions - c) * sizeof(struct Construction));
  scene->constructions[c] = *construction;
  ++scene->num_constructions;
}

void add_antenna(struct Scene* scene,
                 const struct Antenna* antenna,
                 bool validate) {
  scene->antennas = ensure_room(scene->antennas, scene->num_antennas,
                                &scene->max_antennas, sizeof(struct Antenna),
                                "antenna", validate);
  unsigned int a = 0;
  while (a < scene->num_antennas &&
         strcmp(antenna->id, scene->antennas[a].id) > 0)
    ++a;
  if (a < scene->num_antennas &&
      strcmp(antenna->id, scene->antennas[a].id) == 0)
    report_error_non_unique_identifiers("antenna", antenna->id, validate);
  memmove(scene->antennas + a + 1, scene->antennas + a,
          (scene->num_antennas - a) * sizeof(struct Antenna));
  scene->antennas[a] = *antenna;
  ++scene->num_antennas;
}

// Validation
// ----------

void validate_scene(const struct Scene* scene, bool validate) {
  for (unsigned int c1 = 0; c1 < scene->num_constructions; ++c1)
    for (unsigned int c2 = c1 + 1; c2 < scene->num_constructions; ++c2) {
      const struct Construction* construction1 = scene->constructions + c1;
      const struct Construction* construction2 = scene->constructions + c2;
      if (are_constructions_overlapping(construction1, construction2))
        report_error_overlapping_objects(
          construction_type(construction1), construction1->id,
          construction_type(construction2), construction2->id, validate);
    }
  for (unsigned int a1 = 0; a1 < scene->num_antennas; ++a1)
    for (unsigned int a2 = a1 + 1; a2 < scene->num_antennas; ++a2) {
      const struct Antenna* antenna1 = scene->antennas + a1;
      const struct Antenna* antenna2 = scene->antennas + a2;
      if (have_antennas_same_position(antenna1, antenna2))
        report_error_same_position_antennas(antenna1->id, antenna2->id,
                                            validate);
    }
}

// Accessors
// ---------

bool scene_is_empty(const struct Scene* scene) {
  return scene->num_constructions == 0 && scene->num_antennas == 0;
}

int scene_num_buildings(const struct Scene* scene) {
  int num_buildings = 0;
  for (unsigned int c = 0; c < scene->num_constructions; ++c)
    num_buildings += scene->constructions[c].type == BUILDING;
  return num_buildings;
}

int scene_num_houses(const struct Scene* scene) {
  int num_houses = 0;
  for (unsigned int c = 0; c < scene->num_constructions; ++c)
    num_houses += scene->constructions[c].type == HOUSE;
  return num_houses;
}

bool scene_bounding_box(const struct Scene* scene, struct BoundingBox* box) {
  if (scene_is_empty(scene))
    return false;
  struct BoundingBox result = {LLONG_MAX, LLONG_MIN, LLONG_MAX, LLONG_MIN};
  for (unsigned int c = 0; c < scene->num_constructions; ++c) {
    const struct Construction* construction = scene->constructions + c;
    extend_bounding_box(&result,
                        (long long)construction->x - construction->w,
                        (long long)construction->x + construction->w,
                        (long long)construction->y - construction->h,
                        (long long)construction->y + construction->h);
  }
  for (unsigned int a = 0; a < scene->num_antennas; ++a) {
    const struct Antenna* antenna = scene->antennas + a;
    extend_bounding_box(&result,
                        (long long)antenna->x - antenna->r,
                        (long long)antenna->x + antenna->r,
                        (long long)antenna->y - antenna->r,
                        (long long)antenna->y + antenna->r);
  }
  *box = result;
  return true;
}

// Printing
// --------

void print_scene_quality(const struct Scene* scene) {
  if (scene->num_constructions == 0) {
    puts("Nothing to report, scene contains no construction");
    return;
  }
  for (unsigned int c = 0; c < scene->num_constructions; ++c) {
    const struct Construction* construction = scene->constructions + c;
    printf("%s %s: %c\n", construction_type(construction), construction->id,
           construction_quality(construction, scene->antennas,
                                scene->num_antennas));
  }
}

void print_scene_summary(const struct Scene* scene) {
  int num_buildings = scene_num_buildings(scene),
      num_houses = scene_num_houses(scene),
      num_antennas = scene->num_antennas;
  if (scene_is_empty(scene)) {
    puts("An empty scene");
    return;
  }
  printf("A scene with ");
  if (num_buildings > 0) {
    printf("%d building%s", num_buildings, num_buildings > 1 ? "s" : "");
    if (num_houses > 0 && num_antennas > 0)
      printf(", ");
    else if (num_houses > 0 || num_antennas > 0)
      printf(" and ");
  }
  if (num_houses > 0) {
    printf("%d house%s", num_houses, num_houses > 1 ? "s" : "");
    if (num_antennas > 0)
      printf(" and ");
  }
  if (num_antennas > 0)
    printf("%d antenna%s", num_antennas, num_antennas > 1 ? "s" : "");
  printf("\n");
}

void print_scene_constructions(const struct Scene* scene) {
  for (unsigned int c = 0; c < scene->num_constructions; ++c)
    print_construction(scene->constructions + c);
}

void print_scene_antennas(const struct Scene* scene) {
  for (unsigned int a = 0; a < scene->num_antennas; ++a)
    print_antenna(scene->antennas + a);
}

void print_scene_bounding_box(const struct Scene* scene) {
  struct BoundingBox box;
  if (!scene_bounding_box(scene, &box)) {
    printf("undefined (empty scene)\n");
    return;
  }
  printf("bounding box [%lld, %lld] x [%lld, %lld]\n", box.xmin, box.xmax,
         box.ymin, box.ymax);
}
