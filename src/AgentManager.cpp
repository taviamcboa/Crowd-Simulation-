#include "AgentManager.h"

AgentManager::AgentManager(int _width, int _height)
{
	this->m_width = _width; 
	this->m_height = _height; 
	this->m_agentcollection.resize(0); 
}

AgentManager::~AgentManager()
{
	this->m_agentcollection.clear(); 
}

bool AgentManager::LoadfromFile(const char* filepath)
{
	return true; 

}
void AgentManager::AddGroup(std::vector<point_struct>& agents, 
							Vector2f minR, Vector2f maxR, 
							Vector2f minGoalR, Vector2f maxGoalR, 
							float smin, float smax, int groupID)
{
	//
	std::vector<agent_struct> _new_group; 

	Vector2f posFullScreen; 
	Vector2f posRegion; 
	Vector2f posGoal; 
	Vector2f dir; 
	
	Vector2f parkPos; 

	float _smax; 
	float scale = 0.02; 

	int existedagents = 0; 
	for(int i = 0; i<this->m_agentcollection.size();i++)
	{
		existedagents += this->m_agentcollection[i].size();
	}

	int row; 
	int col;  
	int pixelx;
	int pixely;
	
	int startoffset_pixelx, startoffset_pixely; 
	startoffset_pixelx = startoffset_pixely = 10; 
	

	for(int i = 0; i < agents.size(); i++)
	{
		//
		posFullScreen = Vector2f(agents[i].x/this->m_width, agents[i].y/this->m_height); 
		posRegion = TransformPos(posFullScreen, minR, maxR); 

		_smax = smin + RandomFloat()*(smax - smin); 
		_smax *= scale; 

		posGoal.x = minGoalR.x + RandomFloat()*(maxGoalR.x - minGoalR.x); 
		posGoal.y = minGoalR.y + RandomFloat()*(maxGoalR.y - minGoalR.y);

		dir = Vector2f(posGoal.x - posRegion.x, posGoal.y - posRegion.y);
		dir.Normalize(); 
		//uncertainty group
		if(posGoal.x == -1.0 || posGoal.y == -1.0f)
		{
			dir = Vector2f(0.5 - posRegion.x, 0.5 - posRegion.y); 
		}


		row = existedagents/20; 
		col = existedagents - row * 20; 

		pixelx = startoffset_pixelx + row * 5; 
		pixely = startoffset_pixely + col * 5; 

		/*switch(groupID)
		{
		case 0 :
			offsetAgentID = existedagents;
			row = existedagents / 25;
			col = existedagents % 25; 
			pixelx = col * 20 + 560; 
			pixely = 2.5 + 10 * row; 
			break; 
		case 1:
			offsetAgentID = existedagents - 150; 
			col = offsetAgentID / 25;
			row = offsetAgentID % 25; 
			pixely = row * 20 + 210; 
			pixelx = 2.5 + 10 * col; 
			break;
		case 2:
			offsetAgentID = existedagents - 300; 
			row = offsetAgentID / 25;
			col = offsetAgentID % 25; 
			pixelx = col * 20 + 560; 
			pixely = 2.5 + 10 * row; 
			pixely = 900 - pixely; 
			break; 
		case 3:
			offsetAgentID = existedagents - 450; 
			col = offsetAgentID / 25;
			row = offsetAgentID % 25; 
			pixely = row * 20 + 210; 
		    pixelx = 1.5 + 10 * col; 
			pixelx = 1600-pixelx;
			break;
		case 4:
		}*/

		parkPos.x = float(pixelx)/float(1600); 
		parkPos.y = float(pixely)/float(900); 

		_new_group.push_back(agent_struct(posRegion.x, posRegion.y, parkPos.x, parkPos.y, _smax, _smax, dir.x, 
			dir.y, dir.x, dir.y, posGoal.x, posGoal.y, groupID, existedagents)); 

		existedagents++; 
	}

	this->m_agentcollection.push_back(_new_group); 
}

void AgentManager::DrawAgents(int groupnum, bool all)
{
	int startgroupID, endgroupID; 
	if(all){
		startgroupID = 0 ;
		endgroupID = this->m_agentcollection.size()- 1; 
	}
	else{
		startgroupID = groupnum;
		 endgroupID = groupnum; 
	}

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_width, m_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPolygonMode(GL_FRONT,GL_FILL);
	glPointSize(2);
    glDisable(GL_DITHER);
	float cx, cy; 

	Vector3f colormap[10] = {
		Vector3f(1, 0,0 ), 
		Vector3f(0, 1,0 ),  
		Vector3f(0, 0, 1 ),
		Vector3f(1, 1, 0 ),
		Vector3f(0, 1,1 ),  
		Vector3f(1, 1,1 ),  
		Vector3f(1, 0,1 ), 
		Vector3f(0.2, 0.3,0.4 ),  
		Vector3f(0.5, 0.2,0.1 ), 
		Vector3f(0.3, 0.5,0.3 )
	};


	for (int i = startgroupID; i <= endgroupID; ++i) {
		for(int j = 0 ; j < m_agentcollection[i].size(); j++){
			glColor3f(colormap[i].x, colormap[i].y,colormap[i].z);
			cx = m_agentcollection[i][j].px; 
			cy = m_agentcollection[i][j].py; 
			//glBegin(GL_POINTS);
			//glVertex2f(cx, cy);
			//glEnd();
			DrawCircle(cx, cy, 0.2279 * 0.02, 40); 
		}
	}

	glEnable(GL_DEPTH_TEST);

}

void AgentManager::DrawCircle(float x, float y, float r, int segments)
{
    glBegin( GL_TRIANGLE_FAN );
		glMultiTexCoord2f(GL_TEXTURE0, x, y);
		glVertex2f(x, y);
        for( int n = 0; n <= segments; ++n ) {
            float const t = 2*M_PI*(float)n/(float)segments;
			glMultiTexCoord2f(GL_TEXTURE0, x + sin(t)*r, y + cos(t)*r);
            glVertex2f(x + sin(t)*r, y + cos(t)*r);
        }
    glEnd();
}

void AgentManager::ClinchAgents(std::vector<point_struct>& clichagents)
{
	clichagents.clear(); 
	point_struct agentpoint; 
	for (int i = 0; i < m_agentcollection.size(); ++i) {
		for(int j = 0 ; j < m_agentcollection[i].size(); j++){

			agentpoint.x = m_agentcollection[i][j].px; 
			agentpoint.y = m_agentcollection[i][j].py; 

			if(m_agentcollection[i][j].active == 0){
				agentpoint.x = agentpoint.y = -1; 
			}

			clichagents.push_back(agentpoint);
		}
	}
}

