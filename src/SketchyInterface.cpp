// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
//
// Sample Application
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
//
// This sample source file is not covered by the EPL as the rest of the SDK
// and may be used without any restrictions. However, the EPL's disclaimer of
// warranty and liability shall be in effect for this file.
//
// *************************************************************************************************

#include "SketchyInterface.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Windows.h"

SketchyInterface::SketchyInterface(int m, int n) : Texture2DBase(m,n),
	segment(0)
{
	int pixel_num = m * n; 
	this->m_SketchyFlagTex.resize(pixel_num, 0);
	this->m_SketchyGradTex.resize(pixel_num, SketchyGradient()); 
	this->m_SketchyGradTexCopy.resize(pixel_num, SketchyGradient()); 
	m_SketchyAggGradTex.resize(pixel_num, SketchyGradient()); 
}

void SketchyInterface::clearAll(){

	Texture2DBase::clearAll(); 

	this->m_SketchyFlagTex.clear(); 
	this->m_SketchyFlagTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, 0); 

	this->m_SketchyGradTex.clear(); 
	this->m_SketchyGradTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, SketchyGradient()); 
	this->m_SketchyGradTexCopy.clear(); 
	this->m_SketchyGradTexCopy.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, SketchyGradient()); 
	
	this->m_SketchyAggGradTex.clear(); 
	m_SketchyAggGradTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, SketchyGradient()); 

}
void SketchyInterface::copy()
{
	for(int i = 0 ; i<Texture2DBase::m_grid; i++)
	{
		for(int j = 0; j <Texture2DBase::n_grid; j++)
		{
			float gx, gz; 
			gx = readSketchyGradient(i, j).gradientx + m_SketchyGradTexCopy.at(mapping(i, j)).gradientx; 
			gz = readSketchyGradient(i, j).gradientz + m_SketchyGradTexCopy.at(mapping(i, j)).gradientz; 

			float gmag; 
			gmag = sqrt(gx*gx + gz*gz); 

			if(gmag >= 1.0f){
				gx = gx/gmag; 
				gz = gz/gmag; 
			}

			m_SketchyGradTexCopy.at(mapping(i, j)) = SketchyGradient(gx*0.98f, gz*0.98f, 0); 
		}
	}

	this->m_SketchyFlagTex.clear(); 
	this->m_SketchyFlagTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, 0); 

	this->m_SketchyGradTex.clear(); 
	this->m_SketchyGradTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, SketchyGradient()); 

	this->m_SketchyAggGradTex.clear(); 
	m_SketchyAggGradTex.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, SketchyGradient()); 
	
}

void SketchyInterface::init(std::vector<SketchGrid2D>& sketchy)
{
	
}

SketchyGradient SketchyInterface::readSketchyGradient(int px, int py)
{
	return m_SketchyGradTex.at(mapping(px, py));
}

SketchyGradient SketchyInterface::readSketchyGradientCopy(int px, int py)
{
	return m_SketchyGradTexCopy.at(mapping(px, py));
}

int SketchyInterface::readSketchyFlag(int px, int py)
{
	return m_SketchyFlagTex.at(mapping(px, py));
}
SketchyGradient SketchyInterface::readAggSketchyGradient(int px, int py)
{
	return m_SketchyAggGradTex.at(mapping(px, py));
}


void SketchyInterface::setAll(SketchyGradient sketchy_grad)
{
	int pixel_num = Texture2DBase::getGridX() * Texture2DBase::getGridY(); 
	this->m_SketchyGradTex.resize(pixel_num, sketchy_grad);

}

void SketchyInterface::setAll(int sketchy_flag)
{
	int pixel_num = Texture2DBase::getGridX() * Texture2DBase::getGridY(); 
	this->m_SketchyFlagTex.resize(pixel_num, sketchy_flag);

}

void SketchyInterface::writeSketchyGradient(int px, int py,  SketchyGradient sketchy_grad)
{
	int pixel_num = mapping(px, py); 
	this->m_SketchyGradTex.at(pixel_num) = sketchy_grad; 
}

void SketchyInterface::writeSketchyFlag(int px, int py,  int sketchy_flag)
{
	int pixel_num = mapping(px, py); 
	this->m_SketchyFlagTex.at(pixel_num) = sketchy_flag; 
}
void SketchyInterface::writeSketchyGradientCopy(int px, int py,  SketchyGradient sketchy_grad)
{
	int pixel_num = mapping(px, py); 
	this->m_SketchyGradTexCopy.at(pixel_num) = sketchy_grad; 
}

void SketchyInterface::writeAggSketchyGradient(int px, int py,  SketchyGradient sketchy_grad)
{
	int pixel_num = mapping(px, py); 
	this->m_SketchyAggGradTex.at(pixel_num) = sketchy_grad; 
}