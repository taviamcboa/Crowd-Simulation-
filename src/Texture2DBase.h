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

#ifndef _Texture2DBase_H_
#define _Texture2DBase_H_

#include <vector>
#include <string>
#include "math_3d.h"


struct NeighboringGrid
{
	int gridx;
	int gridz; 

	NeighboringGrid(int _gridx, int _gridz)
	{

		gridx = _gridx;
		gridz = _gridz; 
	}
	NeighboringGrid()
	{
		gridx = -1; 
		gridz = -1; 

	}
	bool operator==( const NeighboringGrid &nb ) const
	{
		return ((gridx == nb.gridx) && (gridz == nb.gridz));

	}
};

class Texture2DBase
{
public:
	Texture2DBase(int m=0, int n=0);
	void write(int px, int py, int scalar);
	void clearAll();
	void setAll(int scalar = 0); 
	int read(int px, int py);

	inline int getGridX(){
		return m_grid; 
	}
	inline int getGridY(){
		return n_grid; 
	}

protected: 
	virtual int mapping(int px, int py); 
	virtual void inversemapping(int p1d, int &px, int &py);
	virtual std::vector<NeighboringGrid> getNeibhoringGrid(int px, int pz); 

protected:
	//if no pointer, no need for destructor
	int m_grid;
	int n_grid; 
	std::vector<int> m_tex2D; 
};

#endif // _Texture2DBase_H_
