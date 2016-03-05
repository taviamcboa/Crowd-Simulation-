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

#include "Texture2DBase.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

Texture2DBase::Texture2DBase(int m, int n) : m_grid(m), n_grid(n){

	int pixel_num = m * n; 
	this->m_tex2D.resize(pixel_num); 

}

int Texture2DBase::mapping(int px, int py){

	return px * m_grid + py; 
}

std::vector<NeighboringGrid> Texture2DBase::getNeibhoringGrid(int px, int pz)
{
	std::vector<NeighboringGrid> neighboring; 

	int eastx, northx, westx, southx; 
	int eastz, northz, westz, southz; 

	int id; 

	eastx = px + 1; 
	eastz = pz;

	if(UtilHelper::CellOutofBoundary(eastx, eastz) == false){
		//id = mapping(eastx, eastz);
		neighboring.push_back(NeighboringGrid(eastx, eastz));
	}
	else{

		neighboring.push_back(NeighboringGrid());
	}

	northx = px; 
	northz = pz +1 ;
	if(UtilHelper::CellOutofBoundary(northx, northz) == false){
		//id = mapping(northx, northz);
		neighboring.push_back(NeighboringGrid(northx, northz));
	}
	else{

		neighboring.push_back(NeighboringGrid());
	}

	westx = px -1 ; 
	westz = pz; 
	if(UtilHelper::CellOutofBoundary(westx, westz) == false){
		//id = mapping(westx, westz);
		neighboring.push_back(NeighboringGrid(westx, westz));
	}
	else{
		neighboring.push_back(NeighboringGrid());
	}

	southx = px; 
	southz = pz -1; 
	if(UtilHelper::CellOutofBoundary(southx, southz) == false){
		//id = mapping(southx, southz);
		neighboring.push_back(NeighboringGrid(southx, southz));
	}
	else{
		neighboring.push_back(NeighboringGrid());
	}

	return neighboring; 
}

void Texture2DBase::inversemapping(int p1d, int &px, int &py){

	int rownum = p1d/n_grid; 
	int rowcol = p1d - rownum * m_grid; 

	px = rownum; 
	py = rowcol; 
}


void Texture2DBase::clearAll(){

	this->m_tex2D.resize(m_grid * n_grid, 0);

}

int Texture2DBase::read(int px, int py){

	return m_tex2D.at(mapping(px, py));

}

void Texture2DBase::setAll(int scalar){
	int pixel_num = m_grid * n_grid; 
	this->m_tex2D.resize(pixel_num, scalar);

}
void Texture2DBase::write(int px, int py, int value)
{
	int pixel_num = mapping(px, py); 
	this->m_tex2D.at(pixel_num) = value; 
}