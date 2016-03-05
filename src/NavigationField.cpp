#include "NavigationField.h"
#include <fstream>
#include <sstream>

NavigationField::NavigationField(int _width, int _height)
{
	this->m_width = _width; 
	this->m_height = _height; 
	this->m_navigationCollection.resize(0); 
}

NavigationField::~NavigationField()
{
	this->m_navigationCollection.clear(); 
}

bool NavigationField::LoadfromFile(const char* filepath, int _id)
{
    std::vector<navigation_struct> navi_group; 
	std::ifstream inf(filepath);
	float gx, gz; 
	if( inf.good() )
	{	
		for(int i = 0; i< this->m_width; i++)
		{
			for(int j = 0; j< this->m_height ; j++)
			{
				inf>>gx; 
				inf>>gz; 

				navi_group.push_back(navigation_struct(_id, i, j, gx, gz)); 
			}
		}
	}

	m_navigationCollection.push_back(navi_group); 

	return true; 
}
void NavigationField::AddGroupNavigation(std::vector<navigation_struct>& _navi)
{
	//
	//Vector2f posFullScreen; 

	//int groupID; 

	//posFullScreen = Vector2f(float(pixelx)/this->m_width, float(pixely)/this->m_height); 

	this->m_navigationCollection.push_back(_navi); 
}
