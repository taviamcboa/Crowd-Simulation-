layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform image2D gSpeedImage; 
layout (binding = 1, rgba32f) uniform writeonly image2D gCostImage; 
layout (binding = 2, rgba8) uniform image2D gDiscomfortImage; 
layout (binding = 3, rgba32f) uniform readonly image2D gDensityVelocityImage; 


uniform float weightDiscomfort;  //length, time, discomfort
uniform float weightPathLength;
uniform float weightTime;

uniform int predictable; 


const vec4 ZEROS = vec4(0.0f); 
const vec4 ONES = vec4(1.0f); 

const vec4 INFINITIES = vec4(9999.0f);
const vec4 MINIMUM_SPEED = vec4(0.001f); 
const vec4 MINIMUM_DENSITY = vec4(0.5f); 
const vec4 MAXIMUM_DENSITY = vec4(0.8f); 
const vec4 MAXIMUM_SPEED = vec4(12.0f); 

bool isInside(ivec2 p, ivec2 grid_dimension)
{
	return (p.x >=0 )&&(p.x <grid_dimension.x)&&(p.y >=0 )&&(p.y <grid_dimension.y); 
}



vec4 getDiscomfortValueOrInfinity(ivec2 p, ivec2 grid_dimension, layout(rgba8) in image2D Image)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = INFINITIES; 
	}
	else
	{
		v = imageLoad(Image, p); 
		if((v.r + v.g + v.b) > 0.0f && (v.r + v.g + v.b)<3.0f){
			v = INFINITIES; 
		}
	}
	return v; 
}

vec4 getFutureDiscomfortValueOrZero(ivec2 p, ivec2 grid_dimension, layout(rgba32f) readonly in image2D Image)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = ZEROS; 
	}
	else
	{
		v = vec4(imageLoad(Image, p).y); 
		//v = vec4(1,1,1,1); 
	}
	return v; 
}


vec4 getNESWDiscomfortValueOrInfinity(ivec2 p, ivec2 grid_dimension, layout(rgba8) in image2D Image)
{
	ivec2 pNorth = p; 
	ivec2 pEast = p; 
	ivec2 pWest = p; 
	ivec2 pSouth = p; 

	pNorth.y +=1; 
	pEast.x +=1; 
	pWest.x -=1; 
	pSouth.y -=1; 

	vec4 discomfortValue; 
	discomfortValue.x = getDiscomfortValueOrInfinity(pNorth, grid_dimension, Image).y; 
	discomfortValue.y = getDiscomfortValueOrInfinity(pEast, grid_dimension, Image).y; 
	discomfortValue.z = getDiscomfortValueOrInfinity(pSouth, grid_dimension, Image).y; 
	discomfortValue.w = getDiscomfortValueOrInfinity(pWest, grid_dimension, Image).y; 

	return discomfortValue;
}

vec4 getNESWFutureDiscomfortValueOrZero(ivec2 p, ivec2 grid_dimension, layout(rgba32f) readonly in image2D Image)
{
	ivec2 pNorth = p; 
	ivec2 pEast = p; 
	ivec2 pWest = p; 
	ivec2 pSouth = p; 

	pNorth.y +=1; 
	pEast.x +=1; 
	pWest.x -=1; 
	pSouth.y -=1; 

	vec4 Future_discomfortValue; 
	Future_discomfortValue.x = getFutureDiscomfortValueOrZero(pNorth, grid_dimension, Image).y; 
	Future_discomfortValue.y = getFutureDiscomfortValueOrZero(pEast, grid_dimension, Image).y; 
	Future_discomfortValue.z = getFutureDiscomfortValueOrZero(pSouth, grid_dimension, Image).y; 
	Future_discomfortValue.w = getFutureDiscomfortValueOrZero(pWest, grid_dimension, Image).y; 

	return Future_discomfortValue;
}

shader CSmain()
{	
	vec4 texel; 

	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	vec4 speedNESW = imageLoad(gSpeedImage, p); 

	vec4 futureDiscomfortNESW = getNESWFutureDiscomfortValueOrZero(p, ivec2(128, 128), gDensityVelocityImage); 

    vec4 discomfortNESW  = getNESWDiscomfortValueOrInfinity(p, ivec2(128, 128), gDiscomfortImage);
	
	if(predictable > 0){
		discomfortNESW += futureDiscomfortNESW; 
		discomfortNESW.x = (discomfortNESW.x > INFINITIES.x ? INFINITIES.x : discomfortNESW.x) ; 
		discomfortNESW.y = (discomfortNESW.y > INFINITIES.y ? INFINITIES.y : discomfortNESW.y) ; 
		discomfortNESW.z = (discomfortNESW.z > INFINITIES.z ? INFINITIES.z : discomfortNESW.z) ; 
		discomfortNESW.w = (discomfortNESW.w > INFINITIES.w ? INFINITIES.w : discomfortNESW.w) ; 
	}

	// If user passes zero discomfort weight, infinite (wall) discomfort becomes NAN but should be zero instead.
    // Therefore we need to check here.
    if (weightDiscomfort == 0.0f)
    {
        discomfortNESW = ZEROS;    
    }

	vec4 costNESW = ( weightPathLength * speedNESW + vec4(weightTime, weightTime, weightTime, weightTime)
                        + weightDiscomfort * discomfortNESW )/ speedNESW;

   // costNESW = futureDiscomfortNESW; 

    imageStore(gCostImage, p, costNESW); 
}

program CostCalculatorTechnique
{
	cs(430) = CSmain(); 
};