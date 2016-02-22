struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
	vec4 GoalPriviousDir; // 
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

layout(binding = 1, rgba32f) uniform imageBuffer gMotionBufferNoCollision; 
layout(binding = 4, r32i) uniform iimage2D gDensityField;
layout(binding = 2, r32i) uniform iimageBuffer gDensityFieldBuffer;
layout(binding = 0) uniform sampler2D gAgentVoronoiMap;      
layout(binding = 3) uniform sampler2D gMotionMap;       //previous Pass



//uniform sampler2DArray gNaviTexArray;     //navigation 
//layout(binding = 4, rgba32f) readonly uniform imageBuffer gMotionBufferNoCollisionPre;

const vec4 bits = vec4(256 * 256 * 256, 256* 256, 1* 256, 1); // 0 for alpha channel 
const vec2 step = vec2(0.0015625, 0.0015625); 
const vec2 cellsize = vec2(25.0, 14.0625); 

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

vec2 clampCoordinates(vec2 coords)
{
   vec2 clampedcoords; 
   clampedcoords.x = clamp(coords.x, 0, 1); 
   clampedcoords.y = clamp(coords.y, 0, 1); 
   return clampedcoords;
}

int outsideCoordinates(vec2 coords)
{
	if(coords.x > 1.0f || coords.x < 0.0f || coords.y > 1.0f || coords.y < 0.0f)
	{

		return 1; 
	}
	else 
		return 0; 

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
	//fetch the Agent ID
	ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	vec4 markerAgent = texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgba; 
	uvec2 markerAgentID; 
	markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
	unsigned int markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 
	markerAgentID = uvec2(markerAgentID.x - markeragentID_offset); 

	//fetch the motion vector
	vec2 agent_center = gl_FragCoord.xy; 
	vec4 motion_agent = texelFetch(gMotionMap, ivec2(agent_center.xy), 0).rgba;
	motion_agent.x = (motion_agent.b == 1.0? -motion_agent.x  : motion_agent.x); 
	motion_agent.y = (motion_agent.a == 1.0? -motion_agent.y  : motion_agent.y); 

	vec2 motionvec = motion_agent.rg; 
	float motionmagnitutue = length(motionvec); 
	float smax = FSin.agentID.b;
	smax = smax * 0.02f; // according to 50 m width; 
	float s_tuned = min(motionmagnitutue, smax); 

	float s_bigger = (s_tuned == smax ? 1.0 : 0.0 ) ; 

	motionvec = normalize(motionvec)*s_tuned; 

	//collision detection
	float agentradiustest = FSin.WeightsCombo.a; 
	if(agentradiustest == 0.0f)
		agentradiustest = 6.25;   //7.2928;
	vec2 agent_i_nextstep = FSin.TexCoord.xy + motionvec; 
	//clamp the new pos
	agent_i_nextstep = clampCoordinates(agent_i_nextstep);
	 
	int agentid_input = int(FSin.agentID.y); 
	int groupid_input = int(FSin.agentID.x); 

	//vec4 navi_color =  imageArrayBilinear(gNaviTexArray, FSin.TexCoord.xy, groupid_input);
	//vec4 pre_navi_color = imageLoad(gMotionBufferNoCollision, agentid_input); //only need that direction

	//if(pre_navi_color.x == -99.0f&&pre_navi_color.y == -99.0f&&pre_navi_color.z == -99.0f&&pre_navi_color.w == -99.0f)
	//{
	//	imageStore(gMotionBufferNoCollision, agentid_input, vec4(navi_color));
	//}
	//else
	//{
	//	imageStore(gMotionBufferNoCollision, agentid_input, vec4(pre_navi_color));
	//}


	if(s_tuned == 0.0f)
	{
	   imageStore(gMotionBufferNoCollision, agentid_input, vec4(groupid_input, agentid_input, vec2(0.f)));
       discard; 
    }

	int i=0; 
    vec2 offset= vec2(0.0); 
	float offset_yaxis = 0.0f; 
    vec2 testcollision_pos; 
    vec4 testcollision_agent; 
	uvec2 testcollision_agentID; 
	unsigned int testcollisionagentID_offset;
	vec2 agent_radius = vec2(7.2928/1600); 
    float newindexer_agent; 
	int coords_outside = 0; 
	int ct_outside = 0; 
	int smallmotionvec = 0; 
	
	//because of clamping
	vec2 realmotionvec = agent_i_nextstep - FSin.TexCoord.xy; 
	float realmotion_mag = length(realmotionvec);

	//
	for(i = 0; i< 16; i++)
	{
		offset = possiondiscReOrdered[i]; 
		offset_yaxis = possiondiscReOrdered[i].y*agent_radius + realmotion_mag; 
		if(offset_yaxis < 0.0f) //small motion mag 
		{
			smallmotionvec = 1; 
			continue; 
		}

        offset = TransformPossion(offset, motionvec);  //if s == 0 , need 

        testcollision_pos = agent_i_nextstep + offset * agent_radius; 
		coords_outside = outsideCoordinates(testcollision_pos); 

		if(coords_outside == 1)
		{
		    ct_outside++; 
			continue; 
		}
        testcollision_agent = texture(gAgentVoronoiMap, testcollision_pos).rgba;
		testcollision_agentID = uvec2(testcollision_agent.r * 256 * 256 * 256 + testcollision_agent.g * 256 * 256 + testcollision_agent.b * 256);
		testcollisionagentID_offset = uvec2(floor(testcollision_agentID.x/256.0f)).x; 
		testcollision_agentID = uvec2(testcollision_agentID.x - testcollisionagentID_offset); 

        if(testcollision_agentID.x != markerAgentID.x)
		{
		   //collision exist
           break;
        }
    }
	//
    if(i == 16)
	{
       gl_FragColor.rgba = vec4(1,0, 0,0.0);
    }
	else
	{
	   vec2 translate = possiondiscReOrdered[i]; 
	   float distance; 
	   float motion_mag = s_tuned; 
	   if(translate.y >= 0)
	   { 
	      distance = agent_radius.x  - abs(translate.y) * agent_radius.x; 
		  motion_mag -= distance; 
		  motion_mag = motion_mag < 0? 0.0f:motion_mag; 
		  motionvec = normalize(motion_agent.rg) * motion_mag; 
	   }
	   else
	   {
	      //long offset
		  distance = agent_radius.x - abs(translate.y) * agent_radius.x;
		  motion_mag -= distance; 
		  motion_mag = motion_mag < 0? 0.0f:motion_mag; 
		  motionvec = normalize(motion_agent.rg) * motion_mag; 
	   }
	   gl_FragColor.rgba = vec4(1,1, 0,0.0);
    }

	//offset = possiondiscReOrdered[0]; 
    //offset = TransformPossion(offset, normalize(motion_agent.rg));  //if s == 0 , need 

	//final clamp: 
	////imageStore(gMotionBufferNoCollision, agentid_input, vec4(motion_agent.rg, motionvec.rg));
	imageStore(gMotionBufferNoCollision, agentid_input, vec4(groupid_input, agentid_input, motionvec.rg));

	ivec2 P = ivec2(gl_FragCoord.xy); 
    ivec2 gridXY = ivec2(int(float(P.x)/cellsize.x), int(float(P.y)/cellsize.y));  
	int index_id = gridXY.x + 64 * gridXY.y; 

	//uint density_value = imageLoad(gDensityField, gridXY).x;
	//density_value++; 
	//imageStore(gDensityField, gridXY, uvec4(density_value, gridXY,3));


	//float density_value_buffer = imageLoad(gDensityFieldBuffer, agentid_input).x;
	//density_value_buffer+= 1.0f;
	int valueold_image = imageAtomicAdd(gDensityField, gridXY, 1); 
	int valueold = imageAtomicAdd(gDensityFieldBuffer, index_id, 1); 
	//imageStore(gDensityFieldBuffer, index_id, );
};


