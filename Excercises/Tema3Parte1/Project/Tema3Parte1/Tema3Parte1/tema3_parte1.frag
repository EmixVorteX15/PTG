#version 150 core

uniform sampler2D uBrickTex;
uniform sampler2D uMossTex;
uniform samplerCube uTexMap; 
uniform int uDrawSky;
uniform int reflejo;
uniform bool cromatica;
uniform bool sky;

out vec4 fFragColor;

in vec3 vTexCoord;
in vec2 vTexCoord_vec2;
in vec3 vTexCoordR;
in vec3 vTexCoordG;
in vec3 vTexCoordB;

void main()
{
	if(reflejo%3 == 0)
	{
		vec4 brickTexColor = texture(uBrickTex, vTexCoord_vec2);
		vec4 mossTexColor = texture(uMossTex, vTexCoord_vec2);
		fFragColor = vec4( mix(brickTexColor.rgb, mossTexColor.rgb, mossTexColor.a), 1.0);
	}
	else
	{
		if(cromatica && !sky)
		{
			float texColorR = texture(uTexMap, vTexCoordR).r;
			float texColorG = texture(uTexMap, vTexCoordG).g;
			float texColorB = texture(uTexMap, vTexCoordB).b;
			fFragColor = vec4(texColorR, texColorG, texColorB, 1.0f);
		}
		else
		{
			vec4 texColor = texture(uTexMap, vTexCoord);
			fFragColor = texColor;
		}
		
	}
}