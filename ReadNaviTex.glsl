struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
};

interface VSOutput
{
    vec2 TexCoord;
};

uniform sampler2D gNaviTex; 
uniform sampler2DArray gNaviTexArray;     
uniform float gNaviGroup; 

vec4 fetchBilinear(in sampler2D Sampler, in vec2 Interpolant, in ivec2 Texelcoords[4])
{
	vec4 Texel00 = texelFetch(Sampler, Texelcoords[0], 0);
	vec4 Texel10 = texelFetch(Sampler, Texelcoords[1], 0);
	vec4 Texel11 = texelFetch(Sampler, Texelcoords[2], 0);
	vec4 Texel01 = texelFetch(Sampler, Texelcoords[3], 0);
	
	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 fetchArrayBilinear(in sampler2DArray Sampler, in vec2 Interpolant, in ivec3 Texelcoords[4], in int gid)
{
	vec4 Texel00 = texelFetch(Sampler, Texelcoords[0], 0);
	vec4 Texel10 = texelFetch(Sampler, Texelcoords[1], 0);
	vec4 Texel11 = texelFetch(Sampler, Texelcoords[2], 0);
	vec4 Texel01 = texelFetch(Sampler, Texelcoords[3], 0);
	
	vec4 Texel0 = mix(Texel00, Texel01, Interpolant.y);
	vec4 Texel1 = mix(Texel10, Texel11, Interpolant.y);
	return mix(Texel0, Texel1, Interpolant.x);
}

vec4 imageBilinear(in sampler2D Sampler, in vec2 Texcoord)
{
	ivec2 Size = textureSize(Sampler, 0).xy;
	vec2 Texelcoord = Texcoord * Size - 0.5;
	ivec2 TexelIndex = ivec2(Texelcoord);
	
	ivec2 Texelcoords[] = ivec2[4](
		TexelIndex + ivec2(0, 0),
		TexelIndex + ivec2(1, 0),
		TexelIndex + ivec2(1, 1),
		TexelIndex + ivec2(0, 1));

	return fetchBilinear(
		Sampler, 
		fract(Texelcoord), 
		Texelcoords);
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
    VSout.TexCoord   = VSin.TexCoord;   
    gl_Position = vec4(VSin.Position.x, VSin.Position.y, VSin.Position.z, 1.0); 
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	// FragColor = vec4(texture(gColorMap, FSin.TexCoord.xy).rgb, 1);  
	FragColor = imageBilinear(gNaviTex, FSin.TexCoord.xy);
};

shader FSmainArray(in VSOutput FSin, out vec4 FragColor)
{
	// FragColor = vec4(texture(gColorMap, FSin.TexCoord.xy).rgb, 1);   

	 int groupID = ivec2(gNaviGroup).r; 
	 FragColor = imageArrayBilinear(gNaviTexArray, FSin.TexCoord.xy, groupID);
};

program ReadNaviTexTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};

program ReadNaviTexArrayTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmainArray();
};