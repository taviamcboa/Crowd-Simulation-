/* libvoronoi -- Library to calculate discrete Voronoi diagrams and properties.
 * Copyright (C) 2002, 2003 Adrian Secord.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file   voronoi.h
    \brief  Declarations for Voronoi functions.
 **/

#ifndef AJS_VORONOI_H
#define AJS_VORONOI_H

#include "config.h"
#ifdef _WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

// You need to adjust the location of these header files according to your configuration
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "indexer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** State of Voronoi calculation. */
typedef struct {

    /** Indexer state */
    vor_index_statep indexer;

    /** Display list for drawing the cones */
    GLuint cone_list;

    /** Number of cone slices */
    unsigned num_cone_slices;

    /** Size of the current drawing area */
    unsigned device_width, device_height;

    /** Current magnification */
    unsigned mag_factor;

    /** Radius of the cone to draw */
    double radius;

    /** Flag indicating that the graphics have been initialised. */
    int graphics_initialised;


    /** Number of points */
    unsigned num_points;

    /** Point vertices */
    const float* point_vertices;

    /** Stride, in bytes, between vertices. */
    int point_stride;


    /** Number of lines */
    unsigned num_lines;

    /** Line vertices */
    const float* line_vertices;

    /** Stride, in bytes, between lines. */
    int line_stride;


    /** Number of line strips */
    unsigned num_line_strips;

    /** Line strip vertices. */
    const float** line_strip_vertices;

    /** Number of vertices per strip */
    const unsigned* line_strip_vertex_counts;

    /** Stride, in bytes, between line strip vertices. */
    const unsigned* line_strip_strides;


    /** Image of the Voronoi diagram. */
    GLubyte* col_diagram;

    /** Depth image of the Voronoi diagram. */
    GLfloat* depth_diagram;

    /** Image of indices of the Voronoi diagram. */
    unsigned* index_diagram;

    /** Density function. */
    unsigned* density;

    /** First integral of the density function. */
    unsigned* density_int1;

    /** Second integral of the density function. */
    unsigned* density_int2;

    /** Third integral of the density function. */
    unsigned* density_int3;

    /** Highest index encoded on last call to vor_calc_diagram. */
    unsigned highest_encoded;


    /** Debug trace level. */
    int trace;

} vor_state;

typedef vor_state* vor_statep;
typedef vor_statep* vor_statepp;


/** Geometric information for a single region.
 */
typedef struct {
    unsigned area;                  /**< Area */
    unsigned density;               /**< Integrated density */
    float centroid_x, centroid_y;   /**< Centroid position */
    float orient;                   /**< Orientation of equivalent rectangle */
    float roundness;                /**< Ratio of width/length of equiv rect */
    int valid;                      /**< Whether derived quantities are valid */
} vor_region_info;

typedef vor_region_info* vor_region_infop;
typedef vor_region_infop* vor_region_infopp;


/** Create the state structure. */
vor_statep vor_create_state(void);

/** Destroy the state structure. */
void vor_destroy_state(vor_statepp statep);

/** Initialise values from graphics engine. */
void vor_init_graphics(vor_statep state);

/** Return whether the graphics have been initialised. */
int vor_get_graphics_init(const vor_statep state);

/** Get the various bit depths of each channel. */
void vor_get_bit_depths(const vor_statep state, int* r, int* g, int* b, int* a);

/** Set the value of the trace flag. */
void vor_set_trace(vor_statep state, int value);

/** Get the value of the trace flag. */
int vor_get_trace(const vor_statep state);

/** Set the device size. */
void vor_set_device_size(vor_statep state, 
                         const unsigned width, const unsigned height);

/** Get the device size. */
void vor_get_device_size(const vor_statep state,
                         unsigned* width, unsigned* height);

/** Get the magnification factor */
unsigned vor_get_mag_factor(const vor_statep state);

/** Set the magnification factor */
void vor_set_mag_factor(vor_statep state, const unsigned value);

