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

#include <assert.h>

#include "marker_mesh.h"

MarkerMesh::MarkerMesh(int markerct, int width, int height)
{
    m_NumMarkers = markerct;
    state_markers = NULL;
	m_width = width; 
	m_height = height; 

	use_density = 0; 
	draw_centroids = 1; 

	m_centroid_markers.resize(0); 
	point_vertices_markers = NULL; 
	info_markers= NULL; 
}


MarkerMesh::~MarkerMesh()
{

	free(point_vertices_markers);

    vor_destroy_state(&state_markers);

	if(info_markers != NULL){
		free(info_markers);
	}

	m_centroid_markers.clear(); 

}

bool MarkerMesh::InitializeMesh()
{
	if(point_vertices_markers != NULL){
		free(point_vertices_markers);
	}
	if(info_markers != NULL){
		free(info_markers);
	}
	m_centroid_markers.clear(); 

	point_vertices_markers = (point_struct*) malloc(m_NumMarkers * sizeof(point_struct));

    for (int i = 0; i < m_NumMarkers; i++) 
	{
		point_vertices_markers[i].x = (float)rand() / (float)RAND_MAX;
		point_vertices_markers[i].y = (float)rand() / (float)RAND_MAX;
	} 

	state_markers = vor_create_state();
    assert(state_markers != NULL);

	int r, g, b, a;
    vor_init_graphics(state_markers);
    vor_get_bit_depths(state_markers, &r, &g, &b, &a);
    printf("r %i g %i b %i a %i\n", r, g, b, a);
    printf("max num indices = %u\n", vor_get_max_num_indices(state_markers));

	vor_set_points(state_markers, m_NumMarkers, sizeof(point_struct), 
                        &point_vertices_markers[0].x);

	/* Tell the libaray about the size change */
	vor_set_device_size(state_markers, m_width, m_height);
	if (use_density) {
		fprintf(stderr, "generating density function\n");
		gen_density_function();
		vor_set_density_function(state_markers, m_width, m_height, density);
	} else {
		fprintf(stderr, "clearing density function\n");
		vor_clear_density_function(state_markers);
	}
   
    return true;
}

void MarkerMesh::Render()
{
    const unsigned* index_diagram = NULL;
    unsigned highest_encoded;
    const double total_area = m_width * m_height;

	glPointSize(1);
    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, m_width, m_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vor_calc_diagram_Render(state_markers);

	//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	//glReadPixels(0, 0, m_width, m_height,GL_RGBA,GL_UNSIGNED_BYTE,state_markers->col_diagram);

	//int rgba[4]; 
	//	for(int i =0; i<640 * 640; i++)
	//	{
	//		rgba[0] = state_markers->col_diagram[4*i]; 
	//		rgba[1] = state_markers->col_diagram[4*i+1]; 
	//		rgba[2] = state_markers->col_diagram[4*i+2]; 
	//		rgba[3] = state_markers->col_diagram[4*i+3]; 
	//	}
	//vor_calc_diagram_PostProcessing(state_markers); 

 //   //index_diagram = vor_get_index_diagram(state_markers);


	///* Find out what the highest-numbered encoding was, and allocate enough
	//* space for the Voronoi region information */
	//highest_encoded_markers = vor_get_highest_encoded_index(state_markers);
	//info_markers = (vor_region_info*) malloc((highest_encoded_markers+1) * 
	//									sizeof(vor_region_info));
	//assert(info_markers != NULL);
	//vor_calc_region_quantities(state_markers, info_markers);

	//if(draw_centroids){
	//	float cx, cy; 
	//	for (int i = 0; i <= 100; ++i) {
	//	//	cx = info_markers[i].centroid_x/m_width; 
	//		//cy = info_markers[i].centroid_y/m_height;

	//		cx = point_vertices_markers[i].x;
	//		cy = point_vertices_markers[i].y;
	//		glBegin(GL_POINTS);
	//		glVertex2f(cx, cy);
	//		glEnd();
	//	}
 // 

	//}

}


void MarkerMesh::Read()
{
    const unsigned* index_diagram = NULL;
    unsigned highest_encoded;
    const double total_area = m_width * m_height;

	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, m_width, m_height,GL_RGBA,GL_UNSIGNED_BYTE,state_markers->col_diagram);

	glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	//int rgba[4]; 
	//	for(int i =0; i<640 * 640; i++)
	//	{
	//		rgba[0] = state_markers->col_diagram[4*i]; 
	//		rgba[1] = state_markers->col_diagram[4*i+1]; 
	//		rgba[2] = state_markers->col_diagram[4*i+2]; 
	//		rgba[3] = state_markers->col_diagram[4*i+3]; 
	//	}
	vor_calc_diagram_PostProcessing(state_markers); 

    //index_diagram = vor_get_index_diagram(state_markers);


	/* Find out what the highest-numbered encoding was, and allocate enough
	* space for the Voronoi region information */
	highest_encoded_markers = vor_get_highest_encoded_index(state_markers);
	info_markers = (vor_region_info*) malloc((highest_encoded_markers+1) * 
										sizeof(vor_region_info));
	assert(info_markers != NULL);
	vor_calc_region_quantities(state_markers, info_markers);

	point_struct markercentroid; 
	for(int i =0 ; i<(highest_encoded_markers+1); i++){
		markercentroid.x = info_markers[i].centroid_x; 
		markercentroid.y = info_markers[i].centroid_y; 
		this->m_centroid_markers.push_back(markercentroid);
	}
}

void MarkerMesh::DrawDebug()
{ 
	glDisable(GL_DEPTH_TEST);
	if(draw_centroids){
		float cx, cy; 
		glColor3f(1, 1, 1); 
		for (int i = 0; i <= highest_encoded_markers; ++i) {
			cx = info_markers[i].centroid_x/m_width; 
			cy = info_markers[i].centroid_y/m_height;
			glBegin(GL_POINTS);
			glVertex2f(cx, cy);
			glEnd();
		}
	}
	glEnable(GL_DEPTH_TEST);
}


void MarkerMesh::gen_density_function() {
    unsigned i, j;

    free(density);
    density = (unsigned int*)malloc(m_width * m_height * sizeof(unsigned));

    for (j = 0; j < m_height; ++j) {
        for (i = 0; i < m_width; ++i) {
            density[i + j * m_height] = j;
        }
    }
}

void MarkerMesh::lloyds_algo_step(const vor_region_info* info) {

	for (int i = 0; i < m_NumMarkers; ++i) {
		point_vertices_markers[i].x = info[i].centroid_x / m_width;
		point_vertices_markers[i].y = info[i].centroid_y / m_height;
	}

}
