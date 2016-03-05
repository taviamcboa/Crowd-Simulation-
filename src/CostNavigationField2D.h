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

#ifndef _CostNavigationField2D_H_
#define _CostNavigationField2D_H_

#include <vector>
#include <string>
#include "Texture2DBase.h"
#include "DiscomfortNavigationField2D.h"
#include "util.h" 

//input: 
//coeffecient: alpha, beta, theta
//speed texture for f: anisotropic
//discomfort texture for g: anisotropic 
class DiscomfortNavigationField2D; 

//cost is four direction
typedef std::vector<float> CostVector; 

class CostNavigationField2D : public Texture2DBase
{
public:
	CostNavigationField2D(int m=0, int n=0);

	virtual void write(int px, int py, CostVector cost_mac);

	virtual void clearAll(); // 

	virtual void setAll(CostVector cost_mac); 

	virtual CostVector readCostMac(int px, int py);

	void init(DiscomfortNavigationField2D& discomfort_navi2d); 

	void Normalize(int direction); 

	void DeNormalize(int direction); 
    

private:
	//if no pointer, no need for destructor
	bool NormalizedE, NormalizedN, NormalizedW, NormalizedS; 

public: 
	std::vector<CostVector> m_costNavigationField; 

	float costMaxE, costMaxN, costMaxW, costMaxS; 

	float costMinE, costMinN, costMinW, costMinS; 
};

#endif // _SpeedTexture2D_H_