/** Set the 2D point vertex data.  
 *  \a num is the total number of points in the array.  
 *  \a stride is the distance in bytes between consecutive points.  
 *  A value of zero means that the data is tightly packed, equivalent
 *  to a stride of <tt>2*sizeof(float)</tt>.  
 *  \a data is the array of data.
 *  The total size of the array is therefore \a num * \a stride.
 *  Note that the data is not copied, so changes to the data contents do not
 *  need another call.
 */
void vor_set_points(vor_statep state, const unsigned num,
                            const int stride, const float* data);

/** Set the 2D line vertex data.  
 *  \a num is the total number of lines in the array.  
 *  \a stride is the distance in bytes between consecutive lines.  
 *  A value of zero means that the data is tightly packed, equivalent
 *  to a stride of <tt>4*sizeof(float)</tt>.  
 *  \a data is the array of data.
 *  The total size of the array is therefore \a num * \a stride.
 *  Note that the data is not copied, so changes to the data contents do not
 *  need another call.
 */
void vor_set_lines(vor_statep state, const unsigned num,
                           const int stride, const float* data);

/** Set the 2D line strip vertex data.  
 *  \a num_strips is the total number of line strips in the array.  
 *  \a vertex_count[i] is the number of vertices per line strip.
 *  \a stride[i] is the distance, in bytes, between successive sets of 
 *  vertices.  A value of zero means that the data is tightly packed, 
 *  equivalent to a stride of <tt>vertex_count[i]* 2 * sizeof(float)</tt>.  
 *  \a data[i] is the vertex data for strip i.  If \a stride is NULL, then
 *  all data is assumed to be tightly packed.
 *  Each of \a vertex_count, \a stride, and \a data should be of size 
 *  \a num_strips.  Each data[i] should be of size \a vertex_count[i] * 
 *  \a stride[i].
 *  Note that the data is not copied, so changes to the data contents do not
 *  need another call.
 */
void vor_set_line_strips(vor_statep state, const unsigned num_strips,
                                 const unsigned* vertex_count,
                                 const unsigned* stride,
                                 const float** data);

/** Calculate the Voronoi diagram with the pre-set vertices. */
void vor_calc_diagram(const vor_statep state);

void vor_calc_diagram_Render(const vor_statep state);

void vor_calc_diagram_PostProcessing(const vor_statep state);

/** Calculate quantities of interest for each region.  
 *  \a info should be of size vor_get_highest_encoded_index().
 */
void vor_calc_region_quantities(const vor_statep state, vor_region_infop info);

/** Get the maximum number of indices supported by these bit depths. */
unsigned vor_get_max_num_indices(const vor_statep state);

/** Get the colour diagram buffer.  The size is 4 * width * height bytes, 
 *  where width and height are from get/set_device_size.
 */
const GLubyte* vor_get_col_diagram(const vor_statep state);

/** Get the depth diagram buffer.  The size is width * height * sizeof(float)
 *  bytes.
 */
const GLfloat* vor_get_depth_diagram(const vor_statep state);

/** Get the index diagram buffer. The size is width * height * sizeof(unsigned)
 *  bytes, where width and height are from get/set_device_size().
 */
const unsigned* vor_get_index_diagram(const vor_statep state);

/** Set the density function.
 *  The data buffer size is width * height * sizeof(unsigned) bytes.
 *  The values are scaled between zero and UINT_MAX.
 *  The data is copied to internal buffers, so changes to the original 
 *  function must be followed by another call.
 */
void vor_set_density_function(const vor_statep state, 
                              const unsigned width, const unsigned height, 
                              const unsigned* data);

/** Clear the density function. */
void vor_clear_density_function(vor_statep state);

/** Get the highest encoded index used the last time vor_calc_diagram
 *  was called. 
 */
unsigned vor_get_highest_encoded_index(const vor_statep state);

#ifdef __cplusplus
}
#endif

#endif
