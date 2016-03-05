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

#ifndef _SketchyInterface_H_
#define _SketchyInterface_H_

#include <vector>
#include <string>
#include "Texture2DBase.h"

struct SketchGrid2D
{
	float x; //mapping back to cell grid later 
	float y;
	int cx; 
	int cz; 

	SketchGrid2D(float _x = 0.f, float _y = 0.f){
		x = _x; 
		y = _y; 
	};

	SketchGrid2D& operator=(const SketchGrid2D& rhs){
		
		x = rhs.x; 
		y = rhs.y; 
		return *this;
	};
};

struct SketchGrid2DScreen
{
	int screen_x; 
	int screen_y;
};

struct SketchyGradient
{
	float gradientx; 
	float gradientz; 
	int flag; 

	SketchyGradient(float _gx=0.0f, float _gz=0.0f, int _flag = 0)
	{
		gradientx = _gx; 
		gradientz = _gz;
		flag = _flag;
	}

	SketchyGradient& operator=(const SketchyGradient& rhs){
		
		gradientx = rhs.gradientx; 
		gradientz = rhs.gradientz; 
		flag  = rhs.flag; 
		return *this;
	};
};


class SketchyInterface : public Texture2DBase
{
public:
	SketchyInterface(int m=0, int n=0);

	virtual void writeSketchyGradient(int px, int py, SketchyGradient sketchy_grad);
	virtual void writeSketchyFlag(int px, int py, int sketchy_flag);

	virtual void clearAll(); // 
	virtual void setAll(SketchyGradient sketchy_grad); 
	virtual void setAll(int sketchy_flag); 
	virtual SketchyGradient readSketchyGradient(int px, int py);
	virtual SketchyGradient readSketchyGradientCopy(int px, int py);
	virtual void writeSketchyGradientCopy(int px, int py, SketchyGradient sketchy_grad);
	virtual int readSketchyFlag(int px, int py);


	virtual SketchyGradient readAggSketchyGradient(int px, int py); 
	virtual void writeAggSketchyGradient(int px, int py, SketchyGradient sketchy_grad);



	void init(std::vector<SketchGrid2D>& sketchy); 
	void copy(); 

	//virtual int mapping(int px, int py); 

	std::vector<SketchyGradient> m_SketchyGradTex; 
	std::vector<SketchyGradient> m_SketchyGradTexCopy; 

	std::vector<int> m_SketchyFlagTex; 
	std::vector<SketchGrid2D> m_SketchyVisualTex; 

	int segment;

	std::vector<SketchyGradient> m_SketchyAggGradTex; 
};

#endif // _DiscomforTexure2D_H_
