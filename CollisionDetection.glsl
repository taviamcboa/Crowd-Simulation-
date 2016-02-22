///////////////////////////////////////////////////////////////////////////////////////////////////////
const float      TURN_RATE_FAST_FACTOR        = 4.0f;
const float      TURN_RATE_FAST_THRESHOLD     = 0.6f;
const float      OPPOSITE_DIRECTION_DOT_PRODUCT_THRESHOLD = -0.8f;

//buffer 1: position buffer
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


layout (binding = 3, std430)  buffer gid_aid
{
  ivec2 gid_aids[];

} GidAidsBuffer;

layout (binding = 4, std430)  buffer rads
{
  float radians[];

} radsBuffer;

//buffer 3: parking+status
layout (binding = 5, std430)  buffer parkingPosAlive
{
  flat vec4 parkingPosAlive[];

} parkingPosAliveBuffer;

//buffer 2: delta pos; 
layout (binding = 6, std430)  buffer deltaPos
{
  flat vec4 deltapos[];

} deltaPosBuffer;

//buffer 2: delta pos; 
layout (binding = 7, std430)  buffer debugPurpose
{
  flat vec4 debugpurpose[];

} debugPurposeBuffer;

layout (binding = 8, std430)  buffer turnRates
{
  float turnrates[];

} turnRateBuffer;


//texture: gAgentVoronoiMap; 
layout(binding = 0, rgba8) uniform image2D gAgentVoronoiImage;  
layout(binding = 1, rgba8) uniform image2D gMarkerSamplingImage; 
layout(binding = 2, rgba32f) uniform image2DArray gGuidanceImageArray; 
layout(binding = 3, rgba8) uniform image2D gEularianImageFullResolution; 


uniform float gDeltaTime; 
uniform int gHasGuid; 
const vec4 bits = vec4(256 * 256 * 256, 256* 256, 1* 256, 1); // 0 for alpha channel 
const vec2 possiondisc[12] = 
   {
	vec2(-0.326212, -0.40581),
	vec2(-0.840144, -0.07358),
 	vec2(-0.695914, -0.457137),
	vec2(-0.203345, 0.620716),
	vec2(0.96234, -0.194983),
	vec2(0.473434,-0.480026),
	vec2(0.519456, 0.767022),
	vec2(0.185461, -0.893124),
	vec2(0.507431, 0.064425),
	vec2(0.89642, 0.412458),
	vec2(-0.32194, -0.932615),
	vec2(-0.791559, -0.59771)
   };

const vec2 possiondiscReOrdered[16] = 
{
	//reverse the motion orientation
	vec2(0, -1), 
	vec2(-0.32194, -0.932615),
	vec2(0.185461, -0.893124),
	vec2(-0.791559, -0.59771),
	vec2(0.473434,-0.480026),
	vec2(-0.326212, -0.40581),
	vec2(0.96234, -0.194983),
	vec2(-0.840144, -0.07358),
	vec2(1, 0),
	vec2(-1, 0),  
	vec2(0.507431, 0.064425),
    vec2(0.89642, 0.412458),
	vec2(-0.695914, 0.457137),
	vec2(-0.203345, 0.620716),
	vec2(0.519456, 0.767022),
	vec2(0, 1),
};

ivec2 convert_pos2_grid(const in vec2 position)
{
	ivec2 grid; 
	int grid_x = int(position.x * 128); 
	int grid_y = int(position.y * 128); 
	grid = ivec2(grid_x, grid_y); 
	return grid;
}


