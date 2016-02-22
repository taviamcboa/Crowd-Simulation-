

layout (binding = 0, std430)  buffer GuidanceField01
{
  vec4 guidanceField[];

} GuidanceFieldBuffer01;

layout (binding = 1, std430)  buffer NaviField01
{
  vec4 naviField[];

} NaviFieldBuffer01;

layout (binding = 2, std430)  buffer GuidanceField23
{
  vec4 guidanceField[];

} GuidanceFieldBuffer23;

layout (binding = 3, std430)  buffer NaviField23
{
  vec4 naviField[];

} NaviFieldBuffer23;

layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba8) uniform image2D gNaviMaskImage;

layout (binding = 1, rgba32f)  uniform image2DArray gNaviFieldImage; 
layout (binding = 2, rgba32f)  uniform image2DArray gGuidanceFieldImage; 

layout (binding = 3, rgba32f)  uniform image2DArray gMaskedNaviFieldImage; 
layout (binding = 4, rgba32f)  uniform image2DArray gMaskedGuidanceFieldImage; 


 
shader CSmain()
{	
    //128 * 128
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 
	ivec2 p2 = ivec2(p * 0.5); 
	ivec3 sizeofnavi = imageSize(gNaviFieldImage);
	ivec3 sizeofguid = imageSize(gGuidanceFieldImage); 

	vec4 mask = imageLoad(gNaviMaskImage, p);
	vec4 navi_0, navi_1, navi_2, navi_3; 
	vec4 guid_0, guid_1, guid_2, guid_3; 

	navi_0 = navi_1 = navi_2 = navi_3 = vec4(0.0f, 0, 0, 0); 
    guid_0 = guid_1 = guid_2 = guid_3 = vec4(0.0f, 0, 0, 0); 

	imageStore(gMaskedNaviFieldImage, ivec3(p, 0), navi_0); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 1), navi_1); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 2), navi_2); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 3), navi_3); 

	imageStore(gMaskedGuidanceFieldImage, ivec3(p, 0), guid_0); 
	imageStore(gMaskedGuidanceFieldImage, ivec3(p, 1), guid_1); 
	imageStore(gMaskedGuidanceFieldImage, ivec3(p, 2), guid_2); 
	imageStore(gMaskedGuidanceFieldImage, ivec3(p, 3), guid_3); 

	barrier(); 

	//if(fract(p * 0.5).x == 0.0f)
	//{
		if(mask.r < 0.75)
		{
			navi_0 = navi_1 = navi_2 = navi_3 = vec4(0.0f); 
			guid_0 = guid_1 = guid_2 = guid_3 = vec4(0.0f); 
		}
		else
		{
			//current guidance field. compare the y axis 
			navi_0 = imageLoad(gNaviFieldImage, ivec3(p2, 0)); 
			navi_0.x = ( navi_0.z == 1.0f ? navi_0.x : -navi_0.x); 
			navi_0.y = ( navi_0.w == 1.0f ? navi_0.y : -navi_0.y); 
			navi_0.zw = vec2(0.0f); 

			navi_1 = imageLoad(gNaviFieldImage, ivec3(p2, 1)); 
			navi_1.x = ( navi_1.z == 1.0f ? navi_1.x : -navi_1.x); 
			navi_1.y = ( navi_1.w == 1.0f ? navi_1.y : -navi_1.y); 
			navi_1.zw = vec2(0.0f); 

			navi_2 = imageLoad(gNaviFieldImage, ivec3(p2, 2)); 
			navi_2.x = ( navi_2.z == 1.0f ? navi_2.x : -navi_2.x); 
			navi_2.y = ( navi_2.w == 1.0f ? navi_2.y : -navi_2.y); 
			navi_2.zw = vec2(0.0f); 

			navi_3 = imageLoad(gNaviFieldImage, ivec3(p2, 3)); 
			navi_3.x = ( navi_3.z == 1.0f ? navi_3.x : -navi_3.x); 
			navi_3.y = ( navi_3.w == 1.0f ? navi_3.y : -navi_3.y); 
			navi_3.zw = vec2(0.0f); 

			guid_0 = imageLoad(gGuidanceFieldImage, ivec3(p2, 0)); 
			guid_0.x = ( guid_0.z == 1.0f ? guid_0.x : -guid_0.x); 
			guid_0.y = ( guid_0.w == 1.0f ? guid_0.y : -guid_0.y); 
			guid_0.zw = vec2(0.0f); 

			guid_1 = imageLoad(gGuidanceFieldImage, ivec3(p2, 1)); 
			guid_1.x = ( guid_1.z == 1.0f ? guid_1.x : -guid_1.x); 
			guid_1.y = ( guid_1.w == 1.0f ? guid_1.y : -guid_1.y); 
			guid_1.zw = vec2(0.0f); 

			guid_2 = imageLoad(gGuidanceFieldImage, ivec3(p2, 2)); 
			guid_2.x = ( guid_2.z == 1.0f ? guid_2.x : -guid_2.x); 
			guid_2.y = ( guid_2.w == 1.0f ? guid_2.y : -guid_2.y); 
			guid_2.zw = vec2(0.0f); 

			guid_3 = imageLoad(gGuidanceFieldImage, ivec3(p2, 3)); 
			guid_3.x = ( guid_3.z == 1.0f ? guid_3.x : -guid_3.x); 
			guid_3.y = ( guid_3.w == 1.0f ? guid_3.y : -guid_3.y); 
			guid_3.zw = vec2(0.0f); 
		}
	//}

	barrier(); 

   // imageStore(gMaskedGuidanceFieldImage, ivec3(p, 0), navi_0); 
	//imageStore(gMaskedGuidanceFieldImage, ivec3(p, 1), navi_1); 
	//imageStore(gMaskedGuidanceFieldImage, ivec3(p, 2), navi_2); 
	//imageStore(gMaskedGuidanceFieldImage, ivec3(p, 3), navi_3); 

	imageStore(gMaskedNaviFieldImage, ivec3(p, 0), guid_0); 
    imageStore(gMaskedNaviFieldImage, ivec3(p, 1), guid_1); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 2), guid_2); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 3), guid_3); 

	barrier(); 

	vec4 guidW_0, guidE_0, guidM_0; 
	vec4 guidW_1, guidE_1, guidM_1; 
	vec4 guidW_2, guidE_2, guidM_2; 
	vec4 guidW_3, guidE_3, guidM_3; 
	float maskW, maskE, maskM; 

	maskW = imageLoad(gNaviMaskImage, p+ivec2(-1,0)).r;
	maskE = imageLoad(gNaviMaskImage, p+ivec2(1,0)).r;

	//group-0
	guidW_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 0)); 
	guidE_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 0)); 
	guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 


    //group-1
	guidW_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 1)); 
	guidE_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 1)); 
	guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 


	//group-2
	guidW_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 2)); 
	guidE_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 2)); 
	guidM_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 


	//group-3
	guidW_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 3)); 
	guidE_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 3)); 
	guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 


	if(length(guidM_0) > 0.9f && length(guidE_0.xy) == 0.0f)
	{
		guidM_0.x *= 0.0f; 
		guidM_0.y = 1.0f ; 
	}
	else
	{
		guidM_0.x *= 0.0f; 
		guidM_0.y = 0.0f ; 
	}
	if(length(guidM_1) > 0.9f && length(guidW_1.xy) == 0.0f)
	{
		guidM_1.x *= 0.0f; 
		guidM_1.y = -1.0f ; 
	}
	else
	{
		guidM_1.x *= 0.0f; 
		guidM_1.y = 0.0f ; 
	}

	if(length(guidM_2) > 0.9f && length(guidW_2.xy) == 0.0f)
	{
		guidM_2.x *= 0.0f; 
		guidM_2.y = -1.0f ; 
	}
	else
	{
		guidM_2.x *= 0.0f; 
		guidM_2.y = 0.0f ; 
	}
	if(length(guidM_3) > 0.9f && length(guidE_3.xy) == 0.0f)
	{
		guidM_3.x *= 0.0f; 
		guidM_3.y = 1.0f ; 
	}
	else
	{
		guidM_3.x *= 0.0f; 
		guidM_3.y = 0.0f ; 
	}
	
	 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 0), guidM_0); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 1), guidM_1); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 2), guidM_2); 
	imageStore(gMaskedNaviFieldImage, ivec3(p, 3), guidM_3); 

	barrier();

	//propate the ajacent
	vec4 guidN_0, guidS_0;
	vec4 guidN_1, guidS_1; 
	vec4 guidN_2, guidS_2; 
	vec4 guidN_3, guidS_3; 
	float maskN, maskS; 

	for(int i=0; i<15; i++)
	{
		barrier();
		guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 
		guidN_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1), 0)); 
		guidS_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1), 0)); 

		guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 
		guidN_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1), 1)); 
		guidS_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1), 1)); 

		guidM_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 
		guidN_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1), 2)); 
		guidS_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1), 2)); 

		guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 
		guidN_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1), 3)); 
		guidS_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1), 3)); 

		maskN = imageLoad(gNaviMaskImage, p + ivec2(0,1)).r;
		maskS = imageLoad(gNaviMaskImage, p + ivec2(0,-1)).r;

		if(length(guidM_3.xy) == 1 )
		{
			if(maskS > 0.75f)
				imageStore(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1),3), guidM_3); 
			barrier();
		}

		if(length(guidM_0.xy) == 1 )
		{
			if(maskS > 0.75f)
				imageStore(gMaskedNaviFieldImage, ivec3(p + ivec2(0,-1),0), guidM_0); 
			barrier();
		}

		if(length(guidM_1.xy) == 1 )
		{
			if(maskN > 0.75f)
				imageStore(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1),1), guidM_1); 
			barrier();
		}
		if(length(guidM_2.xy) == 1 )
		{
			if(maskN > 0.75f)
				imageStore(gMaskedNaviFieldImage, ivec3(p + ivec2(0,1),2), guidM_2); 
			barrier();
		}
	}
	//Group 0 Propogating
	guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 
	guidE_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 0)); 
	if(length(guidE_0.xy) == 1)
	{
		guidM_0 = vec4(1,1,0,0); 
		guidM_0.xy = normalize(guidM_0.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,0), guidM_0); 
	barrier();

	//propogate Second-left neighboring
	guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 
	guidE_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 0)); 
	if(guidE_0.xy == normalize(vec2(1, 1)))
	{
		guidM_0 = vec4(2,1,0,0); 
		guidM_0.xy = normalize(guidM_0.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,0), guidM_0); 
	barrier();
	
	//propogate Third-left neighboring
	guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 
	guidE_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 0)); 
	if(guidE_0.xy == normalize(vec2(2, 1)))
	{
		guidM_0 = vec4(3,1,0,0); 
		guidM_0.xy = normalize(guidM_0.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,0), guidM_0); 
	barrier();

	//propogate Last-left neighboring
	guidM_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 0)); 
	guidE_0 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 0)); 
	if(guidE_0.xy == normalize(vec2(3, 1)))
	{
		guidM_0 = vec4(1,0,0,0); 
		guidM_0.xy = normalize(guidM_0.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,0), guidM_0); 
	barrier();

	//Group 1 Propogating
	guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 
	guidW_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 1)); 
	if(length(guidW_1.xy) == 1)
	{
		guidM_1 = vec4(-1,-1,0,0); 
		guidM_1.xy = normalize(guidM_1.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,1), guidM_1); 
	barrier();

	//propogate Second-left neighboring
	guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 
	guidW_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 1)); 
	if(guidW_1.xy == normalize(vec2(-1, -1)))
	{
		guidM_1 = vec4(-2,-1,0,0); 
		guidM_1.xy = normalize(guidM_1.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,1), guidM_1); 
	barrier();
	
	//propogate Third-left neighboring
	guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 
	guidW_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 1)); 
	if(guidW_1.xy == normalize(vec2(-2, -1)))
	{
		guidM_1 = vec4(-3,-1,0,0); 
		guidM_1.xy = normalize(guidM_1.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,1), guidM_1); 
	barrier();

	//propogate Last-left neighboring
	guidM_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 1)); 
	guidW_1 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 1)); 
	if(guidW_1.xy == normalize(vec2(-3, -1)))
	{
		guidM_1 = vec4(-1,0,0,0); 
		guidM_1.xy = normalize(guidM_1.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,1), guidM_1); 
	barrier();


	//Group 2 Propogating
	guidM_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 
	guidW_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0),2)); 
	if(length(guidW_2.xy) == 1)
	{
		guidM_2 = vec4(-1,-1,0,0); 
		guidM_2.xy = normalize(guidM_2.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,2), guidM_2); 
	barrier();

	//propogate Second-left neighboring
	guidM_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 
	guidW_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 2)); 
	if(guidW_2.xy == normalize(vec2(-1, -1)))
	{
		guidM_2 = vec4(-2,-1,0,0); 
		guidM_2.xy = normalize(guidM_2.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,2), guidM_2); 
	barrier();
	
	//propogate Third-left neighboring
	guidM_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 
	guidW_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 2)); 
	if(guidW_2.xy == normalize(vec2(-2, -1)))
	{
		guidM_2 = vec4(-3,-1,0,0); 
		guidM_2.xy = normalize(guidM_2.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,2), guidM_2); 
	barrier();

	//propogate Last-left neighboring
	guidM_2= imageLoad(gMaskedNaviFieldImage, ivec3(p, 2)); 
	guidW_2 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(-1,0), 2)); 
	if(guidW_2.xy == normalize(vec2(-3, -1)))
	{
		guidM_2 = vec4(-1,0,0,0); 
		guidM_2.xy = normalize(guidM_2.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,2), guidM_2); 
	barrier();


	//propogate left neighboring
	guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 
	guidE_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 3)); 
	if(length(guidE_3.xy) == 1)
	{
		guidM_3 = vec4(1,1,0,0); 
		guidM_3.xy = normalize(guidM_3.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,3), guidM_3); 
	barrier();

	//propogate Second-left neighboring
	guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 
	guidE_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 3)); 
	if(guidE_3.xy == normalize(vec2(1, 1)))
	{
		guidM_3 = vec4(2,1,0,0); 
		guidM_3.xy = normalize(guidM_3.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,3), guidM_3); 
	barrier();
	
	//propogate Third-left neighboring
	guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 
	guidE_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 3)); 
	if(guidE_3.xy == normalize(vec2(2, 1)))
	{
		guidM_3 = vec4(3,1,0,0); 
		guidM_3.xy = normalize(guidM_3.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,3), guidM_3); 
	barrier();

	//propogate Last-left neighboring
	guidM_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p, 3)); 
	guidE_3 = imageLoad(gMaskedNaviFieldImage, ivec3(p + ivec2(1,0), 3)); 
	if(guidE_3.xy == normalize(vec2(3, 1)))
	{
		guidM_3 = vec4(1,0,0,0); 
		guidM_3.xy = normalize(guidM_3.xy);
	}
	barrier();
	if(mask.r > 0.75f)
		imageStore(gMaskedNaviFieldImage, ivec3(p,3), guidM_3); 
	barrier();
	
	int Offset = p.y * 128 + p.x; 
	GuidanceFieldBuffer01.guidanceField[Offset].xy = vec2(guidM_0.xy); 
	GuidanceFieldBuffer01.guidanceField[Offset].zw = vec2(guidM_1.xy); 
	GuidanceFieldBuffer23.guidanceField[Offset].xy = vec2(guidM_2.xy); 
	GuidanceFieldBuffer23.guidanceField[Offset].zw = vec2(guidM_3.xy); 

	NaviFieldBuffer01.naviField[Offset].xy = vec2(navi_0.xy); 
	NaviFieldBuffer01.naviField[Offset].zw = vec2(navi_1.xy); 
	NaviFieldBuffer23.naviField[Offset].xy = vec2(navi_2.xy); 
	NaviFieldBuffer23.naviField[Offset].zw = vec2(navi_3.xy); 



}

program MaskNaviGuidTechnique
{
	cs(430) = CSmain(); 
};