#version 150 compatibility 

uniform mat4 uModelViewProjectionMatrix;
out vec4 vColor;
in vec4 aPosition;
in vec4 aColor;
in vec4 aNormal;
in vec4 aTexCoord0;
out vec2 vST;

uniform float timeElapsed;
uniform bool teapot_esfera;
uniform bool teapot_cvertices;
uniform mat4 trans;

void main()
{
	vec4 origin = vec4(0,0,0,0);

	if (teapot_cvertices)
	{
		vColor = abs(aNormal) * trans;
	}
	else
	{
		vColor = abs(aNormal);
	}
	if (teapot_esfera)
	{
		gl_Position = uModelViewProjectionMatrix * vec4(mix(aPosition.x, aNormal.x * 3, abs(sin(timeElapsed/1000.0f))), mix(aPosition.y, aNormal.y * 3, abs(sin(timeElapsed/1000.0f))), mix(aPosition.z, aNormal.z * 3, abs(sin(timeElapsed/1000.0f))), 1);
		
	}
	else
	{
		gl_Position = uModelViewProjectionMatrix * aPosition;
	}
	
	vST = aTexCoord0.st;
	
}