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

#include "agent_mesh.h"

AgentMesh::AgentMesh(int groupID, int agentct, int width, int height)
{
	m_groupID = groupID; 
    m_NumAgents = agentct;
    state_agents = NULL;
	m_width = width; 
	m_height = height; 

	use_density = 0; 
	draw_centroids = 1; 

	m_centroid_agents.resize(0); 
	point_vertices_agents = NULL; 
	info_agents= NULL; 
}


AgentMesh::~AgentMesh()
{
	free(point_vertices_agents);

    vor_destroy_state(&state_agents);

	if(info_agents != NULL){
		free(info_agents);
	}

	m_centroid_agents.clear(); 

}

bool AgentMesh::InitializeMesh(int agentct, int agentgroup)
{
	this->m_NumAgents = agentct;
	this->m_groupID = agentgroup; 
	if(point_vertices_agents != NULL){
		free(point_vertices_agents);
	}
	if(info_agents != NULL){
		free(info_agents);
	}
	m_centroid_agents.clear(); 

	point_vertices_agents = (point_struct*) malloc(m_NumAgents * sizeof(point_struct));

    for (int i = 0; i < m_NumAgents; i++) 
	{
		point_vertices_agents[i].x = (float)rand() / (float)RAND_MAX;
		point_vertices_agents[i].y = (float)rand() / (float)RAND_MAX;
	} 

	state_agents = vor_create_state();
    assert(state_agents != NULL);

	int r, g, b, a;
    vor_init_graphics(state_agents);
    vor_get_bit_depths(state_agents, &r, &g, &b, &a);
    printf("r %i g %i b %i a %i\n", r, g, b, a);
    printf("max num indices = %u\n", vor_get_max_num_indices(state_agents));

	vor_set_points(state_agents, m_NumAgents, sizeof(point_struct), 
                        &point_vertices_agents[0].x);

	/* Tell the libaray about the size change */
	vor_set_device_size(state_agents, m_width, m_height);
	if (use_density) {
		fprintf(stderr, "generating density function\n");
		gen_density_function();
		vor_set_density_function(state_agents, m_width, m_height, density);
	} else {
		fprintf(stderr, "clearing density function\n");
		vor_clear_density_function(state_agents);
	}
   
    return true;
}

bool AgentMesh::ClinchMesh(std::vector<point_struct>& agents)
{
	this->m_NumAgents = agents.size();
	this->m_groupID = -1; 
	if(point_vertices_agents != NULL){
		free(point_vertices_agents);
	}
	if(info_agents != NULL){
		free(info_agents);
	}
	m_centroid_agents.clear(); 

	point_vertices_agents = (point_struct*) malloc(m_NumAgents * sizeof(point_struct));

	float pos_x, pos_y; 

	for (int i = 0; i < m_NumAgents; i++) 
	{
		if(i==0 || i == 149 || i == 150 || i == 299 || i == 300 || i == 449 || i == 450 || i == 599)
		{
			i=i;
		}

		if(i <= 149 && i >= 0)
		{
			if(agents[i].x > 0.5 || agents[i].y > 0.5 )
			{
				i = i; 
			}
		}

		if(i <= 299 && i >= 150)
		{
			if(agents[i].x > 0.5 || agents[i].y < 0.5 )
			{
				i = i; 
			}
		}
				
		if(i <= 449 && i >= 300)
		{
			if(agents[i].x < 0.5 || agents[i].y < 0.5 )
			{
				i = i; 
			}
		}
		if(i <= 599 && i >= 450)
		{
			if(agents[i].x < 0.5 || agents[i].y > 0.5 )
			{
				i = i; 
			}
		}

		pos_x= agents[i].x; 
		pos_y =  agents[i].y;
		point_vertices_agents[i].x = agents[i].x;
		point_vertices_agents[i].y = agents[i].y; 
	} 

	vor_set_points(state_agents, m_NumAgents, sizeof(point_struct), 
                    &point_vertices_agents[0].x);

	return true; 

}

