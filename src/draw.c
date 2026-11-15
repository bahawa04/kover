#include "draw.h"

#include <cairo-svg.h>
#include <cairo.h>
#include <math.h>
#include <stdio.h>

// Constants
// ---------

// The size in pixels of the largest side of the frame
#define MAX_FRAME_SIZE 700.0
// The space in pixels between the frame and the border of the image
#define MARGIN 60.0
// The size in pixels of the text
#define FONT_SIZE 14.0
// The minimum space in pixels between two graduations
#define MIN_TICK_SPACING 40.0
// The side of the frame, in scene units, when the scene is empty
#define EMPTY_SCENE_SIZE 10
// The ratio of the circumference of a circle to its diameter
#define PI 3.14159265358979323846
// The radius in pixels of the dot at the center of an antenna
#define ANTENNA_DOT_RADIUS 3.0

// Types
// -----

// How the scene is mapped to the image
struct Layout {
  // The part of the plane that is drawn
  struct BoundingBox box;
  // The number of pixels per unit
  double scale;
  // The width of the frame in pixels
  double width;
  // The height of the frame in pixels
  double height;
};

// A color with an opacity
struct Color {
  double red;
  double green;
  double blue;
  double alpha;
};

static const struct Color BLACK = {0.0, 0.0, 0.0, 1.0};
static const struct Color GRID_GRAY = {0.0, 0.0, 0.0, 0.3};
static const struct Color BUILDING_FILL = {0.8, 0.85, 1.0, 1.0};
static const struct Color BUILDING_STROKE = {0.0, 0.0, 1.0, 1.0};
static const struct Color HOUSE_FILL = {0.93, 0.8, 0.93, 1.0};
static const struct Color HOUSE_STROKE = {0.6, 0.0, 0.6, 1.0};
static const struct Color ANTENNA_FILL = {0.0, 0.6, 0.0, 0.2};
static const struct Color ANTENNA_STROKE = {0.0, 0.6, 0.0, 1.0};

// Layout
// ------

/**
 * Computes how a scene is mapped to the image
 *
 * @param scene  The scene
 * @return       The layout
 */
static struct Layout compute_layout(const struct Scene* scene) {
  struct Layout layout;
  if (!scene_bounding_box(scene, &layout.box)) {
    layout.box.xmin = layout.box.ymin = 0;
    layout.box.xmax = layout.box.ymax = EMPTY_SCENE_SIZE;
  }
  double width = (double)(layout.box.xmax - layout.box.xmin);
  double height = (double)(layout.box.ymax - layout.box.ymin);
  layout.scale = MAX_FRAME_SIZE / (width > height ? width : height);
  layout.width = width * layout.scale;
  layout.height = height * layout.scale;
  return layout;
}

/**
 * Converts an x coordinate of the scene to a position in the image
 */
static double to_image_x(const struct Layout* layout, double x) {
  return MARGIN + (x - (double)layout->box.xmin) * layout->scale;
}

/**
 * Converts a y coordinate of the scene to a position in the image
 *
 * The y axis of the scene points upwards, whereas the one of the image points
 * downwards.
 */
static double to_image_y(const struct Layout* layout, double y) {
  return MARGIN + ((double)layout->box.ymax - y) * layout->scale;
}

/**
 * Returns the distance between two graduations, in scene units
 *
 * The distance is of the form 1, 2 or 5 times a power of 10, and is large
 * enough for the graduations not to overlap.
 *
 * @param scale  The number of pixels per unit
 * @return       The distance
 */
static long long tick_step(double scale) {
  static const int multipliers[] = {1, 2, 5};
  long long power = 1;
  int m = 0;
  while (multipliers[m] * power * scale < MIN_TICK_SPACING) {
    if (++m == 3) {
      m = 0;
      power *= 10;
    }
  }
  return multipliers[m] * power;
}

/**
 * Returns the smallest multiple of step that is not smaller than start
 */
static long long first_tick(long long start, long long step) {
  long long remainder = ((start % step) + step) % step;
  return remainder == 0 ? start : start + (step - remainder);
}

// Drawing primitives
// ------------------

static void set_color(cairo_t* cr, const struct Color* color) {
  cairo_set_source_rgba(cr, color->red, color->green, color->blue,
                        color->alpha);
}

/**
 * Writes a text centered on a position
 */
static void show_centered_text(cairo_t* cr,
                               const char* text,
                               double x,
                               double y) {
  cairo_text_extents_t extents;
  cairo_text_extents(cr, text, &extents);
  cairo_move_to(cr, x - (extents.width / 2 + extents.x_bearing),
                y - (extents.height / 2 + extents.y_bearing));
  cairo_show_text(cr, text);
  cairo_new_path(cr);
}

/**
 * Draws a rectangle with a fill color and a stroke color
 */
static void draw_rectangle(cairo_t* cr,
                           double x,
                           double y,
                           double width,
                           double height,
                           const struct Color* fill,
                           const struct Color* stroke) {
  cairo_rectangle(cr, x, y, width, height);
  set_color(cr, fill);
  cairo_fill_preserve(cr);
  set_color(cr, stroke);
  cairo_set_line_width(cr, 2.0);
  cairo_stroke(cr);
}

