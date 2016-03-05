/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARKER_MESH_H
#define	MARKER_MESH_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

using namespace std;

class MarkerMesh
{
public:
   MarkerMesh(int markerct, int width, int height); 

    ~MarkerMesh();

    bool InitializeMesh();

    void Render();

	void Read();

	void DrawDebug(); 
	
    uint NumMarkers() const
    {
        return m_NumMarkers;
    }
   
	void lloyds_algo_step(const vor_region_info* info); 
	void gen_density_function();
    
public:
	int m_NumMarkers; 
	int m_width, m_height; 

	vor_statep state_markers;

	typedef struct {
		float x, y;
	} point_struct;
    
	point_struct* point_vertices_markers;
	int draw_centroids; 
	int use_density;
	unsigned* density;

	unsigned highest_encoded_markers; 
	vor_region_info* info_markers;

	std::vector<point_struct> m_centroid_markers; 
};


#endif	