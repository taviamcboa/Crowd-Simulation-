layout (local_size_x = 40, local_size_y = 1) in; 

const float      TURN_RATE_FAST_FACTOR        = 4.0f;
const float      TURN_RATE_FAST_THRESHOLD     = 0.6f;
const float      OPPOSITE_DIRECTION_DOT_PRODUCT_THRESHOLD = -0.8f;
const vec4 ZEROS = vec4(0.0f); 
const vec4 INFINITIES = vec4(9999.0f);
const float F_NAN = 9999.0f; 

#define             EXIT_MODE_PARK         0
#define             EXIT_MODE_CHANGE_GROUP 1
#define             EXIT_MODE_RESPAWN      2
#define             AGENT_ALIVE 1.0f
#define             AGENT_DEAD 0.0f

#define             READY_UNCERTAINTY  1
#define             IN_UNCERTAINTY     2
#define             DEAD_UNCERTAINTY   3
#define             READY_FORWARD_DEST 1
#define             VISIT_DEST         2
#define             PASSING_TUNNEL     3

layout (binding = 0, std430)  buffer position
{
  vec2 Positions[];

} PositionBuffer;

layout (binding = 1, std430)  buffer direction
{
  vec2 Directions[];

} DirectionBuffer;

layout (binding = 2, std430)  buffer speed
{
  float Speeds[];

} SpeedBuffer;

layout (binding = 3, std430)  buffer maxspeed
{
  float MaxSpeed[];

} MaxSpeedBuffer;

layout (binding = 4, std430)  buffer gid_aid
{
  ivec2 gid_aids[];

} GidAidsBuffer;

layout (binding = 5, std430)  buffer rads
{
  float radians[];

} radsBuffer;

layout (binding = 6, std430)  buffer turnRates
{
  float turnrates[];

} turnRateBuffer;

layout (binding = 7, std430)  buffer parkingPosAlive
{
  flat vec4 parkingPosAlive[];

} parkingPosAliveBuffer;

layout (binding = 8, std430)  buffer originalStartPos
{
  flat vec2 originalstartpos[];

} originalStartPosBuffer;

layout (binding = 9, std430)  buffer deltaPos
{
  flat vec4 deltapos[];

} deltaPosBuffer;

layout (binding = 10, std430)  buffer uncertaintystatus
{
  flat ivec2 uncertaintystatus[]; // .R: STATU
} uncertaintyStatusBuffer;

layout (binding = 11, std430)  buffer DebugGuid
{
  vec4 DebugGuid[]; // .R: STATU
} DebugGuidBuffer;

layout (binding = 13, std430)  buffer BuildingPos
{
  vec2 BuildingPos[]; // .xy: center, .z: width, .w: height
} BuildingPosBuffer;

layout (binding = 12, std430)  buffer VisitingHistory
{
  flat ivec4 visitingHistory[]; // r, g, b, a:padding
} VisitingHistoryBuffer;

layout (binding = 14, std430)  buffer RandomFloat
{
  float randomFloat[]; // r, g, b, a:padding
} RandomFloatBuffer;

layout(binding = 0, rgba32f) uniform image2D gGradientImage01;
layout(binding = 1, rgba32f) uniform image2D gGradientImage23;
layout(binding = 2, rgba32f) uniform image2D gSpeedImage; 
layout(binding = 3, rgba32f) uniform image2D gPotentialImage; 
layout(binding = 4, rgba8) uniform image2D gUncertaintiyImage; 
layout(binding = 5, rgba32f) uniform image2DArray gMaskedGuidanceImage; 
layout(binding = 6, rgba8) uniform image2D gMarkerSamplingImage; 

uniform float gDeltaTime; 
uniform int gExitMode; 
uniform int gBuildingNum; 
uniform int gHasGuid; 

ivec2 convert_pos2_grid(const in vec2 position)
{
	ivec2 grid; 
	int grid_x = int(position.x * 128); 
	int grid_y = int(position.y * 128); 
	grid = ivec2(grid_x, grid_y); 
	return grid;
}

bool isInside(ivec2 p, ivec2 grid_dimension)
{
	return (p.x >=0 )&&(p.x <grid_dimension.x)&&(p.y >=0 )&&(p.y <grid_dimension.y); 
}

vec4 getImageValueOrInfinity(ivec2 p, ivec2 grid_dimension, layout(rgba32f) in image2D Image)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = INFINITIES; 
	}
	else
	{
		v = imageLoad(Image, p); 
	}

	return v; 
}

float getPotentialInfinity(uint  group, ivec2  gridPosition, ivec2 gridDimension, layout(rgba32f) in image2D Image)
{
    float potential;
    
    switch (group)
    {
        case 0:
        {
            potential = getImageValueOrInfinity(gridPosition, gridDimension, Image).x;
            break;
        }
        
        case 1:
        {
            potential = getImageValueOrInfinity(gridPosition, gridDimension, Image).y; 
            break;
        }
        
        case 2:
        {
            potential = getImageValueOrInfinity(gridPosition, gridDimension, Image).z;
            break;
        }
        
        case 3:
        {
            potential = getImageValueOrInfinity(gridPosition, gridDimension, Image).w; 
            break;
        }
    }
    
    return potential;
}

/**
 * Returns true if the position is inside the image with given side length size.
 * P is treated as unnormalized coordinates into the image. Which means, for an
 * image with size = 128, p = (float2)(128.0f, 128.0f) is inside the image.
 */

bool isInsideFloat(vec2 p, ivec2 grid_dimension)
{
    return (p.x >= 0) && (p.x<= 1.0f) && (p.y >= 0) && (p.y <= 1.0f);
}

