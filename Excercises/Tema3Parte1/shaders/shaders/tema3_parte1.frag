#version 150 

uniform sampler2D uStoneTex; 
uniform sampler2D uMossTex;
uniform samplerCube uTexMap; 
uniform int uDrawSky;

out vec4 fFragColor;

void main()
{
	fFragColor = vec4(1.0, 0.0, 0.0, 1.0);	
}
