#version 150 compatibility 

uniform mat4 uModelViewProjectionMatrix;
out vec4 vColor;
in vec4 aVertex;
in vec4 aColor;

void main()
{
	vColor = aColor;
	gl_Position = uModelViewProjectionMatrix * aVertex;
}