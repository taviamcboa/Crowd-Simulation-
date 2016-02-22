#pragma unroll

struct VSInput
{
    vec3 Position; //current position
    vec2 TexCoord; //current texcoord

	vec4 GoalPriviousDir; //goal + previous motion direction
	vec2 NavigatonDir; //NAVI + groupid + agentid; 
	vec4 WeightsCombo;  //.a is searching radius
	vec4 agentID;  //.b is max speed
};

interface VSOutput
{
    vec2 TexCoord;
	vec4 GoalPriviousDir; // 
	vec2 NavigatonDir; //NAVI + groupid + agentid; 
	vec4 WeightsCombo;  //.a is searching radius
	vec4 agentID; 
};

uniform sampler2D gAgentVoronoiMap;      
uniform sampler2D gMarkerSamplingMap;       //previous Pass

layout(binding = 4, r32i) uniform iimage2D gDensityMap;
layout(binding = 5, rgba8i) uniform iimage2D gAntiGradientMap;
layout(binding = 6, rgba32f) uniform imageBuffer gPreviousMotionVector; 

//writeonly uniform imageBuffer gMotionBuffer; //current Output
layout(binding = 2, rgba32f) uniform imageBuffer gMotionBuffer;
uniform sampler2DArray gNaviTexArray;     //navigation 
uniform sampler2DArray gGuidTexArray;       //previous Pass


const vec4 bits = vec4(256 * 256 * 256, 256* 256, 1* 256, 0); // 0 for alpha channel 
const vec2 cellsize = vec2(25.0, 14.0625); 

vec4 packFloatToVec4i(const float value)
{
   const vec4 bitSh = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
   const vec4 bitMsk = vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
   vec4 res = fract(value * bitSh);
   res -= res.xxyz * bitMsk;
   return res;
}
 
// Unpacking a [0-1] float value from a 4D vector where each component was a 8-bits integer
float unpackFloatFromVec4i(const vec4 value)
{
   const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
   return(dot(value, bitSh));
}


