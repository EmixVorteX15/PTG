#version 400


layout (quads) in;
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

out vec3 vECPos;
out vec3 vECNorm;
out vec3 vColor;
in vec3 tcColor[];

void main()
{
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	vec4 p_inferior = p1 - p0;
	vec4 p_izquierda = p3 - p0;

	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 pos = (p0+ p_inferior*u + p_izquierda*v);
	vec3 normal = normalize(pos.xyz);
	gl_Position = uModelViewProjMatrix * vec4(normal,1.0);

	vECPos = vec3(uModelViewMatrix * vec4(normal,1.0));
	vECNorm = uNormalMatrix * normal;
	vColor = tcColor[0];
}
