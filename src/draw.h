#ifndef DRAW_H_
#define DRAW_H_

#include <stdbool.h>

#include "scene.h"

/**
 * Draws a scene in a SVG file
 *
 * The image contains a frame enclosing the whole scene, a dotted grid,
 * graduations on the four sides of the frame, the constructions (blue for
 * buildings, purple for houses) and the antennas (green disks), each with its
 * identifier. The y axis points upwards. An empty scene is drawn as an empty
 * frame.
 *
 * @param filename  The name of the SVG file to write
 * @param scene     The scene to draw
 * @return          true if and only if the file was written successfully
 */
bool draw_scene_to_svg(const char* filename, const struct Scene* scene);

#endif