float wicalculatorFullVersion(vec2 marker_i, vec2 agent_i, vec2 goal_i, vec2 previous_i, 
                   vec2 navigation_i, vec3 weightcombo)
{
   float length_ma = length(marker_i - agent_i); 
   float length_ga = length(goal_i - agent_i); 

   vec2 goalvec = normalize(goal_i - agent_i); 

   vec2 previousvec = normalize(previous_i);
   vec2 navigation = normalize(navigation_i);

   vec2 markervec = normalize(marker_i - agent_i);  
   
   vec2 combovec = weightcombo.x * goalvec + weightcombo.y * previousvec + 
   weightcombo.z * navigation; 

   combovec = normalize(combovec); 

   float cosangle = dot(markervec, combovec); 

   float res = (1+cosangle)/(1+length_ma); 
   return res; 
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

vec4 fetchArrayBilinear(in sampler2DArray Sampler, in vec2 Interpolant, in ivec3 Texelcoords[4], in int gid)
{
	vec4 Texel00 = texelFetch(Sampler, Texelcoords[0], 0);
	vec4 Texel10 = texelFetch(Sampler, Texelcoords[1], 0);
	vec4 Texel11 = texelFetch(Sampler, Texelcoords[2], 0);
	vec4 Texel01 = texelFetch(Sampler, Texelcoords[3], 0);
	
	//transform back
	Texel00.r = Texel00.b > 0.5 ? Texel00.r : -Texel00.r; 
	Texel00.g = Texel00.a > 0.5 ? Texel00.g : -Texel00.g; 

	Texel10.r = Texel10.b > 0.5 ? Texel10.r : -Texel10.r; 
	Texel10.g = Texel10.a > 0.5 ? Texel10.g : -Texel10.g; 

	Texel11.r = Texel11.b > 0.5 ? Texel11.r : -Texel11.r; 
	Texel11.g = Texel11.a > 0.5 ? Texel11.g : -Texel11.g; 

	Texel01.r = Texel01.b > 0.5 ? Texel01.r : -Texel01.r; 
	Texel01.g = Texel01.a > 0.5 ? Texel01.g : -Texel01.g; 

	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageArrayBilinear(in sampler2DArray Sampler, in vec2 Texcoord, in int gid)
{
	ivec2 Size = textureSize(Sampler, 0).xy; //lod not equal to gid
	vec2 Texelcoord = Texcoord * Size - 0.5;
	ivec3 TexelIndex = ivec3(Texelcoord, gid);
	
	ivec3 Texelcoords[] = ivec3[4](
		TexelIndex + ivec3(0, 0, 0),
		TexelIndex + ivec3(1, 0, 0),
		TexelIndex + ivec3(1, 1, 0),
		TexelIndex + ivec3(0, 1, 0)
	);

	return fetchArrayBilinear(
		Sampler, 
		fract(Texelcoord), 
		Texelcoords, 
		gid);
}

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
    VSout.TexCoord   =  VSin.TexCoord;   
	VSout.GoalPriviousDir = VSin.GoalPriviousDir; // 
	VSout.NavigatonDir   =  VSin.NavigatonDir;   
	VSout.WeightsCombo   =  VSin.WeightsCombo; 
	VSout.agentID = VSin.agentID; 

    gl_Position = vec4(2*(VSin.Position.x-0.5), 2*(VSin.Position.y-0.5), VSin.Position.z, 1.0); 
};


shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	//Sample the AgentVoronoiMap to fetch the Agent ID
	ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	vec4 markerAgent = texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgba; 
	uvec2 markerAgentID; 
	
	markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
	unsigned int markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 

	markerAgentID = uvec2(markerAgentID.x - markeragentID_offset); 

	//load the para
	 float gSearchRadius = FSin.WeightsCombo.a; 
	 vec2 goal_dir = FSin.GoalPriviousDir.rg; 
	 vec2 previous_dir = FSin.GoalPriviousDir.ba; 
	 vec2 navigation_dir = FSin.NavigatonDir.rg; 
	 vec3 weightcomb = FSin.WeightsCombo.rgb; 

	 //search scope
	 vec2 agent_center = gl_FragCoord.xy; 
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

	 //density and anti-gradient
	 ivec2 fragcoords = ivec2(gl_FragCoord.xy);
	 ivec2 gridXY = ivec2(int(float(fragcoords.x)/cellsize.x), int(float(fragcoords.y)/cellsize.y));  
	 int densityvalue = imageLoad(gDensityMap, gridXY).r; 
	 ivec2 anti_gradient = -imageLoad(gAntiGradientMap, gridXY).rg; 
	 float anti_gradient_mag = length(anti_gradient); 
	 vec2 anti_gradient_norm = vec2(0.0f); 
	 if(anti_gradient_mag > 0)
		anti_gradient_norm = normalize(anti_gradient); 

	 //option1: goal direction
	 float wsigma = 0.0;  //total weight
     float wi = 0.0; 
     vec2 sumwvector = vec2(0.0f, 0.0f); 

	 //option2: navigation
	 float wsigma_nav = 0.0; 
	 float wi_nav = 0.0f; 
	 vec2 sumwvector_nav = vec2(0.0f, 0.0f); 

	 //option3: anti-gradient
	 float wsigma_antigradient = 0.0f; 
	 float wi_antigradient  = 0.0f; 
	 vec2 sumwvector_antigradient  = vec2(0.0f, 0.0f); 

	 //option4: guidance; 
	 float wsigma_guidance = 0.0f; 
	 float wi_guidance = 0.0f; 
	 vec2 sumwvector_guidance = vec2(0.0f); 

	 int error_id = 0; 
	 int sample_in = 0; 
	 int sample_out = 0; 
	 float ratio_inout; 

	 int agentid_input = int(FSin.agentID.y); 
	 int groupid_input = int(FSin.agentID.x); 

	 vec4 navi_dir =  imageArrayBilinear(gNaviTexArray, FSin.TexCoord.xy, groupid_input);
	 float navi_mag = length(navi_dir.rg); 

	 vec4 guid_dir =  imageArrayBilinear(gGuidTexArray, FSin.TexCoord.xy, groupid_input);
	 float guid_mag = length(guid_dir.rg); 

	 //vec4 pre_navi_color = imageLoad(gMotionBufferNoCollision, agentid_input); //only need that direction
	 //if(pre_navi_color.x == -99.0f&&pre_navi_color.y == -99.0f&&pre_navi_color.z == -99.0f&&pre_navi_color.w == -99.0f)
	 //{
	 //	imageStore(gMotionBufferNoCollision, agentid_input, vec4(navi_color));
	 //}
	 //else
	 //{
	 //	imageStore(gMotionBufferNoCollision, agentid_input, vec4(pre_navi_color));
	 //}

	 while(ybottomstart < yupend){
        while(xleftstart < xrightend)
		{
           marker_i = vec2(xleftstart, ybottomstart); 

		   if(fract(marker_i)!= 0.5f){
				error_id++; 
		   }
		   //validate the marker_i's fract part is 0.5; 
		   marker_sample = texelFetch(gMarkerSamplingMap, ivec2(marker_i.xy), 0).rgba;
		   marker_sampleID = uvec2(marker_sample.r * 256 * 256 * 256 + marker_sample.g * 256 * 256 + marker_sample.b * 256);
		   unsigned int markersampleID_offset = uvec2(floor(marker_sampleID.x/256.0f)).x; 
	       marker_sampleID = uvec2(marker_sampleID.x - markersampleID_offset); 

		   if(marker_sampleID.x != markerAgentID || marker_sample.a == 0.0f)
		   {
			  wi = 0.0f; 
			  wi_nav = 0.0f; 
			  wi_antigradient = 0.0f; 
			  wi_guidance = 0.0f; 
			  sample_out++; 
		   }
		   else
		   {
		      wi = wicalculator(marker_i, agent_center, goal_dir, Size); 
			  sample_in++; 

			  if(navi_dir.x != -99.0f && navi_dir.y != -99.0f){
				 wi_nav  =  wicalculator_2(marker_i, agent_center, navi_dir.xy, Size); 
			  }
			  else{
				 wi_nav =-99.0f; 
			  } 

			  wi_antigradient = wicalculator_2(marker_i, agent_center, anti_gradient_norm.xy, Size); 
			  wi_guidance = wicalculator_2(marker_i, agent_center, guid_dir.xy, Size); 
		   }
           wsigma += wi; 
		   if(wi_nav != -99.0f)
		      wsigma_nav += wi_nav; 
		   wsigma_antigradient += wi_antigradient;
		   wsigma_guidance += wi_guidance;


		   vec2 marker_i_remapped = vec2(marker_i.x / Size.x, marker_i.y / Size.y); 
           vec2 agent_i_remapped = vec2(agent_center.x / Size.x, agent_center.y / Size.y); 

           sumwvector += wi*(marker_i_remapped - agent_i_remapped);  // in fragment coords

		   if(wi_nav != -99.0f)
			  sumwvector_nav += wi_nav * (marker_i_remapped - agent_i_remapped); 

		   sumwvector_antigradient += wi_antigradient*(marker_i_remapped - agent_i_remapped);
		   sumwvector_guidance += wi_guidance*(marker_i_remapped - agent_i_remapped);

           xleftstart+=1.0f;  //

		}
        xleftstart = searchleftbottom.x;
        xrightend = searchrightup.x;   
        ybottomstart += 1.0f; 
    }

	ratio_inout = float(sample_in)/float(sample_out + sample_in); 

	//normalize
	if(wsigma == 0.0f){
		sumwvector = vec2(0.0f); 
	}
	else
	{
		sumwvector = sumwvector/wsigma;
	}

	if(wsigma_nav == 0.0f){
		sumwvector_nav = vec2(0.0f); 
	}
	else
	{
		sumwvector_nav = sumwvector_nav/wsigma_nav;
	}

	if(wsigma_antigradient == 0.0f)
	{
		sumwvector_antigradient = vec2(0.0f); 
	}
	else
	{
		sumwvector_antigradient = sumwvector_antigradient/wsigma_antigradient;
	}

	if(wsigma_guidance == 0.0f)
	{
		sumwvector_guidance = vec2(0.0f); 
	}
	else
	{
		sumwvector_guidance = sumwvector_guidance/wsigma_guidance;
	}

	float sumvector_mag = length(sumwvector); 
	float sumvector_mag_nav = length(sumwvector_nav); 
	float sumvector_mag_antigradient = length(sumwvector_antigradient);
	float sumvector_mag_guidance = length(sumwvector_guidance);

	float sumvector_mag_scalars[4] = {sumvector_mag, sumvector_mag_nav, sumvector_mag_antigradient, sumvector_mag_guidance};

	vec2 sumvector_mag_vectors[4] =  {sumwvector, sumwvector_nav, sumwvector_antigradient, sumwvector_guidance};
	float sumvector_mag_max = 0.0f; 
	vec2 sumvector_vector_max = vec2(0.0f); 


	for(int i = 0; i< 4; i++)
	{
		if(sumvector_mag_scalars[i] > sumvector_mag_max){
			sumvector_mag_max = sumvector_mag_scalars[i];
			sumvector_vector_max = sumvector_mag_vectors[i]; 
		} 
	}

	//predictable motion vec
	float smax = FSin.agentID.b;
	smax = smax * 0.02f; // according to 50 m width; 
	float s_tuned[4]; //goal-oriented, nav, antigradient, guidance

	vec2 motionvector_s[4] = 
	{
		vec2(0), 
		vec2(0), 
		vec2(0), 
		vec2(0)
	}; 

	for(int i =0 ;i <4; i++)
	{
		s_tuned[i] = min(sumvector_mag_scalars[i], smax); 
		if(s_tuned[i]>0)
		{
			motionvector_s[i] = normalize(sumvector_mag_vectors[i]) * s_tuned[i];
		}
	}
	//stuned is to test the future congestion condition. 
	vec2 future_motion[4]; 
	future_motion[0] = motionvector_s[0] *  8 + FSin.TexCoord.xy; 
	future_motion[0] = clamp(future_motion[0], 0.0, 1.0); 
	future_motion[1] = motionvector_s[1] *  8 + FSin.TexCoord.xy; 
	future_motion[1] = clamp(future_motion[1], 0.0, 1.0); 
	future_motion[2] = motionvector_s[2] *  8 + FSin.TexCoord.xy;  
	future_motion[2] = clamp(future_motion[2], 0.0, 1.0); 
	future_motion[3] = motionvector_s[3] *  8 + FSin.TexCoord.xy; 
	future_motion[3] = clamp(future_motion[3], 0.0, 1.0); 

	vec2 future_fragcoords[4]; 
	ivec2 frag_dimension = ivec2(1600, 900);  
	future_fragcoords[0] = future_motion[0] * frag_dimension; 
	future_fragcoords[1] = future_motion[1] * frag_dimension; 
	future_fragcoords[2] = future_motion[2] * frag_dimension; 
	future_fragcoords[3] = future_motion[3] * frag_dimension; 
	
	ivec2 future_gridXY[4]; 
	future_gridXY[0] = ivec2(int(future_fragcoords[0].x/cellsize.x), int(future_fragcoords[0].y/cellsize.y));  
	future_gridXY[1] = ivec2(int(future_fragcoords[1].x/cellsize.x), int(future_fragcoords[1].y/cellsize.y));  
	future_gridXY[2] = ivec2(int(future_fragcoords[2].x/cellsize.x), int(future_fragcoords[2].y/cellsize.y));  
	future_gridXY[3] = ivec2(int(future_fragcoords[3].x/cellsize.x), int(future_fragcoords[3].y/cellsize.y));  

	int future_densityvalue[4]; 
	future_densityvalue[0] = imageLoad(gDensityMap, future_gridXY[0]).r; 
	future_densityvalue[1] = imageLoad(gDensityMap, future_gridXY[1]).r; 
	future_densityvalue[2] = imageLoad(gDensityMap, future_gridXY[2]).r; 
	future_densityvalue[3] = imageLoad(gDensityMap, future_gridXY[3]).r; 
    int future_density_min = 100;
	int future_density_max = 0;  
	int min_index, max_index; 
	int gap_max_min; 
	for(int i = 0 ; i<4; i++)
	{
		if(future_densityvalue[i] < future_density_min)
		{
			future_density_min = future_densityvalue[i];
			min_index = i;  
		}
		if(future_densityvalue[i] > future_density_max)
		{
			future_density_max  = future_densityvalue[i];	
			max_index = i ;
		}
	}
	gap_max_min = max_index - min_index; 

	//angle between goal and these
	float radian_angle[4]; 
	if(sumvector_mag_scalars[0] > 0 && sumvector_mag_scalars[0] >0)
	{
	   radian_angle[0] = dot(sumvector_mag_vectors[0],sumvector_mag_vectors[0]); 
	   radian_angle[0] = radian_angle[0]/(sumvector_mag_scalars[0] * sumvector_mag_scalars[0]); 
	}
	if(sumvector_mag_scalars[0] > 0 && sumvector_mag_scalars[1] >0)
	{
	   radian_angle[1] = dot(sumvector_mag_vectors[0],sumvector_mag_vectors[1]); 
	   radian_angle[1] = radian_angle[1]/(sumvector_mag_scalars[0] * sumvector_mag_scalars[1]); 
	}
	if(sumvector_mag_scalars[0] > 0 && sumvector_mag_scalars[2] >0)
	{
	   radian_angle[2] = dot(sumvector_mag_vectors[0],sumvector_mag_vectors[2]); 
	   radian_angle[2] = radian_angle[2]/(sumvector_mag_scalars[0] * sumvector_mag_scalars[2]); 
	}
	if(sumvector_mag_scalars[0] > 0 && sumvector_mag_scalars[3] >0)
	{
	   radian_angle[3] = dot(sumvector_mag_vectors[0],sumvector_mag_vectors[3]); 
	   radian_angle[3] = radian_angle[3]/(sumvector_mag_scalars[0] * sumvector_mag_scalars[3]); 
	}

	//coarse-level magnitutd
	float navi_antigrad_guid[3] = 
	{
		navi_mag, 
		anti_gradient_mag,
		guid_mag
	}; 


	//imageStore(gMotionBuffer, agentid_input, vec4(agentid_input , anti_gradient_mag, anti_gradient_norm.rg));
	
	//1. future prediction: future_densityvalue[4]..   gap_max_min >= 3, or  
	//2. local available marker: sumvector_mag_scalars[4] 
	//3. user-define: navi_antigrad_guid[3], 
	//4. vector: sumvector_mag_scalars[4]. 

	//when to consider anti_gradient direction as the most dominant?  1. gap_max_min >= 3; 2. navi_antigrad_guid[1] >=3
	//when to consider guidance as the most dominant? 1. the amplitude > 0.5f; 

	//imageStore(gMotionBuffer, agentid_input, vec4(before_sort_mag_id[0], before_sort_mag_id[1], before_sort_mag_id[2], before_sort_mag_id[3]));
	
	float cellwidth = 1.0f/64.0f; 
	float cell_diag = sqrt(cellwidth * cellwidth + cellwidth * cellwidth);
	if(length(goal_dir - FSin.TexCoord.xy) > cell_diag * 10){
		if(sumvector_mag_scalars[1] > 0.001) //exist navigation field
			sumwvector = sumvector_mag_scalars[2] > sumvector_mag_scalars[1] ? sumvector_mag_vectors[2] : sumvector_mag_vectors[1]; 

	}
	vec4 previous_info = imageLoad(gPreviousMotionVector, agentid_input);
	

	vec2 previous_motion_vector = previous_info.ba;
	if(previous_motion_vector.x != -99 && previous_motion_vector.x != -99 && length(previous_motion_vector) >0)
	{
		previous_motion_vector = normalize(previous_motion_vector); 
		float radangle = atan(previous_motion_vector.y, previous_motion_vector.x); 
		float degree = degrees(radangle); 
		float degree1 = degree + 60; 
		degree1 = degree1 > 180 ? -360 + degree1 : degree1; 
		degree1 = radians(degree1); 
		float degree2 = degree - 60; 
		degree2 = degree2 < -180 ? 360 + degree2 : degree2; 
		degree2 = radians(degree2); 

		vec2 edge1 = vec2(cos(degree1), sin(degree1)); 
		vec2 edge2 = vec2(cos(degree2), sin(degree2)); 

		vec2 direction_mv; 
		float mag_mv; 
		float rad_mv; 
		float degree_mv;
		float degree_between; 
		float degree_mv_new; 
		float rad_mv_new; 
		float degree_adjust;
		float mag_scaledown; 
		if(length(sumwvector) > 0)
		{
			mag_mv = length(sumwvector);
			direction_mv = normalize(sumwvector); 
			rad_mv = atan(direction_mv.y, direction_mv.x); 
			degree_mv= degrees(rad_mv);
			degree_between = degree_mv - degree; 
			degree_between = degree_between > 180? 360 - degree_between : degree_between;
			degree_between = degree_between < -180? 360 + degree_between : degree_between;
		    

			if(degree_between > 45.f)
			{
				mag_scaledown = 45.0f / degree_between;
				degree_mv_new = degree + 30.f; 
				degree_mv_new = degree_mv_new > 180 ? 360 - degree_mv_new : degree_mv_new; 
				rad_mv_new = radians(degree_mv_new);

				sumwvector = 0 * mag_mv * vec2(cos(rad_mv_new), sin(rad_mv_new)); 
			}
			else if(degree_between < -45.f)
			{

				mag_scaledown = -60.0f / degree_between;
				degree_mv_new = degree - 30.f; 
				degree_mv_new = degree_mv_new < -180 ? 360 + degree_mv_new : degree_mv_new; 
				rad_mv_new = radians(degree_mv_new);
				sumwvector = 0 * mag_mv * vec2(cos(rad_mv_new), sin(rad_mv_new)); 
			}
			else
			{
				degree_mv_new = degree_mv;
				rad_mv_new = radians(degree_mv_new);
			}

			degree_adjust = degree_mv_new - degree_mv; 
		}

	
		//float angle1 = dot(edge1, previous_motion_vector);
		//float angle2 = dot(edge2, previous_motion_vector);

		imageStore(gMotionBuffer, agentid_input, vec4(degree, degree_mv, degree_between, degree_adjust ));
	}
	else
	{
		imageStore(gMotionBuffer, agentid_input, vec4(-1, -1, -1, -1 ));

	}


	vec2 flag= vec2(0.0f);

	flag.x = sumwvector.x < 0? 1  : 0; 
	flag.y = sumwvector.y < 0? 1  : 0; 
	sumwvector.x = sumwvector.x < 0? -sumwvector.x  : sumwvector.x; 
	sumwvector.y = sumwvector.y < 0? -sumwvector.y  : sumwvector.y; 

	FragColor = vec4(sumwvector, flag);
};