vec4 fetchBilinear(layout(rgba8)  in image2D Image, in vec2 Interpolant, in ivec2 Texelcoords[4])
{
	vec4 Texel00 = imageLoad(Image, Texelcoords[0]);
	vec4 Texel10 = imageLoad(Image, Texelcoords[1]);
	vec4 Texel11 = imageLoad(Image, Texelcoords[2]);
	vec4 Texel01 = imageLoad(Image, Texelcoords[3]);
	
	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageBilinear(layout(rgba8) in image2D Image, in vec2 Texcoord)
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

vec2 TransformPossion(const vec2 offset, const vec2 motion)
{
   //step1: Transform
   if(length(motion)==0.0f)
      return motion; 

   vec2 axisy = normalize(motion); 
   vec2 axisx = vec2(axisy.y, -axisy.x); 
   
   vec2 newoffset; 
   newoffset = offset.x * axisx + offset.y * axisy; 
   return newoffset; 
}

bool isInside(vec2 texcoords)
{
	return (texcoords.x >= 0)&&(texcoords.x <= 1)&&(texcoords.y >= 0)&&(texcoords.y <= 1); 
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

vec2 DeductNewMotionVector(layout(rgba8) in image2D Image, const in vec2 direction, 
const in float turnrate, const in int AgentID, const in vec2 FragPos, const in vec2 pos, const in vec2 DeltaMove
, const in float currentstepspeed, const in float deltaTime, 
inout int errornum, inout int samplein, inout int sampleout, inout float ratio)
{
	vec2 newMotionVector; 
	vec2 agent_center = FragPos;  
	vec2 c_direction = direction; 
	float c_turnrate = 0.35; //turnrate; 
	int agent_id = AgentID;
	ivec2 i_size = imageSize(Image); 

	float deltaMoveMag = length(DeltaMove);
	if(deltaMoveMag == 0)
		return vec2(0.0f); 

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
		      wi = wicalculator_2(marker_i, agent_center, DeltaMove, i_size); 
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
	vec2 newDirection = (wsigma == 0.0f ? vec2(0.0f) : normalize(sumwvector)); 

	float dotProduct = dot(newDirection, c_direction);
	float turnRate = getTurnRate(c_turnrate, dotProduct, false);
	vec2 newDirection_2 = turnToDirection(c_direction, newDirection, dotProduct, turnRate);


	vec2 newVelocity = newDirection_2 * currentstepspeed;     
    vec2 newDeltaPosition = newVelocity * deltaTime;

	return newDeltaPosition; 
}


layout (local_size_x = 40, local_size_y = 1) in; 
shader CSmain()
{
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 
	int offset = p.x; 
	int a_id = GidAidsBuffer.gid_aids[offset].y;
    int g_id = GidAidsBuffer.gid_aids[offset].x;
	float turnrate = turnRateBuffer.turnrates[a_id]; 
	vec2 direction = DirectionBuffer.Directions[a_id];

	//fetch the Agent ID
	vec2 currentstep_agentpos = PositionBuffer.Positions[a_id].xy; 
	vec2 deltamove = deltaPosBuffer.deltapos[a_id].xy; //deltamove = newDirection * speed * gDeltaTime;
	vec2 velocity = deltamove/gDeltaTime;

	vec2 currentstep_direction = DirectionBuffer.Directions[a_id].xy;
	//will be compasanted 
	float currentstep_speed = SpeedBuffer.Speeds[a_id].x; 
	
	float motionmag = length(deltamove);//== length(newDirection * speed * gDeltaTime)
	vec2 motiondirection = normalize(deltamove); 

	//
	vec2 statuschange = deltaPosBuffer.deltapos[a_id].zw; 
	vec2 laststep_agentpos = currentstep_agentpos.xy - deltamove.xy; 
	float agent_rad = radsBuffer.radians[a_id].r; 

	vec4 markerAgent; 
	uvec2 markerAgentID; 
	unsigned int markeragentID_offset;

	if(statuschange.x == 1.0f && statuschange.y == 1.0f)
	{
		markerAgent = imageBilinear(gAgentVoronoiImage, laststep_agentpos.xy); 
		markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
		markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 
		markerAgentID = uvec2(markerAgentID.x - markeragentID_offset);

		
		if(motionmag > 0 && isnan(motiondirection) == bvec2(false, false))
		{
			//
			int sample_index = 0; 
			vec2 sample_offset = vec2(0.0); 
			float sample_offset_yaxis = 0.0f; 
			int smallmotionvec = 0; 
			vec2 testcollision_pos; 
			vec4 testcollision_agent; 
			uvec2 testcollision_agentID; 
			unsigned int testcollisionagentID_offset;
			bool coords_inside; 
			int ct_outside = 0; 
			//
			vec4 testcollision_eularianmap; 
			for(sample_index = 0; sample_index < 16; sample_index++)
			{
				sample_offset = possiondiscReOrdered[sample_index]; 
				
				sample_offset_yaxis = possiondiscReOrdered[sample_index].y*agent_rad + currentstep_agentpos; 
				
				if(sample_offset_yaxis < 0.0f) //small motion mag 
				{
					smallmotionvec = 1; 
					continue; 
				}

				sample_offset = TransformPossion(sample_offset, deltamove);  //if s == 0 , need 
				testcollision_pos = currentstep_agentpos + sample_offset * agent_rad; 

				coords_inside = isInside(testcollision_pos); 

				if(coords_inside == false)
				{
					ct_outside++; 
					continue; 
				}
            
				testcollision_agent = imageBilinear(gAgentVoronoiImage, testcollision_pos).rgba;
				testcollision_agentID = uvec2(testcollision_agent.r * 256 * 256 * 256 + testcollision_agent.g * 256 * 256 + testcollision_agent.b * 256);
				testcollisionagentID_offset = uvec2(floor(testcollision_agentID.x/256.0f)).x; 
				testcollision_agentID = uvec2(testcollision_agentID.x - testcollisionagentID_offset); 

				if(testcollision_agentID.x != markerAgentID.x)
				{
				   //collision exist
				   break;
				}

				//against the environment map
				testcollision_eularianmap = imageBilinear(gEularianImageFullResolution, testcollision_pos).rgba;
				if(testcollision_eularianmap != vec4(0,0,0,0))
				{
					break; 
				}																						
			}

		
			if(sample_index == 16)
			{
				//DOING NOTHING HERE
				debugPurposeBuffer.debugpurpose[a_id] = vec4(0,1000, 1000, 1000); 
					ivec2 image_size = imageSize(gMarkerSamplingImage); 
				vec2 newAgentPosOffset[4] = 
				{
					vec2(1, 0) * agent_rad ,
					vec2(-1, 0) * agent_rad,
					vec2(0, 1) * agent_rad,
					vec2(0, -0) * agent_rad
				};


				ivec2 NewAgentFragPos[4];
				NewAgentFragPos[0] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[0]) * image_size.xy);
				NewAgentFragPos[1] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[1]) * image_size.xy);
				NewAgentFragPos[2] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[2]) * image_size.xy);
				NewAgentFragPos[3] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[3]) * image_size.xy);

				vec4 newEularian[4];
				newEularian[0] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[0]);
				newEularian[1] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[1]);
				newEularian[2] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[2]);
				newEularian[3] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[3]);


				if(newEularian[0] != vec4(0,0,0,0) && newEularian[1] != vec4(0,0,0,0) && newEularian[2] != vec4(0,0,0,0) && newEularian[3] != vec4(0,0,0,0))
				{
					PositionBuffer.Positions[a_id].xy  = laststep_agentpos;
					SpeedBuffer.Speeds[a_id] = 0.0f; 
				} 
			}
			else
			{
				//
				vec2 translate = possiondiscReOrdered[sample_index]; 
				float distance; 
				float motion_mag = motionmag; 
				vec2 motion_final; 
				vec2 velocity_final; 
				float speedratio = 1.0; 
				float newspeed; 
				if(translate.y >= 0)
				{ 
					distance = agent_rad.x  - abs(translate.y) * agent_rad.x; 
					motion_mag -= distance; 
					motion_mag = motion_mag < 0 ? 0.0f:motion_mag; 
					motion_final = motiondirection * motion_mag; 
					velocity_final = motion_final/gDeltaTime;
				}
				else
				{
					//long offset
					distance = agent_rad.x - abs(translate.y) * agent_rad.x;
					motion_mag -= distance; 
					motion_mag = motion_mag < 0? 0.0f:motion_mag; 
					motion_final = motiondirection * motion_mag; 
					velocity_final = motion_final/gDeltaTime;
				}

				vec2 NewMotionVector = vec2(0.0f); 
				ivec2 image_size = imageSize(gMarkerSamplingImage); 
				vec2 AgentFragPos = ivec2(laststep_agentpos * image_size.xy) + vec2(0.5f, 0.5f);
				int errors, sampleins, sampleouts; 
				float s_ratio; 


				vec2 guid = imageLoad(gGuidanceImageArray, ivec3(convert_pos2_grid(laststep_agentpos), g_id)).xy;
				

				if(motion_final.xy == vec2(0.0f, 0.0f))
				{
					NewMotionVector = DeductNewMotionVector(gMarkerSamplingImage, direction, 
					turnrate, a_id, AgentFragPos, laststep_agentpos, deltamove, currentstep_speed,
					gDeltaTime, errors, sampleins,sampleouts, s_ratio);

				}

				if(guid != vec2(0.0f)&& gHasGuid == 1)
				{
					//NewMotionVector = vec2(0.0f); 
				}

				motion_final.xy = (motion_final.xy == vec2(0.0f, 0.0f) ? NewMotionVector : motion_final.xy); 
				vec2 finalDirection = length(motion_final) > 0.0f ? normalize(motion_final) : vec2(0.0f); 
				
			    speedratio = length(velocity_final) / length(velocity); 
				PositionBuffer.Positions[a_id].xy = laststep_agentpos + motion_final;
				SpeedBuffer.Speeds[a_id] = ( motion_final.xy == vec2(0.0f, 0.0f)? currentstep_speed : currentstep_speed * sqrt(speedratio) );
				//DirectionBuffer.Directions[a_id].xy = finalDirection;

				debugPurposeBuffer.debugpurpose[a_id] = vec4( NewMotionVector, sampleouts, s_ratio) ;


				
				vec2 newAgentPosOffset[4] = 
				{
					vec2(1, 0) * agent_rad ,
					vec2(-1, 0) * agent_rad,
					vec2(0, 1) * agent_rad,
					vec2(0, -0) * agent_rad
				};


				ivec2 NewAgentFragPos[4];
				NewAgentFragPos[0] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[0]) * image_size.xy);
				NewAgentFragPos[1] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[1]) * image_size.xy);
				NewAgentFragPos[2] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[2]) * image_size.xy);
				NewAgentFragPos[3] = ivec2((PositionBuffer.Positions[a_id].xy + newAgentPosOffset[3]) * image_size.xy);

				vec4 newEularian[4];
				newEularian[0] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[0]);
				newEularian[1] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[1]);
				newEularian[2] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[2]);
				newEularian[3] = imageLoad(gEularianImageFullResolution, NewAgentFragPos[3]);


				if(newEularian[0] != vec4(0,0,0,0) && newEularian[1] != vec4(0,0,0,0) && newEularian[2] != vec4(0,0,0,0) && newEularian[3] != vec4(0,0,0,0))
				{
					PositionBuffer.Positions[a_id].xy  = laststep_agentpos;
					SpeedBuffer.Speeds[a_id] = 0.0f; 
				} 
			}

		    if(g_id !=4)
		       PositionBuffer.Positions[a_id].xy = laststep_agentpos;
		}
	  }// live TO live
}


program CollisionDetectionTechnique
{
     cs(430) = CSmain(); 
};