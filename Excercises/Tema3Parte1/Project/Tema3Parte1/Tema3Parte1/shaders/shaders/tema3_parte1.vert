#version 150  

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform int uDrawSky;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

void main()
{
	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}