shader FSmain2(in VSOutput FSin, out vec4 FragColor)
{
	vec4 sample_agent = texture(gAgentVoronoiMap, FSin.TexCoord.xy); 
	float agent_id = dot(bits, sample_agent); 

	vec4 sample_marker = texture(gMarkerSamplingMap, FSin.TexCoord.xy);

	ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	vec2 Texelcoord = FSin.TexCoord.xy * Size;
	ivec2 TexelIndex = ivec2(Texelcoord);

	TexelIndex = ivec2(gl_FragCoord.xy-0.5);
	
	ivec2 Texelcoords[] = ivec2[4](
		TexelIndex + ivec2(0, 0),
		TexelIndex + ivec2(1, 0),
		TexelIndex + ivec2(1, 1),
		TexelIndex + ivec2(0, 1));

	vec2 frac = fract(Texelcoord); 
	ivec2 idvec = ivec2(FSin.agentID.x, FSin.agentID.y);

    float agentID00, agentID10, agentID11, agentID01; 
	vec4 Texel00 = texelFetch(gMarkerSamplingMap, ivec2(Texelcoords[0]), 0);
	vec4 Texel10 = texelFetch(gMarkerSamplingMap, ivec2(Texelcoords[1]), 0);
	vec4 Texel11 = texelFetch(gMarkerSamplingMap, ivec2(Texelcoords[2]), 0);
	vec4 Texel01 = texelFetch(gMarkerSamplingMap, ivec2(Texelcoords[3]), 0);

	agentID00 = dot(bits, Texel00); 
	agentID10 = dot(bits, Texel10); 
	agentID11 = dot(bits, Texel11); 
	agentID01 = dot(bits, Texel01); 

	//imageStore(gMotionBuffer, idvec.y, vec4(gl_FragCoord.xy, Texelcoord.xy));
	imageStore(gMotionBuffer, idvec.y, vec4(agentID00, agentID10, agentID11, agentID01));


	 float indexer_agent = dot(sample_agent, bits); 

	 float gSearchRadius = FSin.WeightsCombo.a; 
	 vec2 goal_dir = FSin.GoalPriviousDir.rg; 
	 vec2 previous_dir = FSin.GoalPriviousDir.ba; 
	 vec2 navigation_dir = FSin.NavigatonDir.rg; 
	 vec3 weightcomb = FSin.WeightsCombo.rgb; 


	 vec2 agent_i = FSin.TexCoord.xy; 
	 vec2 searchleftbottom = agent_i - vec2(0.0145856, 0.0145856);
     vec2 searchrightup = agent_i + vec2(0.0145856, 0.0145856); 

	 float xleftstart = searchleftbottom.x; 
     float xrightend = searchrightup.x; 

	 float ybottomstart = searchleftbottom.y; 
     float yupend = searchrightup.y; 

	 float indexer_marker; 
     vec2 marker_i; 

	 float wsigma = 0.0; 
     float wi = 0.0; 
     vec2 sumwvector = vec2(0.0f, 0.0f); 
	 vec3 test; 

	 while(ybottomstart < yupend){
        while(xleftstart < xrightend)
		{
           marker_i = vec2(xleftstart, ybottomstart); 
           indexer_marker = dot(texture(gMarkerSamplingMap, marker_i), bits);

           if(sample_agent != texture(gMarkerSamplingMap, marker_i)){
              wi = 0.0f; 
           }
           else
           {
              wi = wicalculator(marker_i, agent_i, goal_dir, previous_dir, navigation_dir, 
								weightcomb); 
           }
           wsigma += wi; 
           sumwvector += wi*(marker_i - agent_i); 
           xleftstart+=0.0015625;  //640
		}
        xleftstart = searchleftbottom.x;
        xrightend = searchrightup.x;   
        ybottomstart += 0.0015625; 
    }
	//normalize
    sumwvector = sumwvector/wsigma;

	FragColor = vec4(1, 1 , 0.0, 1.0);
}