shader FSmain2(in VSOutput FSin, out vec4 FragColor)
{

	 ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	 vec4 sample_agent = texture(gAgentVoronoiMap, FSin.TexCoord.xy); 
	 ivec2 indexer_agent = ivec2(dot(sample_agent, bits)); 

	 vec2 agent_i = FSin.TexCoord.xy; 
	 vec2 agent_Texelcoord = agent_i * Size;
	 ivec2 agent_TexelIndex = ivec2(agent_Texelcoord);
	 ivec2 agent_TexelIndex2 = ivec2(gl_FragCoord.xy - 0.5); 

	 //float agentradiustest = FSin.WeightsCombo.a ; 
	 float agentradiustest = 0.004558; 

	 //old glsl
	 //vec4 motion_agent = texture(gMotionMap, FSin.TexCoord.xy); //motion vector current step

	// vec4 motion_agent = texelFetch(gMotionMap, agent_TexelIndex, 0); 
	 vec4 motion_agent = texelFetch(gMotionMap, agent_TexelIndex2, 0); 
  	 motion_agent.x = motion_agent.b == 1.0? -motion_agent.x  : motion_agent.x; 
	 motion_agent.y = motion_agent.a == 1.0? -motion_agent.y  : motion_agent.y; 

	 //motion_agent.r = unpackFloatFromVec4i(vec4(0, 0, motion_agent.rg)) * 0.1;
	 //motion_agent.g = unpackFloatFromVec4i(vec4(0, 0, motion_agent.ba)) * 0.1;

	 ivec2 idvec = ivec2(FSin.agentID.x, FSin.agentID.y);




	 vec2 motionvec = motion_agent.rg; 
	 vec2 motionvecwithcollison = motionvec; 
	 float motionmagnitutue = length(motionvec);
	 float agentmaxs = FSin.agentID.b;
	 float s = min(motionmagnitutue, agentmaxs); 

     motionvec = normalize(motionvec)*s*0.1; 

	 vec2 agent_i_nextstep = agent_i + motionvec; 
	 motionvecwithcollison = motionvec;
	 


	// imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent ));

	 if(motion_agent.rg == vec2(0.0f)){
	 	   imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, 0.0f, 0.0f));
		   discard; 
	 }

	 if(s == 0.0f){
	   imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, 0.0f, 0.0f));
       discard; 
     }

	 int i=0; 
     vec2 offset= vec2(0.0); 
     vec2 newsamplingpos = vec2(0,0); 
	 vec2 newsamplingpos_Texelcoord;
	 ivec2 newsamplingpos_TexelIndex;

     vec4 newsample_agent = vec4(0,0,0,1); 
     ivec2 newindexer_agent; 

	 for(i = 0; i< 12; i++){
		offset = possiondiscReOrdered[i]; 
       // offset = offset * step; 
        offset = TransformPossion(offset, motionvec);  //if s == 0 , need 

        newsamplingpos = agent_i_nextstep + offset * agentradiustest; 
		newsamplingpos_Texelcoord = newsamplingpos*Size.xy; 
		newsamplingpos_TexelIndex = ivec2(newsamplingpos_Texelcoord);
		newindexer_agent = ivec2(dot(texelFetch(gAgentVoronoiMap, newsamplingpos_TexelIndex, 0), bits)); 

       // newsample_agent = texture(gAgentVoronoiMap, newsamplingpos);
        if(indexer_agent.r != newindexer_agent.r)
		{
              break;
        }
    }
	//
    if(i ==12){

       gl_FragColor.rgba = vec4(1,0, 0,0.0);
	   if(s > 0){
	   imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motionvec.xy));
	   }
	  // imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));

    }
	else{
	    
		   vec2 translate = possiondiscReOrdered[i]; 
		   float distance; 
		   if(translate.y >= 0)
		   { 
				distance = agentradiustest - abs(translate.y) * agentradiustest ; 
				motionvec = motionvec - abs(distance) * (motionvec);
		   }
		   else{
				distance = agentradiustest + abs(translate.y) * agentradiustest;
				motionvec = motionvec - abs(distance ) * (motionvec);
		   }


       
			gl_FragColor.rgba = vec4(1,1, 0,0.0);


		   // motionvec.xy= motionvec.xy - motionvecwithcollison; 
	        //imageStore(gMotionBufferNoCollision, idvec.y, vec4(i, groupid, motion_agent.xy));
			//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));
			if(s > 0){
			    imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motionvec));
			}
			
			//imageStore(gMotionBufferNoCollision, idvec.y, vec4(motionvecwithcollison.xy, motion_agent.xy));
    }

	//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));

	//motionvecwithcollison = vec2(-0.01, 0.01); 
	//imageStore(gMotionBufferNoCollision, idvec.y, vec4(motionvecwithcollison, idvec.x, idvec.y));
};