vec4 fetchBilinear(layout(rgba32f) in image2D Image, in vec2 Interpolant, in ivec2 Texelcoords[4])
{
	vec4 Texel00 = imageLoad(Image, Texelcoords[0]);
	vec4 Texel10 = imageLoad(Image, Texelcoords[1]);
	vec4 Texel11 = imageLoad(Image, Texelcoords[2]);
	vec4 Texel01 = imageLoad(Image, Texelcoords[3]);
	
	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageBilinear(layout(rgba32f) in image2D Image, in vec2 Texcoord)
{
	//const ivec2 SizeArray[3] = ivec2[3](ivec2(256), ivec2(128), ivec2(64));
	//ivec2 Size = SizeArray[Instance];

	ivec2 Size = imageSize(Image);
	vec2 Texelcoord = Texcoord * Size - 0.5;
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

vec4 fetchBilinear_Array(layout(rgba32f) in image2DArray ImageArray, in vec2 Interpolant, in ivec2 Texelcoords[4], uint GroupID)
{
    //only xy valid
	vec4 Texel00 = imageLoad(ImageArray, ivec3(Texelcoords[0], GroupID));
	vec4 Texel10 = imageLoad(ImageArray, ivec3(Texelcoords[1], GroupID));
	vec4 Texel11 = imageLoad(ImageArray, ivec3(Texelcoords[2], GroupID));
	vec4 Texel01 = imageLoad(ImageArray, ivec3(Texelcoords[3], GroupID));

	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageBilinear_Array(layout(rgba32f) in image2DArray ImageArray, in vec2 Texcoord, uint GroupID)
{

	ivec2 Size = imageSize(ImageArray).xy;
	int GroupNum = imageSize(ImageArray).z; 
	vec2 Texelcoord = Texcoord * Size - 0.5;
	ivec2 TexelIndex = ivec2(Texelcoord);
	
	ivec2 Texelcoords[] = ivec2[4](
		TexelIndex + ivec2(0, 0),
		TexelIndex + ivec2(1, 0),
		TexelIndex + ivec2(1, 1),
		TexelIndex + ivec2(0, 1));

	return fetchBilinear_Array(
		ImageArray, 
		fract(Texelcoord), 
		Texelcoords,
		GroupID);
}





vec4 getGradientValueWithBilinearFilteringOrZero(vec2 position, ivec2 grid_dimension, layout(rgba32f) in image2D Image)
{
    vec4 v;
    
    bool isInsideArea = isInsideFloat(position, grid_dimension);

	vec4 imageValue; 
    
    imageValue = imageBilinear(Image, position); 

	v = (isInsideArea == true ? imageValue : vec4(0.0f)); 

   
    return v;
}



vec2 getGradient(uint              group,
                 vec2              positionWithOffset,
                 const ivec2       gridDimension, 
				 layout(rgba32f) in image2D Image01, 
				 layout(rgba32f) in image2D Image23)
{ 
    vec2 gradient;
    
    // Retrieve gradient for agent group
    
    switch (group)
    {
        case 0:
        {
            gradient = getGradientValueWithBilinearFilteringOrZero(positionWithOffset, gridDimension, Image01).xy;
            break;
        }
        
        case 1:
        {
            gradient = getGradientValueWithBilinearFilteringOrZero(positionWithOffset, gridDimension, Image01).zw;
            break;
        }
        
        case 2:
        {
            gradient = getGradientValueWithBilinearFilteringOrZero(positionWithOffset, gridDimension, Image23).xy;
            break;
        }
        
        case 3:
        {
            gradient = getGradientValueWithBilinearFilteringOrZero(positionWithOffset, gridDimension, Image23).zw;
            break;
        }
    }
    
    return gradient;    
}

float getTurnRate(float turnRate, float dotProduct, bool isFacingWall)
{
    //
    // Choose faster turn rate if we are facing a wall or trying to turn around
    //
   
    bool isTurningAround = dotProduct < TURN_RATE_FAST_THRESHOLD; 
    bool isTurningFast = isFacingWall || isTurningAround;

	turnRate = (isTurningFast == true ? turnRate * TURN_RATE_FAST_FACTOR : turnRate);
       
    return turnRate;
}

vec2 turnToDirection(vec2 direction, vec2 targetDirection, float dotProduct, float turnRate)
{
    //
    // Slowly turn towards new direction
    //
    vec2 newDirection;
    
    // If new direction is directly opposite
    if (dotProduct < OPPOSITE_DIRECTION_DOT_PRODUCT_THRESHOLD)
    {
        // Compute a direction perpendicular to the current direction. Then compute the offset vector
        // from the current direction to the perpendicular one. Use part of this offset direction
        // to compute a new direction turned a bit towards the perpendicular direction.
        vec2 orthogonalDirection = vec2(-direction.y, direction.x);
        newDirection = normalize( direction + normalize(orthogonalDirection - direction) * turnRate );
    }
    else
    {
        vec2 directionOffset = targetDirection - direction;
        float directionOffsetLength = length(directionOffset);
        
        //
        // Ensure that the agent does not turn faster than the turn rate allows.
        //
        // Covers against zero directionOffsetLength if turnRate is zero or greater.
        //
        if (directionOffsetLength > turnRate)
        {
            directionOffset /= directionOffsetLength;
            newDirection = normalize( direction + directionOffset * turnRate );
        }
        else
        {
            newDirection = targetDirection;
        }
    }
    
    return newDirection;
}

/**
 * Returns true if the position is inside the buffer with given side length size.
 */

vec4 getValueOrZero(ivec2 p, ivec2 grid_dimension, layout(rgba32f) in image2D Image)
{
    vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = ZEROS; 
	}
	else
	{
		v = imageLoad(Image, p); 
	}
	return v; 
}


vec4 getGuidanceFieldWithBilinearFilteringOrZero(uint groupID, vec2 position, ivec2 grid_dimension, layout(rgba32f) in image2DArray ImageArray)
{
    vec4 v;
    
    bool isInsideArea = isInsideFloat(position, grid_dimension);

	vec4 imageValue; 
    
    imageValue = imageBilinear_Array(ImageArray, position, groupID); 

	v = (isInsideArea == true ? imageValue : vec4(0.0f)); 

    return v;
}
vec4 getGuidanceFieldOrZero(uint groupID, vec2 position, ivec2 grid_dimension, layout(rgba32f) in image2DArray ImageArray)
{
    vec4 v;
    
    bool isInsideArea = isInsideFloat(position, grid_dimension);

	ivec2  gridPositionWithOffset = convert_pos2_grid(position);

	vec4 imageValue; 
    
    imageValue = imageLoad(ImageArray, ivec3(gridPositionWithOffset, groupID)); 

	v = (isInsideArea == true ? imageValue : vec4(0.0f)); 

    return v;
}

vec2 getGuidanceFieldWithBilinear(uint              groupID,
                      vec2              positionWithOffset,
                      const ivec2       gridDimension, 
				      layout(rgba32f) in image2DArray ImageArray)
{ 
    vec2 guid_field;
    
    // Retrieve gradient for agent group
    
    guid_field = getGuidanceFieldWithBilinearFilteringOrZero(groupID,positionWithOffset, gridDimension, ImageArray).xy;

    return guid_field;    
}

vec2 getGuidanceField(uint              groupID,
                      vec2              positionWithOffset,
                      const ivec2       gridDimension, 
				      layout(rgba32f) in image2DArray ImageArray)
{ 
    vec2 guid_field;
    
    // Retrieve gradient for agent group
    
    guid_field = getGuidanceFieldOrZero(groupID,positionWithOffset, gridDimension, ImageArray).xy;

    return guid_field;    
}

//same as paper
//all in coordinates
float wicalculator(vec2 marker_i, vec2 agent_i, vec2 goal_i, ivec2 size_i)
{
   //remapping to 0--1 scope
   vec2 marker_i_remapped = vec2(marker_i.x / size_i.x, marker_i.y / size_i.y); 
   vec2 agent_i_remapped = vec2(agent_i.x / size_i.x, agent_i.y / size_i.y); 

   float length_ma = length(marker_i_remapped - agent_i_remapped); //distance between marker and agent
   float length_ga = length(goal_i - agent_i_remapped); 

   vec2 goalvec = vec2(0.0f);
   if(length_ga > 0)
     	goalvec = normalize(goal_i - agent_i_remapped); 
   vec2 markervec = vec2(0.0f);
   if(length_ma > 0)
        markervec = normalize(marker_i_remapped - agent_i_remapped);  

   float cosangle = dot(markervec, goalvec); 
   float res = (1+cosangle)/(1+length_ma); 
   return res; 
}

float wicalculator_2(vec2 marker_i, vec2 agent_i, vec2 direction_i, ivec2 size_i)
{
   //remapping to 0--1 scope
   vec2 marker_i_remapped = vec2(marker_i.x / size_i.x, marker_i.y / size_i.y); 
   vec2 agent_i_remapped = vec2(agent_i.x / size_i.x, agent_i.y / size_i.y); 

   float length_ma = length(marker_i_remapped - agent_i_remapped); //distance between marker and agent
   float length_ga = length(direction_i); 

   if(length_ga == 0)
      return 0; 

   vec2 goalvec = vec2(0); 
   if(length_ga > 0)
	  goalvec = normalize(direction_i); 

   vec2 markervec = vec2(0); 
   if(length_ma>0)
	   markervec = normalize(marker_i_remapped - agent_i_remapped);  

   float cosangle = dot(markervec, goalvec); 
   float res = (1+cosangle)/(1+length_ma); 
   return res; 
}


vec2 DeductNewMotionVector(layout(rgba8) in image2D Image, const in vec2 goalPos, 
                           const in int AgentID, const in vec2 FragPos, 
						   const in vec2 pos, 
						   const in float maxspeed,
						   //const in float currentstepspeed, 
						   //const in float deltaTime, 
                           inout int errornum, inout int samplein, inout int sampleout, 
						   inout float ratio)
{
	vec2 newMotionVector; 
	vec2 agent_center = FragPos;  
	int agent_id = AgentID;
	ivec2 i_size = imageSize(Image); 

	//radius = 1.5m 
	//radius mapping to X: 1.25m / 50 * 1600 = 40 
	//radius mapping to Y: 1.25m / 50 * 900 = 22.5
	vec2 searchleftbottom = agent_center - vec2(40, 23);
    vec2 searchrightup = agent_center + vec2(40, 23); 
	float xleftstart = searchleftbottom.x; 
    float xrightend = searchrightup.x; 
	float ybottomstart = searchleftbottom.y; 
    float yupend = searchrightup.y; 
	//clamp the scope to the valid 
	xleftstart = clamp(xleftstart, 0.5, 1600.5); 
	xrightend = clamp(xrightend, 0.5, 1600.5); 
	ybottomstart = clamp(ybottomstart, 0.5, 900.5); 
	yupend = clamp(yupend, 0.5, 900.5); 

	vec2 marker_i; //fragment coord
	vec4 marker_sample;  //sampling value
	uvec2 marker_sampleID; //integer id 

	//option1: goal direction
	float wsigma = 0.0;  //total weight
    float wi = 0.0; 
    vec2 sumwvector = vec2(0.0f, 0.0f); 

	int error_id = 0; 
	int sample_in = 0; 
	int sample_out = 0; 
	float ratio_inout; 

	while(ybottomstart < yupend){
        while(xleftstart < xrightend)
		{
           marker_i = vec2(xleftstart, ybottomstart); 

		   if(fract(marker_i)!= 0.5f){
				error_id++; 
		   }
		   //validate the marker_i's fract part is 0.5; 
		   marker_sample = imageLoad(gMarkerSamplingImage, ivec2(marker_i.xy)).rgba;
		   marker_sampleID = uvec2(marker_sample.r * 256 * 256 * 256 + marker_sample.g * 256 * 256 + marker_sample.b * 256);
		   unsigned int markersampleID_offset = uvec2(floor(marker_sampleID.x/256.0f)).x; 
	       marker_sampleID = uvec2(marker_sampleID.x - markersampleID_offset); 

		   if(marker_sampleID.x != agent_id || marker_sample.a == 0.0f)
		   {
			  wi = 0.0f; 
			  sample_out++; 
		   }
		   else
		   {
		      wi = wicalculator(marker_i, agent_center, goalPos, i_size); 
			  sample_in++; 
		   }
           wsigma += wi; 

		   vec2 marker_i_remapped = vec2(marker_i.x / i_size.x, marker_i.y / i_size.y); 
           vec2 agent_i_remapped = vec2(agent_center.x / i_size.x, agent_center.y / i_size.y); 

           sumwvector += wi*(marker_i_remapped - agent_i_remapped);  // in fragment coords
           xleftstart+=1.0f;  //
		}
        xleftstart = searchleftbottom.x;
        xrightend = searchrightup.x;   
        ybottomstart += 1.0f; 
    }
	ratio_inout = float(sample_in)/float(sample_out + sample_in); 
	errornum = error_id; 
	sampleout = sample_out;
	samplein = sample_in;
	ratio = ratio_inout; 

	//normalize
	if(wsigma == 0.0f){
		sumwvector = vec2(0.0f); 
	}
	else
	{
		sumwvector = sumwvector/wsigma;
	}
	float motionmagnitutue = length(sumwvector); 
	vec2 newDirection = (motionmagnitutue == 0.0f ? vec2(0.0f) : normalize(sumwvector)); 
	float s_tuned = min(motionmagnitutue, maxspeed * 0.0056f); 
	newMotionVector = newDirection * s_tuned; 
	return newMotionVector;
}

int SearchNearestBuilding(vec2 agentPos, ivec2 visitingHistory)
{
	vec2 b_center; 
	float distance_ba; 
	float distance_min = 2000.0f; 
	float distance_max = 4000.0f; 
	int b_id = 0; 
	for(int i = 0; i < gBuildingNum; i++)
	{
		b_center = BuildingPosBuffer.BuildingPos[i].xy; 
		distance_ba = length(b_center - agentPos); 
		distance_ba = ( (i == visitingHistory.r || i == visitingHistory.g) ? distance_max : distance_ba); 
		distance_min = (distance_ba < distance_min? distance_ba : distance_min); 
		b_id = (distance_min == distance_ba ? i : b_id );
	}
	return b_id; 
}

float rand(vec2 co){
     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rnd(vec2 x)
{
    int n = int(x.x * 40.0 + x.y * 6400.0);
    n = (n << 13) ^ n;
    return 1.0 - float( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}
int EvaluateBuilding(vec2 agentPos, ivec2 agentGridPos, vec2 agentDirection,  ivec3 visitingHistory, vec3 abt, int agentID)
{
	vec2 b_center; 
	float distance_ba[20]; 
	float distance_max = 0.0f; 
	float angle_ba[20]; 
	float h[20]; 
	int latestvisitbuilding_ID = visitingHistory.y; 
	vec2 reverse_direction = normalize(agentPos.xy - BuildingPosBuffer.BuildingPos[latestvisitbuilding_ID].xy);
	vec2 a_direction = (latestvisitbuilding_ID >= 0 ?  reverse_direction : normalize(vec2(0,0) - agentPos.xy));
	vec2 ba_direction; 
	float alpha = abt.x; 
	float beta = abt.y;
	float theta = abt.z;
	float evaluation[20]; 
	float evaluation_max = 0.0f; 
	float evaluation_total = 0.0f; 
	int b_id_max = -1; 
	int b_id_uncertain = -1; 

	for(int i = 0; i < 20; i++)
	{
		distance_ba[i] = F_NAN;
		angle_ba[i] = F_NAN; 
		h[i] = F_NAN;
		evaluation[i] = F_NAN; 
	}

	for(int i = 0; i < gBuildingNum; i++)
	{
		b_center = BuildingPosBuffer.BuildingPos[i].xy; 
		distance_ba[i] = length(b_center - agentPos);  //noma to 0-1
		distance_max = (distance_ba[i] > distance_max ? distance_ba[i] : distance_max);
		//
		ba_direction = normalize(b_center - agentPos); 
		angle_ba[i] = dot(a_direction, ba_direction); //from -1 to 1 , mapping to 0-1
		angle_ba[i] = 0.5 * (angle_ba[i] + 1.0f); 

		if(i==visitingHistory.r || i==visitingHistory.g || i==visitingHistory.b)
		{
			h[i] = 0.0f;
		}
		else
		{
			h[i] = 1.0f;
		}
	}
	for(int i = 0; i < gBuildingNum; i++)
	{
		distance_ba[i] = distance_ba[i]/distance_max; // 0 --1
		distance_ba[i] = 1.0f - distance_ba[i]; 
		distance_ba[i] = clamp(distance_ba[i], 0, 1); 
	}

	for(int i = 0; i < gBuildingNum; i++)
	{
		evaluation[i] = alpha * distance_ba[i] +  beta * angle_ba[i];
		if(h[i] == 0.0f)
		{
			evaluation[i] = 0.0f;
		}
		evaluation_total += evaluation[i]; 
		evaluation_max = (evaluation[i] > evaluation_max ? evaluation[i] : evaluation_max);
		b_id_max = i ; 
	}
	for(int i = 0; i < gBuildingNum; i++)
	{
		evaluation[i] = evaluation[i]/evaluation_total; 
		evaluation[i] = 1/float(gBuildingNum); 
		if(i>0)
			evaluation[i] += evaluation[i-1];
	}

	float rand = RandomFloatBuffer.randomFloat[agentID].x;
	for(int i = 0; i < gBuildingNum; i++)
	{
		if(evaluation[i] >= rand)
		{
			b_id_uncertain = i; 
			break; 
		}
	}

	return b_id_uncertain; 
}

bool ReachDestination(int B_ID, vec2 agentPos, vec2 dir, float rad, layout(rgba32f) image2D potentialImage, int Coeff)
{
	vec2 b_center; 
	float distance_ba; 
	b_center = BuildingPosBuffer.BuildingPos[B_ID].xy; 
	vec2 sample_offset = dir * rad * Coeff;
	vec2 agentPositionWithOffset = agentPos + sample_offset;
	ivec2 newGridPosition = convert_pos2_grid(agentPositionWithOffset);
	float newPotential = getPotentialInfinity(0, newGridPosition, ivec2(128, 128), potentialImage); 
	bool isNewPositionInsideWall = isinf(newPotential)||(newPotential == INFINITIES.x);
	return isNewPositionInsideWall; 
}

bool ReachDestination2(int B_ID, vec2 agentPos, vec2 dir, float rad, layout(rgba32f) image2D potentialImage, int Coeff)
{
	vec2 b_center; 
	float distance_ba; 
	b_center = BuildingPosBuffer.BuildingPos[B_ID].xy; 
	vec2 sample_offset = dir * rad * Coeff;
	vec2 agentPositionWithOffset = agentPos + sample_offset;
	ivec2 newGridPosition = convert_pos2_grid(agentPositionWithOffset);
	float newPotential = getPotentialInfinity(0, newGridPosition, ivec2(128, 128), potentialImage); 
	bool isNewPositionInsideWall = isinf(newPotential)||(newPotential == INFINITIES.x);

	distance_ba = length(b_center - agentPos); 
    bool nearby = (distance_ba < 10.0/128.0f ? true : false); 
	return isNewPositionInsideWall && nearby; 
}

shader CSmain()
{
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 
	int offset = p.x; 

	int a_id = GidAidsBuffer.gid_aids[offset].y;
    int g_id = GidAidsBuffer.gid_aids[offset].x;

	vec2 laststepPos =  PositionBuffer.Positions[a_id].xy; 

	float radius = radsBuffer.radians[a_id];
	vec2 position  = PositionBuffer.Positions[a_id];
    vec2 direction = DirectionBuffer.Directions[a_id];
	float turnrate = turnRateBuffer.turnrates[a_id]; 
	float maxspeed = MaxSpeedBuffer.MaxSpeed[a_id]; 

	vec2 parkposition = parkingPosAliveBuffer.parkingPosAlive[a_id].xy; 
	
	float alive = parkingPosAliveBuffer.parkingPosAlive[a_id].z; 


	if(g_id < 4)
	{
		if ( alive == AGENT_ALIVE )
		{
			ivec2 gridPosition = convert_pos2_grid(position);
			float fpotential = getPotentialInfinity(g_id, gridPosition, ivec2(128, 128), gPotentialImage);
			bool isInsideWall = isinf(fpotential)||(fpotential == INFINITIES.x);
			isInsideWall = false; // for prevent the navigation error
			if (isInsideWall) // wrong advection
			{
				// Park it
				PositionBuffer.Positions[a_id].xy = parkposition;
				parkingPosAliveBuffer.parkingPosAlive[a_id].z = AGENT_DEAD;
				deltaPosBuffer.deltapos[a_id].xy = PositionBuffer.Positions[a_id].xy - laststepPos.xy;
				deltaPosBuffer.deltapos[a_id].zw = vec2(1.0, 0.0); //from alive to dead
			} 
			else
			{
				// Compute offset for gradient lookup so that we don't retrieve a gradient value
				// influenced by agent itself
				vec2 sample_offset = direction * radius;
				vec2 positionWithOffset = position + sample_offset;
				ivec2  gridPositionWithOffset = convert_pos2_grid(positionWithOffset);
				// Lookup gradient
				vec2 gradient = getGradient(g_id, positionWithOffset,ivec2(128, 128),gGradientImage01,gGradientImage23);
				// Check if the gradient was sampled inside a wall
				bool isInsideWall = any(isinf(gradient) || isnan(gradient));
				// Check if both components are zero, if so gradient is invalid, from all infinite potentials, or
				// from goal areas.
				bool isInsideInvalidEdge = all(gradient == vec2(0.0f, 0.0f));
				//
				// Check if the agent stepped into an exit (0 potential) cell
				//
				float fpotential_exit = getPotentialInfinity(g_id, gridPositionWithOffset, ivec2(128, 128), gPotentialImage);
				if (fpotential_exit == 0.0f)
				{
					switch (gExitMode)
					{
						// If it should park, park it.
						// If it should respawn, park it so that it can respawn next frame.
						case EXIT_MODE_PARK:
						case EXIT_MODE_RESPAWN:
						default:
						{
							PositionBuffer.Positions[a_id].xy = parkposition;
							parkingPosAliveBuffer.parkingPosAlive[a_id].z = AGENT_DEAD;

							deltaPosBuffer.deltapos[a_id].xy = PositionBuffer.Positions[a_id].xy - laststepPos.xy;
							deltaPosBuffer.deltapos[a_id].zw = vec2(1.0, 0.0f);

							break; 
						}

						case EXIT_MODE_CHANGE_GROUP:
						{
							g_id = (g_id + 1) % 4;
							GidAidsBuffer.gid_aids[a_id].x = g_id;
							break;
						}
					}
				}
				else
				{
					//normal agent advection
					bool isFacingWall = isInsideWall || isInsideInvalidEdge;
					//
					// We are only interested in the direction of the gradient.
					// The new direction points opposite of the gradient.
					// The agent moves against the gradient towards the goal.
					//
					vec2 targetDirection = -normalize(gradient);

					bool guidActive =false; 
					vec2 guidanceFieldWithBiliner = getGuidanceFieldWithBilinear(g_id, positionWithOffset, ivec2(128), gMaskedGuidanceImage);
					vec2 guidanceField = getGuidanceField(g_id, positionWithOffset, ivec2(128), gMaskedGuidanceImage);
					guidanceField = imageLoad(gMaskedGuidanceImage, ivec3(gridPositionWithOffset, g_id)).xy; 
					if(length(guidanceFieldWithBiliner.xy) > 0.75f && gHasGuid == 1){
						targetDirection = normalize(guidanceFieldWithBiliner);
						guidActive = true; 
					}
				
					targetDirection =( isFacingWall == true ? -direction : targetDirection );
				
					float dotProduct = dot(targetDirection, direction);
					float turnRate = getTurnRate(turnrate, dotProduct, isFacingWall);
					// Let agent turn towards new direction
					vec2 newDirection = turnToDirection(direction, targetDirection, dotProduct, turnRate);

					float speed = 0.0f;
					// If the agent hit a wall, we want it to turn in place, else compute a speed based on speed field
					if (!isFacingWall)
					{
						//
						// Retrieve speed based on sample position
						// and interpolate from the 2 nearest general directions (N, E, S, W).
						//
						vec4 anisotropicSpeeds = getValueOrZero(gridPositionWithOffset, ivec2(128, 128), gSpeedImage);
                    
						// Pythagoras tells us that for unit vector x * x + y * y = 1. We can use that
						// to retrieve speed components.
						float xComponentFactor = newDirection.x * newDirection.x;
						float yComponentFactor = newDirection.y * newDirection.y;
                    
						bvec2 isGreaterThanEqual = bvec2(newDirection.x >= 0.0f, newDirection.y >= 0.0f);
                    
						// East
						speed = (isGreaterThanEqual.x == true ? speed + (xComponentFactor * anisotropicSpeeds.y) : speed);
						// West
						speed = (isGreaterThanEqual.x == false ? speed + (xComponentFactor * anisotropicSpeeds.w) : speed);       
						// North
						speed = (isGreaterThanEqual.y == true ? speed + (yComponentFactor * anisotropicSpeeds.x) : speed);
						// South
						speed = (isGreaterThanEqual.y == false ? speed + (yComponentFactor * anisotropicSpeeds.z) : speed);
					}
				
					//
					// Limit speed to maximum speed for this agent
					//
					speed = min(speed, maxspeed);
					//
					// Update position and velocity
					//
					vec2 newVelocity = newDirection * speed;     
					vec2 newPosition = position + newVelocity * gDeltaTime;

					// Check if new position is inside a wall
					ivec2 newGridPosition = convert_pos2_grid(newPosition);
					float newPotential = getPotentialInfinity(g_id, newGridPosition, ivec2(128, 128), gPotentialImage); 
					bool isNewPositionInsideWall = isinf(newPotential)||(newPotential == INFINITIES.x);
					// If new position would put agent inside wall, stay put, don't move
					if(isNewPositionInsideWall)
					{
						newPosition = position; 
						speed = 0.0f; 
					}
					//
					// Write new position, direction and speed
					//
        
					PositionBuffer.Positions[a_id].xy = newPosition;
					DirectionBuffer.Directions[a_id].xy = newDirection;
					SpeedBuffer.Speeds[a_id] = speed;
					deltaPosBuffer.deltapos[a_id].xy = PositionBuffer.Positions[a_id].xy - laststepPos.xy;
					deltaPosBuffer.deltapos[a_id].zw = vec2(1.0f, 1.0f); 

					DebugGuidBuffer.DebugGuid[a_id].xy = guidanceField.xy; 
					DebugGuidBuffer.DebugGuid[a_id].zw = deltaPosBuffer.deltapos[a_id].xy; 

					//Uncertainty Related, consider laststepPos
					ivec2 uncertaintyStatus = uncertaintyStatusBuffer.uncertaintystatus[a_id].xy;
					ivec2 last_gridPosition = convert_pos2_grid(laststepPos);
					vec4 uncertaintyField = imageLoad(gUncertaintiyImage, last_gridPosition); 
					int uncertaintyID = int(uncertaintyField.x * 4 + uncertaintyField.y * 2 + uncertaintyField.z); 
					float uncertaintyValue = uncertaintyField.a;  
					float rand = RandomFloatBuffer.randomFloat[a_id].x;
					uncertaintyValue = (uncertaintyValue > rand ? uncertaintyValue : 0.0f); 

					//case 1: from ready to In
					if(uncertaintyStatus.x == READY_UNCERTAINTY && uncertaintyValue > 0.0f )
					{
						PositionBuffer.Positions[a_id].xy = laststepPos.xy;
						DirectionBuffer.Directions[a_id].xy = direction;
						SpeedBuffer.Speeds[a_id] = 0.0f;
						deltaPosBuffer.deltapos[a_id].xy = vec2(0.0f);
						deltaPosBuffer.deltapos[a_id].zw = vec2(0.0f); 

						uncertaintyStatus.x = IN_UNCERTAINTY;
						uncertaintyStatus.y = 300; 

						uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus;
					}
					else if(uncertaintyStatus.x == IN_UNCERTAINTY)
					{
						PositionBuffer.Positions[a_id].xy = laststepPos.xy;
						DirectionBuffer.Directions[a_id].xy = direction;
						SpeedBuffer.Speeds[a_id] = 0.0f;
						deltaPosBuffer.deltapos[a_id].xy = vec2(0.0f);
						deltaPosBuffer.deltapos[a_id].zw = vec2(0.0f); 
						uncertaintyStatus.x = IN_UNCERTAINTY;
						uncertaintyStatus.y-=1; 

						if(uncertaintyStatus.y == 0)
						{
							uncertaintyStatus.x = DEAD_UNCERTAINTY;
							uncertaintyStatus.y = 500; 
							uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
						}
						else
						{
							uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
						}	
					}
					else if(uncertaintyStatus.x == DEAD_UNCERTAINTY)
					{
						uncertaintyStatus.y-=1; 
						if(uncertaintyStatus.y == 0)
						{
							uncertaintyStatus.x = READY_UNCERTAINTY;
							uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
						}
						else
						{
							uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
						}
					}		
				}
			}
		}
		else
		{
			//parked agents
			switch(gExitMode)
			{
				case EXIT_MODE_RESPAWN:
				{
					vec2 originalpos = originalStartPosBuffer.originalstartpos[a_id]; 
					PositionBuffer.Positions[a_id] = vec2(originalpos.x, originalpos.y);
					parkingPosAliveBuffer.parkingPosAlive[a_id].z = AGENT_ALIVE;

					deltaPosBuffer.deltapos[a_id].xy = PositionBuffer.Positions[a_id].xy - laststepPos.xy;
					deltaPosBuffer.deltapos[a_id].zw = vec2(0.0f, 1.0f); 
			
					break; 
				}
				default:
				{
					break;
				}

			}
		}
	}
	else
	{
		//for lagrangian uncertainty group agents
		ivec2 uncertaintyStatus = uncertaintyStatusBuffer.uncertaintystatus[a_id].xy;
		if ( alive == AGENT_ALIVE && (uncertaintyStatus.x == READY_FORWARD_DEST || uncertaintyStatus.x == PASSING_TUNNEL))
		{
			//VisitingHistoryBuffer.visitingHistory[a_id].a= 999; 

			ivec2 gridPosition = convert_pos2_grid(position);
			ivec2 image_size = imageSize(gMarkerSamplingImage); 
			vec2 AgentFragPos = ivec2(position * image_size.xy) + vec2(0.5f, 0.5f);
			int iTargetBuildingID = VisitingHistoryBuffer.visitingHistory[a_id].x; 
			ivec3 visitingHis = VisitingHistoryBuffer.visitingHistory[a_id].yzw;
			vec2 deltaMove = vec2(0.0f);  
			vec2 newDirection = vec2(0.0f); 

			//something to calculate the speed
			float speed = 0.0f; 
			vec2 sample_offset = direction * radius;
			vec2 positionWithOffset = position + sample_offset;
			ivec2  gridPositionWithOffset = convert_pos2_grid(positionWithOffset);
			vec4 anisotropicSpeeds = getValueOrZero(gridPositionWithOffset, ivec2(128, 128), gSpeedImage);
              
			if(iTargetBuildingID < 0) // first time
			{
				//vec2 agentPos, vec2 agentDirection,  ivec3 visitingHistory, vec3 abt
				iTargetBuildingID = EvaluateBuilding(position, gridPosition, direction, visitingHis, vec3(1.0f, 1.0f, 1.0f), a_id); 
				//iTargetBuildingID = a_id % gBuildingNum;
				VisitingHistoryBuffer.visitingHistory[a_id].x = iTargetBuildingID; 
			}

			int errornum = -1; 
			int samplein = -1; 
			int sampleout = -1; 
			float ratio = -1.0f; 
			if(iTargetBuildingID >= 0 && iTargetBuildingID < gBuildingNum)//valid value
			{

				//VisitingHistoryBuffer.visitingHistory[a_id].a = 799; 
				vec2 goalPos = BuildingPosBuffer.BuildingPos[iTargetBuildingID].xy; 
			    
				deltaMove = DeductNewMotionVector(gMarkerSamplingImage, goalPos, 
                           a_id, AgentFragPos, position, maxspeed, 
						   errornum, samplein, sampleout, ratio);
		        DebugGuidBuffer.DebugGuid[a_id].xyzw = vec4(errornum, samplein, sampleout, ratio);
				newDirection = (length(deltaMove) > 0.0f ? normalize(deltaMove) : newDirection); 
				// Pythagoras tells us that for unit vector x * x + y * y = 1. We can use that
				// to retrieve speed components.
				float xComponentFactor = newDirection.x * newDirection.x;
				float yComponentFactor = newDirection.y * newDirection.y;
				bvec2 isGreaterThanEqual = bvec2(newDirection.x >= 0.0f, newDirection.y >= 0.0f);
				// East
				speed = (isGreaterThanEqual.x == true ? speed + (xComponentFactor * anisotropicSpeeds.y) : speed);
				// West
				speed = (isGreaterThanEqual.x == false ? speed + (xComponentFactor * anisotropicSpeeds.w) : speed);       
				// North
				speed = (isGreaterThanEqual.y == true ? speed + (yComponentFactor * anisotropicSpeeds.x) : speed);
				// South
				speed = (isGreaterThanEqual.y == false ? speed + (yComponentFactor * anisotropicSpeeds.z) : speed);
				//clamp
				speed = min(speed, maxspeed);

				//consider the guid
				int guid_ID; 
				if(goalPos.y > positionWithOffset.y)
				{
					guid_ID = 0; 
				}
				else
				{
					guid_ID = 1; 
				}
				if(uncertaintyStatus.x == PASSING_TUNNEL)
				{
					guid_ID = uncertaintyStatus.y; 
				}

		

				vec2 guidanceFieldWithBiliner = getGuidanceFieldWithBilinear(guid_ID, positionWithOffset, ivec2(128), gMaskedGuidanceImage);
				vec2 guidanceField = getGuidanceField(guid_ID, positionWithOffset, ivec2(128), gMaskedGuidanceImage);
				guidanceField = imageLoad(gMaskedGuidanceImage, ivec3(gridPositionWithOffset, g_id)).xy; 
				if(length(guidanceFieldWithBiliner.xy) > 0.75f && gHasGuid == 1){
					newDirection = normalize(guidanceFieldWithBiliner);
					float dotProduct = dot(newDirection, direction);
					float turnRate = getTurnRate(turnrate, dotProduct, false);
					// Let agent turn towards new direction
					newDirection = turnToDirection(direction, newDirection, dotProduct, turnRate);
					vec2 newVelocity = newDirection * speed;     
					vec2 newPosition = position + newVelocity * gDeltaTime;
					// Check if new position is inside a wall
					ivec2 newGridPosition = convert_pos2_grid(newPosition);
					float newPotential = getPotentialInfinity(0, newGridPosition, ivec2(128, 128), gPotentialImage); 
					bool isNewPositionInsideWall = isinf(newPotential)||(newPotential == INFINITIES.x);
					// If new position would put agent inside wall, stay put, don't move
					if(isNewPositionInsideWall)
					{
						newPosition = position; 
						speed = 0.0f; 
					}
					PositionBuffer.Positions[a_id].xy = newPosition;
					DirectionBuffer.Directions[a_id].xy = newDirection;
					SpeedBuffer.Speeds[a_id] = speed;
					deltaPosBuffer.deltapos[a_id].xy = newPosition - position;
					deltaPosBuffer.deltapos[a_id].zw = vec2(1.0f, 1.0f); 

					//UPDATE TUNNEL INFO
					if(uncertaintyStatus.x == READY_FORWARD_DEST)
					{
						uncertaintyStatus.x = PASSING_TUNNEL;
						uncertaintyStatus.y = guid_ID; 
						uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus;
					}
				}
				else{
					//no guidance
					PositionBuffer.Positions[a_id].xy = position + deltaMove;
					DirectionBuffer.Directions[a_id].xy = newDirection;
					SpeedBuffer.Speeds[a_id] = speed;
					deltaPosBuffer.deltapos[a_id].xy = deltaMove;
					deltaPosBuffer.deltapos[a_id].zw = vec2(1.0f, 1.0f); 

					if(uncertaintyStatus.x == PASSING_TUNNEL)
					{
					    //leave the guidance area
						uncertaintyStatus.x = READY_FORWARD_DEST;
						uncertaintyStatus.y = 0; 
						uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus;
					}
				}
				

				//Decide Distance 
				ivec2 newGridPosition = convert_pos2_grid(PositionBuffer.Positions[a_id].xy);
				vec4 uncertaintyField = imageLoad(gUncertaintiyImage, newGridPosition); 
				int uncertaintyID = int(uncertaintyField.x * 4 + uncertaintyField.y * 2 + uncertaintyField.z); 
				float uncertaintyValue = uncertaintyField.a;  
			//	float rand = rand(newGridPosition); 
			//	rand = clamp(rand, 0.0f, 1.0f); 
			//	//attach the eularian uncertainty
			//	uncertaintyValue = (uncertaintyValue > rand ? uncertaintyValue : 0.0f); 

				bool reachdest; 
				if(uncertaintyValue > 0.0f)//near to somewhat building
				{
					//
					 reachdest = ReachDestination2(iTargetBuildingID, PositionBuffer.Positions[a_id].xy, 
											 DirectionBuffer.Directions[a_id].xy, radius, gPotentialImage, 2);

					 
					// reachdest = true; 
					 if(reachdest == true)
					 {
						//update visiting list; 
						VisitingHistoryBuffer.visitingHistory[a_id].w = VisitingHistoryBuffer.visitingHistory[a_id].z; 
						VisitingHistoryBuffer.visitingHistory[a_id].z = VisitingHistoryBuffer.visitingHistory[a_id].y;
						VisitingHistoryBuffer.visitingHistory[a_id].y = iTargetBuildingID; 

						iTargetBuildingID = EvaluateBuilding(PositionBuffer.Positions[a_id].xy, newGridPosition, newDirection, VisitingHistoryBuffer.visitingHistory[a_id].yzw, vec3(1.0f, 1.0f, 1.0f),
						a_id); 
						VisitingHistoryBuffer.visitingHistory[a_id].x = iTargetBuildingID; 

						//update state machine
						if(uncertaintyStatus.x == READY_FORWARD_DEST)
						{
							uncertaintyStatus.x = VISIT_DEST;
							uncertaintyStatus.y = 100; 
							uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus;
						}
					 }
				}
			}
		}
		else if(alive == AGENT_ALIVE && uncertaintyStatus.x == VISIT_DEST)
		{
			DebugGuidBuffer.DebugGuid[a_id].xyzw = vec4(2); 
			PositionBuffer.Positions[a_id].xy = laststepPos.xy;
			DirectionBuffer.Directions[a_id].xy = vec2(0.0f);
			SpeedBuffer.Speeds[a_id] = 0.0f;
			deltaPosBuffer.deltapos[a_id].xy = vec2(0.0f);
			deltaPosBuffer.deltapos[a_id].zw = vec2(0.0f); 
			uncertaintyStatus.x = VISIT_DEST;
			uncertaintyStatus.y = uncertaintyStatus.y-1; 

			if(uncertaintyStatus.y == 0)
			{
				uncertaintyStatus.x = READY_FORWARD_DEST;
				uncertaintyStatus.y = 0; 
				uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
			}
			else
			{
				uncertaintyStatusBuffer.uncertaintystatus[a_id].xy = uncertaintyStatus; 
			}	
		}
		else
		{
		    //parked agents
			switch(gExitMode)
			{
				case EXIT_MODE_RESPAWN:
				{
					vec2 originalpos = originalStartPosBuffer.originalstartpos[a_id]; 
					PositionBuffer.Positions[a_id] = vec2(originalpos.x, originalpos.y);
					parkingPosAliveBuffer.parkingPosAlive[a_id].z = AGENT_ALIVE;

					deltaPosBuffer.deltapos[a_id].xy = PositionBuffer.Positions[a_id].xy - laststepPos.xy;
					deltaPosBuffer.deltapos[a_id].zw = vec2(0.0f, 1.0f); 
			
					break; 
				}
				default:
				{
					break;
				}

			}

		}
	}
}

program AdvectionMoveTechnique
{
    cs(430) = CSmain(); 
};