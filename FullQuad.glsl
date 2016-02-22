struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
};

interface VSOutput
{
    vec2 TexCoord;
};

uniform sampler2D gColorMap;    
  
layout(binding = 0, r32i) uniform coherent iimage2D gDensityImage;
layout(binding = 1, rgba8i) uniform coherent iimage2D gAntiGradientImage;

uniform sampler2D gDensityVelocityTexture; 

uniform sampler2DArray gGuidTexureArray;     //navigation 
uniform sampler2DArray gNaviTexureArray;       //previous Pass
 
uniform float gIntegerMap; // 0: normal, 1: density map, 2: graident map, 3: test gradient map;
uniform int gIntegerNaviGuidance; 
uniform int gIntegerGuidance; 
uniform int gIntegerNavi; 
uniform int gGroupID; 

const vec2 cellsize = vec2(25.0, 14.0625); 

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


vec4 VisualizeVector(in vec2 direction)
{
	vec4 ret; 
	if(direction.x >=0 && direction.y >0)
	{
		ret = vec4(1, 0,0 ,0); 
	}
	else if(direction.x <0 && direction.y>=0)
	{
		ret = vec4(0, 1, 0,0);
	}
	else if(direction.x <=0 && direction.y <0)
	{
		ret = vec4(0,0,1,0); 
	}
	else if(direction.x >=0 && direction.y <= 0)
	{
		ret = vec4(1,1,0,0); 
	}
	return ret; 
}


shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
    VSout.TexCoord   = VSin.TexCoord;   
    gl_Position = vec4(VSin.Position.x, VSin.Position.y, VSin.Position.z, 1.0); 
};


shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	 if(gIntegerMap == 0.0f)
		FragColor = vec4(texture(gColorMap, FSin.TexCoord.xy).rgb, 1);   
	 else if(gIntegerMap == 1.0f)
	 {
		ivec2 fragcoords = ivec2(gl_FragCoord.xy);

		ivec2 gridXY = ivec2(int(float(fragcoords.x)/cellsize.x), int(float(fragcoords.y)/cellsize.y));  

		int density_value = imageLoad(gDensityImage, gridXY).r; 

		FragColor = vec4(density_value * 0.2); 
	}
	 else if(gIntegerMap == 3.0f)
	 {
		ivec2 fragcoords = ivec2(gl_FragCoord.xy);

		ivec2 gridXY = ivec2(fragcoords.x/cellsize.x, fragcoords.y/cellsize.y);  

		ivec2 anti_gradient = imageLoad(gAntiGradientImage, gridXY).rg; 

		if(anti_gradient.x > 0 && anti_gradient.y > 0)
			FragColor = vec4(1,0,0,0); 
		else if(anti_gradient.x > 0 && anti_gradient.y < 0)
			FragColor = vec4(0,1,0 ,0); 
		else if(anti_gradient.x < 0 && anti_gradient.y < 0)
			FragColor = vec4(0,0,1 ,0); 
		else if(anti_gradient.x < 0 && anti_gradient.y > 0)
			FragColor = vec4(1,1,0 ,0); 
		else 
			FragColor = vec4(1,1,1,0); 
	}
	 else if(gIntegerMap == 2.0f)
	 {
		ivec2 fragcoords = ivec2(gl_FragCoord.xy);

		if(fragcoords.x > 63 || fragcoords.y > 63 )
			discard; 

		ivec2 gridXY = ivec2(int(float(fragcoords.x)/cellsize.x), int(float(fragcoords.y)/cellsize.y));  
		ivec2 gridXY0 = ivec2(0); 
		gridXY = fragcoords; 
	   
	    ivec2 offsetM = ivec2(0,0); 
		ivec2 offsetN = ivec2(0, 1); 
		ivec2 offsetE = ivec2(1, 0);
		ivec2 offsetS = ivec2(0, -1); 
		ivec2 offsetW = ivec2(-1, 0); 

		ivec2 coordsN = gridXY + offsetN; 
		ivec2 coordsE = gridXY + offsetE; 
		ivec2 coordsS = gridXY + offsetS; 
		ivec2 coordsW = gridXY + offsetW; 


		int densityM, densityN, densityE, densityS, densityW; 

		densityM = imageLoad(gDensityImage, gridXY).r; 

		if(coordsN.x < 64 && coordsN.x >=0 && coordsN.y < 64 && coordsN.y >=0)
			densityN = imageLoad(gDensityImage, coordsN).r; 
		else
			densityN = 0;

		if(coordsE.x < 64 && coordsE.x >=0 && coordsE.y < 64 && coordsE.y >=0)
			densityE = imageLoad(gDensityImage, coordsE).r; 
		else
			densityE = 0;

		if(coordsW.x < 64 && coordsW.x >=0 && coordsW.y < 64 && coordsW.y >=0)
			densityW = imageLoad(gDensityImage, coordsW).r; 
		else
			densityW = 0;

		if(coordsS.x < 64 && coordsS.x >=0 && coordsS.y < 64 && coordsS.y >=0)
			densityS = imageLoad(gDensityImage, coordsS).r; 
		else
			densityS = 0;

		int deltaX = (densityE - densityW); 
		int deltaY = (densityN - densityS); 
	
		imageStore(gAntiGradientImage, gridXY, ivec4(deltaX, deltaY, 99, -99));  
	 }
	 else if(gIntegerMap == 4.0f)
	 {
		if(gIntegerNaviGuidance == 1 && gIntegerGuidance == 1 && gIntegerNavi == 0)
		{
			vec4 guid_dir =  imageArrayBilinear(gGuidTexureArray, FSin.TexCoord.xy, gGroupID);
			FragColor = VisualizeVector(guid_dir.rg); 
		}
		else if(gIntegerNaviGuidance == 1 && gIntegerGuidance == 0 && gIntegerNavi == 1)
		{
			vec4 navi_dir =  imageArrayBilinear(gNaviTexureArray, FSin.TexCoord.xy, gGroupID);
			FragColor = VisualizeVector(navi_dir.rg); 
		}		
	 }
	 else if(gIntegerMap == 5.0f)
	 {
		ivec2 fragcoords = ivec2(gl_FragCoord.xy);
		ivec2 gridXY = ivec2(int(float(fragcoords.x)/cellsize.x), int(float(fragcoords.y)/cellsize.y));  

		int mode = gridXY.x + gridXY.y;
		if(mode % 2 == 0)
		{
			FragColor = vec4(0.2, 0.2, 0.2,0); 
		}
		else{
			FragColor = vec4(0.8, 0.8, 0.8, 0); 
		}
	 }
	 else if(gIntegerMap == 6.0f)
	 {
	    vec4 samplevalue = texture(gDensityVelocityTexture, FSin.TexCoord.xy); 
		FragColor = vec4(samplevalue * 0.1); 
		//FragColor = vec4(samplevalue); 

	//	samplevalue = samplevalue * 0.1; 
	//	FragColor = vec4(abs(samplevalue.ba), 0, 0); 
		//FragColor = vec4(abs(samplevalue.a)); 
		 
		
	 }
	 else if(gIntegerMap == 7.0f)
	 {
	    vec4 samplevalue = texture(gDensityVelocityTexture, FSin.TexCoord.xy); 
		//FragColor = vec4(samplevalue * 0.01); 
		FragColor = vec4(samplevalue.y); 

	//	samplevalue = samplevalue * 0.1; 
	//	FragColor = vec4(abs(samplevalue.ba), 0, 0); 
		//FragColor = vec4(abs(samplevalue.a)); 
		 
		
	 }
	 else if(gIntegerMap == 8.0f)
	 {
	    vec4 samplevalue = texture(gDensityVelocityTexture, FSin.TexCoord.xy); 
		FragColor = vec4(samplevalue); 

	//	samplevalue = samplevalue * 0.1; 
	//	FragColor = vec4(abs(samplevalue.ba), 0, 0); 
		//FragColor = vec4(abs(samplevalue.a)); 
		 
		
	 }
	 else if(gIntegerMap == 9.0f)
	 {
	 //EULARIAN MAP
	    vec4 samplevalue = texture(gDensityVelocityTexture, FSin.TexCoord.xy); 
		FragColor = vec4(samplevalue); 

	//	samplevalue = samplevalue * 0.1; 
	//	FragColor = vec4(abs(samplevalue.ba), 0, 0); 
		//FragColor = vec4(abs(samplevalue.a)); 
		 
		
	 }
};

program FullQuadTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};