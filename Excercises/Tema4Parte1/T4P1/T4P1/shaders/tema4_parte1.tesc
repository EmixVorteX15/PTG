#version 400

layout (vertices = 4) out;

const int uTessLevel = 4;

const float uMinDepth = 3.0;
const float uMaxDepth = 8.0;
const float uMinTessLevel = 2.0;
const float uMaxTessLevel = 10.0; // 64.0
in vec3 vColor[];
out vec3 tcColor[];

uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjMatrix;
uniform int uTessVariable;

float level (in vec4 pos)
{
	vec4 p = uModelViewMatrix * pos;
	float depth = clamp( (abs(p.z) - uMinDepth) / (uMaxDepth - uMinDepth), 0.0, 1.0);
	return mix( uMaxTessLevel, uMinTessLevel, depth );
}

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	float tessLevel;

	if(uTessVariable%4 == 0){
		tessLevel = float(uTessLevel);
	}else if (uTessVariable%4 == 1){
		vec4 pos = vec4(0.0,0.0,0.0,1.0);
		tessLevel = level(pos);
	}else if (uTessVariable%4 == 2 || uTessVariable%4 == 3){
		vec4 pos = gl_in[gl_InvocationID].gl_Position;
		tessLevel = level(pos);
	}
	
	// Asignacion del nivel externo de teselacion
	if (uTessVariable%4 == 3){
		
		gl_TessLevelOuter[0] = level((gl_in[0].gl_Position + gl_in[3].gl_Position)*0.5);
		gl_TessLevelOuter[1] = level((gl_in[0].gl_Position + gl_in[1].gl_Position)*0.5);
		gl_TessLevelOuter[2] = level((gl_in[1].gl_Position + gl_in[2].gl_Position)*0.5);
		gl_TessLevelOuter[3] = level((gl_in[2].gl_Position + gl_in[3].gl_Position)*0.5);
	}else{
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
		gl_TessLevelOuter[3] = tessLevel;
	}
	

	// Asignacion del nivel interno de teselacion
	gl_TessLevelInner[0] = tessLevel;
	gl_TessLevelInner[1] = tessLevel;


	tcColor[gl_InvocationID] = vColor[gl_InvocationID];
}