// Scene elements
// --------------

static void draw_grid(cairo_t* cr, const struct Layout* layout) {
  long long step = tick_step(layout->scale);
  static const double dashes[] = {4.0, 4.0};
  set_color(cr, &GRID_GRAY);
  cairo_set_line_width(cr, 1.0);
  cairo_set_dash(cr, dashes, 2, 0);
  for (long long x = first_tick(layout->box.xmin, step); x <= layout->box.xmax;
       x += step) {
    cairo_move_to(cr, to_image_x(layout, (double)x), MARGIN);
    cairo_line_to(cr, to_image_x(layout, (double)x), MARGIN + layout->height);
  }
  for (long long y = first_tick(layout->box.ymin, step); y <= layout->box.ymax;
       y += step) {
    cairo_move_to(cr, MARGIN, to_image_y(layout, (double)y));
    cairo_line_to(cr, MARGIN + layout->width, to_image_y(layout, (double)y));
  }
  cairo_stroke(cr);
  cairo_set_dash(cr, NULL, 0, 0);
}

static void draw_graduations(cairo_t* cr, const struct Layout* layout) {
  long long step = tick_step(layout->scale);
  char label[32];
  const double gap = 18.0;
  set_color(cr, &BLACK);
  for (long long x = first_tick(layout->box.xmin, step); x <= layout->box.xmax;
       x += step) {
    double px = to_image_x(layout, (double)x);
    snprintf(label, sizeof(label), "%lld", x);
    show_centered_text(cr, label, px, MARGIN - gap);
    show_centered_text(cr, label, px, MARGIN + layout->height + gap);
  }
  for (long long y = first_tick(layout->box.ymin, step); y <= layout->box.ymax;
       y += step) {
    double py = to_image_y(layout, (double)y);
    snprintf(label, sizeof(label), "%lld", y);
    show_centered_text(cr, label, MARGIN - 2 * gap, py);
    show_centered_text(cr, label, MARGIN + layout->width + 2 * gap, py);
  }
}

static void draw_frame(cairo_t* cr, const struct Layout* layout) {
  set_color(cr, &BLACK);
  cairo_set_line_width(cr, 2.0);
  cairo_rectangle(cr, MARGIN, MARGIN, layout->width, layout->height);
  cairo_stroke(cr);
}

static void draw_constructions(cairo_t* cr,
                               const struct Layout* layout,
                               const struct Scene* scene) {
  for (unsigned int i = 0; i < scene->num_constructions; ++i) {
    const struct Construction* c = scene->constructions + i;
    bool is_building = c->type == BUILDING;
    double left = to_image_x(layout, (double)c->x - c->w);
    double top = to_image_y(layout, (double)c->y + c->h);
    draw_rectangle(cr, left, top, 2.0 * c->w * layout->scale,
                   2.0 * c->h * layout->scale,
                   is_building ? &BUILDING_FILL : &HOUSE_FILL,
                   is_building ? &BUILDING_STROKE : &HOUSE_STROKE);
    set_color(cr, &BLACK);
    show_centered_text(cr, c->id, to_image_x(layout, c->x),
                       to_image_y(layout, c->y));
  }
}

static void draw_antennas(cairo_t* cr,
                          const struct Layout* layout,
                          const struct Scene* scene) {
  for (unsigned int i = 0; i < scene->num_antennas; ++i) {
    const struct Antenna* a = scene->antennas + i;
    double x = to_image_x(layout, a->x), y = to_image_y(layout, a->y);
    cairo_new_path(cr);
    cairo_arc(cr, x, y, a->r * layout->scale, 0, 2 * PI);
    set_color(cr, &ANTENNA_FILL);
    cairo_fill_preserve(cr);
    set_color(cr, &ANTENNA_STROKE);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);
    cairo_arc(cr, x, y, ANTENNA_DOT_RADIUS, 0, 2 * PI);
    cairo_fill(cr);
    show_centered_text(cr, a->id, x + 3 * ANTENNA_DOT_RADIUS,
                       y - 3 * ANTENNA_DOT_RADIUS);
  }
}

// Public function
// ===============

bool draw_scene_to_svg(const char* filename, const struct Scene* scene) {
  struct Layout layout = compute_layout(scene);
  cairo_surface_t* surface = cairo_svg_surface_create(
    filename, layout.width + 2 * MARGIN, layout.height + 2 * MARGIN);
  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
    cairo_surface_destroy(surface);
    return false;
  }
  cairo_t* cr = cairo_create(surface);
  cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, FONT_SIZE);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  draw_grid(cr, &layout);
  draw_constructions(cr, &layout, scene);
  draw_antennas(cr, &layout, scene);
  draw_frame(cr, &layout);
  draw_graduations(cr, &layout);
  cairo_destroy(cr);
  cairo_surface_finish(surface);
  bool success = cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS;
  cairo_surface_destroy(surface);
  // Releases the caches kept by cairo (fonts), so that memory checkers do not
  // report them as leaks
  cairo_debug_reset_static_data();
  return success;
}
