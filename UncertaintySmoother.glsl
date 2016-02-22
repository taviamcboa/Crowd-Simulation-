layout (local_size_x = 16, local_size_y = 16) in; 

layout (binding = 0, rgba8) uniform image2D gEularianImage; 
layout (binding = 1, rgba8) uniform image2D gUncertaintyImage; 

const vec4 ZEROS = vec4(0.0f); 
const vec4 ONES = vec4(1.0f); 
const vec4 INFINITIES = vec4(9999.0f);



bool isInside(ivec2 p, ivec2 grid_dimension)
{
	return (p.x >=0 )&&(p.x <grid_dimension.x)&&(p.y >=0 )&&(p.y <grid_dimension.y); 
}

vec4 getUncertaintyValueOrZero(layout(rgba8) in image2D Image, in ivec2 p, in ivec2 grid_dimension)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = ZEROS; 
	}
	else
	{
		v = vec4(imageLoad(Image, p)); 
	}
	return v; 
}

const ivec2 offsetX[5] = 
{
   ivec2(0,0), 
   ivec2(1,0), 
   ivec2(2,0), 
   ivec2(3,0),
   ivec2(4,0)
};

const ivec2 offsetY[5] = 
{
   ivec2(0,0), 
   ivec2(0,1), 
   ivec2(0,2), 
   ivec2(0,3), 
   ivec2(0,4)
};

const ivec2 offsetXY[9] = 
{
   ivec2(-0.707,0.707), 
   ivec2(0,1), 
   ivec2(0.707,0.707), 
   ivec2(-1,0), 
   ivec2(0,0),
   ivec2(1,0),
   ivec2(-0.707,-0.707),
   ivec2(0,-1),
   ivec2(0.707,-0.707)
};
const int weightXY[9] = 
{
   1, 
   2, 
   1, 
   2, 
   4,
   2,
   1,
   2,
   1
};

const float weight[5] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};


vec4 getImage2DValueOrZero(layout(rgba8) in image2D Image, in ivec2 p, in ivec2 grid_dimension)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = ZEROS; 
	}
	else
	{
		v = vec4(imageLoad(Image, p)); 
	}
	return v; 
}

vec4 fetchBilinear(layout(rgba8) in image2D Image, in vec2 Interpolant, in ivec2 Texelcoords[4])
{
	vec4 Texel00 = imageLoad(Image, Texelcoords[0]);
	vec4 Texel10 = imageLoad(Image, Texelcoords[1]);
	vec4 Texel11 = imageLoad(Image, Texelcoords[2]);
	vec4 Texel01 = imageLoad(Image, Texelcoords[3]);
	
	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageBilinear(layout(rgba8) in image2D Image, in vec2 Texelcoord)
{
	ivec2 TexelIndex = ivec2(Texelcoord);
	
	ivec2 Texelcoords[] = ivec2[4](
		TexelIndex + ivec2(0, 0),
		TexelIndex + ivec2(1, 0),
		TexelIndex + ivec2(1, 1),
		TexelIndex + ivec2(0, 1));

	return fetchBilinear(
		Image, 
		fract(Texelcoord), 
		Texelcoords);
}

shader CSmain()
{	
	vec4 texel; 

	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	vec4 uncertainty, eularian; 
	vec4 uncertaintyN, uncertaintyW, uncertaintyS, uncertaintyE;
	vec4 eularianN, eularianW, eularianS, eularianE; 

	ivec2 pNorth, pWest, pSouth, pEast; 
	pNorth = p;  pNorth.y += 1; 
    pWest = p;   pWest.x -= 1; 
	pSouth = p;  pSouth.y -= 1; 
	pEast = p;   pEast.x += 1; 

	
	eularian = getUncertaintyValueOrZero(gEularianImage, p, ivec2(128));
	barrier();
	if(eularian.r + eularian.g + eularian.b > 0.5 && eularian.a > 0.5f)
	{
		eularian.a = 1; 
		imageStore(gUncertaintyImage, p, eularian); 
	}
	else
	{
		imageStore(gUncertaintyImage, p, vec4(0.0f)); 
	}
	
	
	for(int i =0; i<2; i++)
	{
		barrier();

		uncertainty = getUncertaintyValueOrZero(gUncertaintyImage, p, ivec2(128)); 
		uncertaintyN = getUncertaintyValueOrZero(gUncertaintyImage, pNorth, ivec2(128)); 
		uncertaintyW = getUncertaintyValueOrZero(gUncertaintyImage, pWest, ivec2(128)); 
		uncertaintyS = getUncertaintyValueOrZero(gUncertaintyImage, pSouth, ivec2(128)); 
		uncertaintyE = getUncertaintyValueOrZero(gUncertaintyImage, pEast, ivec2(128)); 

		if(uncertainty.a == 0.0f) // amplitude = 0
		{
			//1-neighboring propagate
			if(uncertaintyN.a > 0 )
			{
				uncertainty = uncertaintyN; 
				uncertainty.a *= 0.85f; 
			}
			//1-neighboring propagate
			if(uncertaintyW.a > 0)
			{
				uncertainty = uncertaintyW; 
				uncertainty.a *= 0.85f; 
			}
			//1-neighboring propagate
			if(uncertaintyS.a > 0)
			{
				uncertainty = uncertaintyS; 
				uncertainty.a *= 0.85f; 
			}
			//1-neighboring propagate
			if(uncertaintyE.a > 0)
			{
				uncertainty = uncertaintyE; 
				uncertainty.a *= 0.85f; 
			}
		}
		barrier();
		imageStore(gUncertaintyImage, p, uncertainty); 
	}
}

shader CSmain2()
{	
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	vec4 uncertainty, eularian; 

	eularian = getImage2DValueOrZero(gEularianImage, p, ivec2(128));
	barrier();
	imageStore(gUncertaintyImage, p, eularian); 
	barrier();
	uncertainty = getImage2DValueOrZero(gUncertaintyImage, p, ivec2(128));
	                 
	if(uncertainty.a == 0)
	{	
		vec4 fragColor = vec4(0); 
		for(int i =0; i<9; i++)
		{
			//fragColor += getImage2DValueOrZero(gUncertaintyImage, p + offsetXY[i], ivec2(128)) * weightXY[i]; 
			fragColor += imageBilinear(gUncertaintyImage, p + offsetXY[i]) * weightXY[i]; 
		}
		fragColor = fragColor/16.0f; 
		barrier();
		imageStore(gUncertaintyImage, p, vec4(fragColor)*1.5);
		barrier();
	}
	barrier();
	//uncertainty = getImage2DValueOrZero(gUncertaintyImage, p, ivec2(128));            
	//if(uncertainty.a == 0)
	//{	
	//	vec4 fragColor = vec4(0); 
	//	for(int i =0; i<9; i++)
	//	{
			//fragColor += getImage2DValueOrZero(gUncertaintyImage, p + offsetXY[i], ivec2(128)) * weightXY[i]; 
	//		fragColor += imageBilinear(gUncertaintyImage, p + offsetXY[i]) * weightXY[i]; 
	//	}
	//	fragColor = fragColor/16.0f; 
	//	barrier();
	//	imageStore(gUncertaintyImage, p, vec4(fragColor) * 2 );
	//	barrier();
//	}
//	barrier();
}

program UncertaintySmootherTechnique
{
	cs(430) = CSmain2(); 
};