bool AgentMesh::UpdateMesh(std::vector<point_struct>& agents)
{
	
	//dead agents(-1, -1)
	if(this->m_NumAgents != agents.size())
		return false; 

	m_centroid_agents.clear(); 

	//m_NumAgents = 0; 

	for (int i = 0; i < agents.size(); i++) 
	{

		point_vertices_agents[i].x = agents[i].x;
		point_vertices_agents[i].y = agents[i].y; 
	//	m_NumAgents++; 
	} 

	vor_set_points(state_agents, m_NumAgents, sizeof(point_struct), 
                    &point_vertices_agents[0].x);

	return true; 

}


void AgentMesh::Render()
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

	vor_calc_diagram_Render(state_agents);

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


void AgentMesh::Read()
{
    const unsigned* index_diagram = NULL;
    unsigned highest_encoded;
    const double total_area = m_width * m_height;

	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, m_width, m_height,GL_RGBA,GL_UNSIGNED_BYTE,state_agents->col_diagram);

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
	vor_calc_diagram_PostProcessing(state_agents); 

    //index_diagram = vor_get_index_diagram(state_markers);


	/* Find out what the highest-numbered encoding was, and allocate enough
	* space for the Voronoi region information */
	highest_encoded_agents = vor_get_highest_encoded_index(state_agents);
	info_agents = (vor_region_info*) malloc((highest_encoded_agents+1) * 
										sizeof(vor_region_info));
	assert(info_agents != NULL);
	vor_calc_region_quantities(state_agents, info_agents);

	point_struct agentcentroid; 
	for(int i =0 ; i<(highest_encoded_agents+1); i++){
		agentcentroid.x = info_agents[i].centroid_x; 
		agentcentroid.y = info_agents[i].centroid_y; 
		this->m_centroid_agents.push_back(agentcentroid);
	}

	free(info_agents);
	info_agents = NULL; 
}

void AgentMesh::DrawDebug(bool draw_centroids, bool keypoints)
{ 
	glDisable(GL_DEPTH_TEST);
	if(draw_centroids){

		float cx, cy; 
		glColor3f(1, 1, 1); 
		glPointSize(1.0);
		for (int i = 0; i <= highest_encoded_agents; ++i) {
			//if(keypoints && i != 0 && i != 149
			//	&& i != 150 && i != 299
			//	&& i != 300 && i != 449
			//	&& i != 382 && i != 383
			//	&& i != 450 && i != 599){
			if(keypoints && i != 512 
				&& i != 511  &&  i != 513){
					continue; 
			}
			
			if(i == 511) 
				glColor3f(1,0,0); 
			if(i == 512) 
				glColor3f(0,1,0); 
			if(i == 513) 
				glColor3f(1,1,0); 

	/*if(i == 383)
	{
	  glColor3f(1,1,1); 
	}
	if(i == 384)
	{
	   glColor3f(0,1,0); 
	}
	if(i == 511)
	{
	   glColor3f(0.5,0.5,0); 
	}
	if(i == 512)
	{
	   glColor3f(1,1,1); 
	}*/
			cx = info_agents[i].centroid_x/m_width; 
			cy = info_agents[i].centroid_y/m_height;
			glBegin(GL_POINTS);
			

			glVertex2f(cx, cy);
			glEnd();
		}
	}
	else{
		float cx, cy; 
		glColor3f(0, 1, 1); 
		for (int i = 0; i <= highest_encoded_agents; ++i) {
			cx = point_vertices_agents[i].x; 
			cy = point_vertices_agents[i].y;
			glBegin(GL_POINTS);
			glVertex2f(cx, cy);
			glEnd();
		}


	}
	glEnable(GL_DEPTH_TEST);
}


void AgentMesh::gen_density_function() {
    unsigned i, j;

    free(density);
    density = (unsigned int*)malloc(m_width * m_height * sizeof(unsigned));

    for (j = 0; j < m_height; ++j) {
        for (i = 0; i < m_width; ++i) {
            density[i + j * m_height] = j;
        }
    }
}
