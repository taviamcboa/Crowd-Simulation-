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

/** \file   indexer.h
 *  \brief  Declarations for the colour indexer stuctures and functions.
 */

#ifndef AJS_INDEXER_H
#define AJS_INDEXER_H

#include "config.h"
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

// You need to adjust the location of these header files according to your configuration
#include <GL/glew.h>
#include <GL/freeglut.h>

#ifdef __cplusplus
extern "C" {
#endif

/** State information for the indexer functions. */ 
typedef struct {
    /** Bit depths of each colour channel. */
    int r_bits, g_bits, b_bits, a_bits;

    /** 8 - bit depth for each colour channel. */
    int r_ibits, g_ibits, b_ibits, a_ibits;

    /** Maximum values for each colour channel. */
    unsigned r_max, g_max, b_max, a_max;

    unsigned r_shift, g_shift, b_shift;

    unsigned num_spare_indices; /**< Indices to reserve for non-colour use   */
    unsigned first_spare_index; /**< First of the spare indices              */
    unsigned null_index;        /**< Null index: "that which cannot be used" */
    unsigned max_index;         /**< Max index which we can work with        */

} vor_index_state;

/** Pointer to vor_index_state structure */
typedef vor_index_state* vor_index_statep;

/** Pointer to pointer to vor_index_state_structure */
typedef vor_index_statep* vor_index_statepp;


/** Create a new vor_index_state structure, which must be destroyed 
 *  with vor_index_destroy_state.  
 */
vor_index_statep vor_index_create_state(void);

/** Destroy a vor_index_state structure created with vor_index_create_state. */
void vor_index_destroy_state(vor_index_statepp statep);

/** Set the bit depths of each channel. */
void vor_index_set_bit_depths(vor_index_statep state, 
                              const int r, const int g, 
                              const int b, const int a);

/** Get the bit depths of each channel. */
void vor_index_get_bit_depths(const vor_index_statep state, 
                              int* r, int* g, int* b, int* a);

/** Get the maximum number of indices supported by these bit depths */
unsigned vor_index_get_max_num(const vor_index_statep state);

/** Get the maximum index */
unsigned vor_index_get_max(const vor_index_statep state);

/** Get the null index */
unsigned vor_index_get_null(const vor_index_statep state);


/** Decode colour as an index. */
unsigned vor_index_decode_4ub(const vor_index_statep state, 
                              const GLubyte r, const GLubyte g, const GLubyte b,
                              const GLubyte a);

/** Decode a block of colours to indices, effectively calling decode_4ub
 *  on each element of col_data and storing the result into index_data.
 */
void vor_index_block_decode_4ub(const vor_index_statep state,
                                const unsigned num_entries,
                                const GLubyte* col_data,
                                const unsigned max_expected_index,
                                unsigned* index_data,
                                unsigned* index_exceeded_count);

/** Decode colour as an index. */
unsigned vor_index_decode_4f(const vor_index_statep state, 
                             const float r, const float g, const float b, 
                             const float a);

/** Decode colour, represented as an array of 3 GLubytes, as an index. */
unsigned vor_index_decode_3ubv(const vor_index_statep state, const GLubyte* c);

/** Decode colour, represented as an array of 4 GLubytes, as an index. */
unsigned vor_index_decode_4ubv(const vor_index_statep state, const GLubyte* c);

/** Decode colour, represented as an array of 3 floats, as an index. */
unsigned vor_index_decode_3fv(const vor_index_statep state, const float* c);

/** Decode colour, represented as an array of 4 floats, as an index. */
unsigned vor_index_decode_4fv(const vor_index_statep state, const float* c);



/** Encode index to colour. */
void vor_index_encode_4ub(const vor_index_statep state, const unsigned index, 
                          GLubyte* r, GLubyte* g, GLubyte* b, GLubyte* a);

/* Encode a block of indices as colours. */
void vor_index_block_encode_4ub(const vor_index_statep state, 
                                const unsigned num_entries,
                                const unsigned* index, 
                                GLubyte* col);

/** Encode index to colour. */
void vor_index_encode_3ubv(const vor_index_statep state, 
                           const unsigned index, GLubyte* c);

/** Encode index to colour. */
void vor_index_encode_4ubv(const vor_index_statep state, 
                           const unsigned index, GLubyte* c);

/** Encode index to colour. */
void vor_index_encode_4f(const vor_index_statep state, const unsigned index, 
                         float* r, float* g, float* b, float* a);

/** Encode index to colour. */
void vor_index_encode_3fv(const vor_index_statep state, 
                          const unsigned index, float* c);

/** Encode index to colour. */
void vor_index_encode_4fv(const vor_index_statep state, 
                          const unsigned index, float* c);

#ifdef __cplusplus
}
#endif

#endif