shader FSmain3(in VSOutput FSin, out vec4 FragColor)
{

	 ivec2 Size = textureSize(gAgentVoronoiMap, 0).xy;
	 vec4 sample_agent = texture(gAgentVoronoiMap, FSin.TexCoord.xy); 
	 ivec2 indexer_agent = ivec2(dot(sample_agent, bits)); 

	 vec2 agent_i = FSin.TexCoord.xy; 
	 vec2 agent_Texelcoord = agent_i * Size;
	 ivec2 agent_TexelIndex = ivec2(agent_Texelcoord);
	 ivec2 agent_TexelIndex2 = ivec2(gl_FragCoord.xy - 0.5); 

	 //float agentradiustest = FSin.WeightsCombo.a ; 
	 float agentradiustest = 0.004558; 

	 //old glsl
	 //vec4 motion_agent = texture(gMotionMap, FSin.TexCoord.xy); //motion vector current step

	// vec4 motion_agent = texelFetch(gMotionMap, agent_TexelIndex, 0); 
	 vec4 motion_agent = texelFetch(gMotionMap, agent_TexelIndex2, 0); 
  	 motion_agent.x = motion_agent.b == 1.0? -motion_agent.x  : motion_agent.x; 
	 motion_agent.y = motion_agent.a == 1.0? -motion_agent.y  : motion_agent.y; 

	 //motion_agent.r = unpackFloatFromVec4i(vec4(0, 0, motion_agent.rg)) * 0.1;
	 //motion_agent.g = unpackFloatFromVec4i(vec4(0, 0, motion_agent.ba)) * 0.1;

	 ivec2 idvec = ivec2(FSin.agentID.x, FSin.agentID.y);


	 vec2 motionvec = motion_agent.rg; 
	 vec2 motionvecwithcollison = motionvec; 
	 float motionmagnitutue = length(motionvec);
	 float agentmaxs = FSin.agentID.b;
	 float s = min(motionmagnitutue, agentmaxs); 

	 //
	 vec2 navigation_dir = FSin.NavigatonDir.rg; 
	 int groupID = ivec2(FSin.agentID.x).r; 
	 vec4 navi_color =  imageArrayBilinear(gNaviTexArray, FSin.TexCoord.xy, groupID);
	 //navigation_dir.x = navi_color.b == 1? navi_color.r : -navi_color.r; 
	 //navigation_dir.y = navi_color.a == 1? navi_color.g : -navi_color.g; 
	 navigation_dir.xy = navi_color.rg; 

	// if(navigation_dir.x == 0 && navigation_dir.y == 0){
		//motionvec = normalize(motionvec) *s*0.1; 
	// }

	 //motionvec = normalize(motionvec) *s*0.1; 

	 //motionvec = normalize(0.0*normalize(motionvec) + 1*normalize(navigation_dir)) *s*0.1; 
	 
	 motionvec = 1.0*normalize(motionvec) * s * 0.1 + 0.0001*normalize(navigation_dir) * agentmaxs * 0.01;

	 
	 vec4 previousMotion = imageLoad(gMotionBufferNoCollisionPre, idvec.y);
	 motionvec += 0.000018 * previousMotion.zw * agentmaxs * 0.01;

	 vec2 agent_i_nextstep = agent_i + motionvec; 
	 motionvecwithcollison = motionvec;
	 


	// imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, previousMotion.zw ));

	 if(motion_agent.rg == vec2(0.0f)){
	 	   imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, 0.0f, 0.0f));
		   discard; 
	 }

	 if(s == 0.0f){
	   imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, 0.0f, 0.0f));
       discard; 
     }

	 int i=0; 
     vec2 offset= vec2(0.0); 
     vec2 newsamplingpos = vec2(0,0); 
	 vec2 newsamplingpos_Texelcoord;
	 ivec2 newsamplingpos_TexelIndex;

     vec4 newsample_agent = vec4(0,0,0,1); 
     ivec2 newindexer_agent; 

	 for(i = 0; i< 12; i++){
		offset = possiondiscReOrdered[i]; 
       // offset = offset * step; 
        offset = TransformPossion(offset, motionvec);  //if s == 0 , need 

        newsamplingpos = agent_i_nextstep + offset * agentradiustest; 
		newsamplingpos_Texelcoord = newsamplingpos*Size.xy; 
		newsamplingpos_TexelIndex = ivec2(newsamplingpos_Texelcoord);
		newindexer_agent = ivec2(dot(texelFetch(gAgentVoronoiMap, newsamplingpos_TexelIndex, 0), bits)); 

       // newsample_agent = texture(gAgentVoronoiMap, newsamplingpos);
        if(indexer_agent.r != newindexer_agent.r)
		{
              break;
        }
    }
	//
    if(i ==12){

       gl_FragColor.rgba = vec4(1,0, 0,0.0);
	   if(s > 0){
		 imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motionvec.xy));
	   }
	  // imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));

    }
	else{
	    
		//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motionvec));

		   vec2 translate = possiondiscReOrdered[i]; 
		   float distance; 
		   if(translate.y >= 0)
		   { 
				distance = agentradiustest + abs(translate.y) * agentradiustest ; 
				motionvec = motionvec - abs(distance) * (motionvec);
		   }
		   else{
				distance = agentradiustest - abs(translate.y) * agentradiustest;
				motionvec = motionvec - abs(distance ) * (motionvec);
		   }
       
			gl_FragColor.rgba = vec4(1,1, 0,0.0);

		   // motionvec.xy= motionvec.xy - motionvecwithcollison; 
	        //imageStore(gMotionBufferNoCollision, idvec.y, vec4(i, groupid, motion_agent.xy));
			//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));

			if(s > 0){
			    imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motionvec));
				//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, vec2(0.0f)));
			}
			
			//imageStore(gMotionBufferNoCollision, idvec.y, vec4(motionvecwithcollison.xy, motion_agent.xy));
    }

	//imageStore(gMotionBufferNoCollision, idvec.y, vec4(idvec.xy, motion_agent.xy));

	//motionvecwithcollison = vec2(-0.01, 0.01); 
	//imageStore(gMotionBufferNoCollision, idvec.y, vec4(motionvecwithcollison, idvec.x, idvec.y));
};

program CollisionAvoidanceTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
	//fs(430) = FSmain2();
	//fs(430) = FSmain3();
};