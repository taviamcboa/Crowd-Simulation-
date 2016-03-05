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

#ifndef _DiscomfortNavigationField2D_H_
#define _DiscomfortNavigationField2D_H_

#include <vector>
#include <string>
#include "util.h"

#include "Texture2DBase.h"

struct Obstacle
{
	float posx, posz; 
	//float radius;
	float width, height; 
	float xmax, xmin, zmax, zmin; 

	Obstacle& operator=(const Obstacle& rhs){
	
			posx = rhs.posx; 
			posz = rhs.posz; 
			xmax = rhs.xmax; 
			xmin = rhs.xmin; 

			zmax = rhs.zmax; 
			zmin = rhs.zmin; 

			//radius = rhs.radius;
			width = rhs.width; 
			height = rhs.height; 

			return *this;
	};

	void computeAABB(float posx, float posz, float width, float height)
	{
		xmin = posx - width ; 
		xmax = posx + width; 
		zmin = posz - height; 
		zmax = posz + height; 
	}; 

	Obstacle(float _posx, float _posz, float _width, float _height){
		posx = _posx; 
		posz = _posz; 
		width = _width;
		height = _height; 
		computeAABB(posx, posz, width, height);
	}
};

typedef Obstacle* ObstaclePtr; 

typedef std::vector<ObstaclePtr> ObstaclePtrList; 


class DiscomfortNavigationField2D : public Texture2DBase
{
public:
	DiscomfortNavigationField2D(int m=0, int n=0);
	virtual void write(int px, int py, float value);
	virtual void clearAll(); // 
	virtual void setAll(float value); 

	virtual float readDiscomfortNavigationField2D(int px, int py);

	void update(ObstaclePtrList& o_list); 
	void convert(const ObstaclePtr obstacle); 

	//virtual int mapping(int px, int py); 
	ObstaclePtrList m_optList;

	void Normalize(); 
	void DeNormalize(); 

public:
	//if no pointer, no need for destructor
	std::vector<float> m_discomfortNavigationField2D; 
	float dfmax;
	float dfmin;
	bool Normalized;
};

#endif // _DiscomforTexure2D_H_