shader FSmain3(in VSOutput FSin, out vec4 FragColor)
{

	 ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	 vec4 sample_agent = texture(gAgentVoronoiMap, FSin.TexCoord.xy); 
	 ivec2 indexer_agent = ivec2(dot(sample_agent, bits)); 

	 //configuring coefficent
	 float gSearchRadius = FSin.WeightsCombo.a; 
	 vec2 goal_dir = FSin.GoalPriviousDir.rg; 


	 vec2 previous_dir = FSin.GoalPriviousDir.ba; 
	 vec2 navigation_dir = FSin.NavigatonDir.rg; 
	 vec3 weightcomb = FSin.WeightsCombo.rgb; 

	 //
	 vec2 agent_i = FSin.TexCoord.xy; 
	 vec2 agent_Texelcoord = agent_i * Size;
	 ivec2 agent_TexelIndex = ivec2(agent_Texelcoord);
	 ivec2 agent_TexelIndex2 = ivec2(gl_FragCoord.xy - 0.5); 
	 //vec2(0.0145856, 0.0145856);

	 vec2 searchleftbottom = agent_i - vec2(0.015, 0.015);
	 vec2 searchleftbottom_Texelcoord = searchleftbottom * Size; 
	 ivec2 searchleftbottom_TexelIndex = ivec2(searchleftbottom_Texelcoord);

     vec2 searchrightup = agent_i + vec2(0.015, 0.015); 
	 vec2 searchrightup_Texelcoord = searchrightup * Size; 
	 ivec2 searchrightup_TexelIndex = ivec2(searchrightup_Texelcoord);

	 float xleftstart = searchleftbottom.x; 
     float xrightend = searchrightup.x; 

	 float ybottomstart = searchleftbottom.y; 
     float yupend = searchrightup.y; 

	 ivec2 indexer_marker; 
     vec2 marker_i; 
	 vec2 marker_i_Texelcoord;
	 ivec2 marker_i_TexelIndex;

	 float wsigma = 0.0; 
     float wi = 0.0; 
     vec2 sumwvector = vec2(0.0f, 0.0f); 

	 while(ybottomstart < yupend){
        while(xleftstart < xrightend)
		{
           marker_i = vec2(xleftstart, ybottomstart); 
		   marker_i_Texelcoord  = marker_i * Size; 
		   marker_i_TexelIndex = ivec2(marker_i_Texelcoord);

		   float indexer_marker_f = dot(texelFetch(gMarkerSamplingMap, marker_i_TexelIndex, 0), bits); 
		   indexer_marker = ivec2(indexer_marker_f, indexer_marker_f);

		   //old style glsl
           //indexer_marker = dot(texture(gMarkerSamplingMap, marker_i), bits);

           if(indexer_marker.r != indexer_agent.r){
              wi = 0.0f; 
           }
           else
           {
              wi = wicalculator(marker_i, agent_i, goal_dir, previous_dir, navigation_dir, 
								weightcomb); 
           }
           wsigma += wi; 
           sumwvector += wi*(marker_i - agent_i); 
           xleftstart+=0.0015625;  //640
		}
        xleftstart = searchleftbottom.x;
        xrightend = searchrightup.x;   
        ybottomstart += 0.0015625; 
    }
	//normalize

    sumwvector = sumwvector/wsigma;

	//if(wsigma < 0.1){
	//	sumwvector = vec2(0.0f); 

	//}

	vec2 flag= vec2(0.0f);
	flag.x = sumwvector.x < 0? 1  : 0; 
	flag.y = sumwvector.y < 0? 1  : 0; 
	sumwvector.x = sumwvector.x < 0? -sumwvector.x  : sumwvector.x; 
	sumwvector.y = sumwvector.y < 0? -sumwvector.y  : sumwvector.y; 
	//int agentid = FSin.agentID.y; 
	//int groupid = FSin.agentID.x; 
	
	ivec2 idvec = ivec2(FSin.agentID.x, FSin.agentID.y);

	imageStore(gMotionBuffer, idvec.y, vec4(idvec.x, idvec.y, sumwvector.xy));

	//FragColor = vec4(packFloatToVec4i(sumwvector.r*10).ba, packFloatToVec4i(sumwvector.g*10).ba);
	FragColor = vec4(sumwvector, flag);
};

//test 20130721
shader FSmain4(in VSOutput FSin, out vec4 FragColor)
{
	ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	 vec4 sample_agent = texture(gAgentVoronoiMap, FSin.TexCoord.xy); 
	 ivec2 indexer_agent = ivec2(dot(sample_agent, bits)); 

	 //configuring coefficent
	 float gSearchRadius = FSin.WeightsCombo.a; 
	 vec2 goal_dir = FSin.GoalPriviousDir.rg; 
	 vec2 previous_dir = FSin.GoalPriviousDir.ba; 
	 vec2 navigation_dir = FSin.NavigatonDir.rg; 
	 vec3 weightcomb = FSin.WeightsCombo.rgb; 

	 //
	 vec2 agent_i = FSin.TexCoord.xy; 
	 vec2 agent_Texelcoord = agent_i * Size;
	 ivec2 agent_TexelIndex = ivec2(agent_Texelcoord);
	 ivec2 agent_TexelIndex2 = ivec2(gl_FragCoord.xy - 0.5); 
	 //vec2(0.0145856, 0.0145856);

	 vec2 searchleftbottom = agent_i - vec2(0.015, 0.015);
	 vec2 searchleftbottom_Texelcoord = searchleftbottom * Size; 
	 ivec2 searchleftbottom_TexelIndex = ivec2(searchleftbottom_Texelcoord);

     vec2 searchrightup = agent_i + vec2(0.015, 0.015); 
	 vec2 searchrightup_Texelcoord = searchrightup * Size; 
	 ivec2 searchrightup_TexelIndex = ivec2(searchrightup_Texelcoord);

	 float xleftstart = searchleftbottom.x; 
     float xrightend = searchrightup.x; 

	 float ybottomstart = searchleftbottom.y; 
     float yupend = searchrightup.y; 

	 ivec2 indexer_marker; 
     vec2 marker_i; 
	 vec2 marker_i_Texelcoord;
	 ivec2 marker_i_TexelIndex;

	 float wsigma = 0.0; 
     float wi = 0.0; 
     vec2 sumwvector = vec2(0.0f, 0.0f); 

	 while(ybottomstart < yupend){
        while(xleftstart < xrightend)
		{
           marker_i = vec2(xleftstart, ybottomstart); 
		   marker_i_Texelcoord  = marker_i * Size; 
		   marker_i_TexelIndex = ivec2(marker_i_Texelcoord);

		   float indexer_marker_f = dot(texelFetch(gMarkerSamplingMap, marker_i_TexelIndex, 0), bits); 
		   indexer_marker = ivec2(indexer_marker_f, indexer_marker_f);

		   //old style glsl
           //indexer_marker = dot(texture(gMarkerSamplingMap, marker_i), bits);

           if(indexer_marker.r != indexer_agent.r){
              wi = 0.0f; 
           }
           else
           {
              wi = wicalculator(marker_i, agent_i, goal_dir, previous_dir, navigation_dir, 
								weightcomb); 
           }
           wsigma += wi; 
           sumwvector += wi*(marker_i - agent_i); 
           xleftstart+=0.0015625;  //640
		}
        xleftstart = searchleftbottom.x;
        xrightend = searchrightup.x;   
        ybottomstart += 0.0015625; 
    }
	//normalize

    sumwvector = sumwvector/wsigma;

	//if(wsigma < 0.1){
	//	sumwvector = vec2(0.0f); 

	//}

	vec2 flag= vec2(0.0f);
	flag.x = sumwvector.x < 0? 1  : 0; 
	flag.y = sumwvector.y < 0? 1  : 0; 
	sumwvector.x = sumwvector.x < 0? -sumwvector.x  : sumwvector.x; 
	sumwvector.y = sumwvector.y < 0? -sumwvector.y  : sumwvector.y; 
	//int agentid = FSin.agentID.y; 
	//int groupid = FSin.agentID.x; 
	
	ivec2 idvec = ivec2(FSin.agentID.x, FSin.agentID.y);

	imageStore(gMotionBuffer, idvec.y, vec4(idvec.x, idvec.y, sumwvector.xy));

	//FragColor = vec4(packFloatToVec4i(sumwvector.r*10).ba, packFloatToVec4i(sumwvector.g*10).ba);
	FragColor = vec4(sumwvector, flag);
};


program MotionComputingTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
	//fs(430) = FSmain2();
	//fs(430) = FSmain3();//pure goal driven
	//fs(430) = FSmain4();
};