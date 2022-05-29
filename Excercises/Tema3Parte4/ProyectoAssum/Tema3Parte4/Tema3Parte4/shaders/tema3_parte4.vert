#version 150  

in vec3 aPosition;
in vec3 aNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

uniform int uDrawEnvironment;
uniform int uLuzAmbienteHemisferio;

uniform vec3 uCameraPos;
uniform vec3 uAmbientLight;
uniform vec3 uGroundLight;
uniform vec3 uSkyLight;

uniform int uLuzImagenes;


out vec3 vNormal;
out vec3 vPosition;
out vec3 vAmbientLight; 
out vec3 vTextCoordCuboEntorno;

out vec3 vReflectDir; // Para ser usado en los reflejos del mapa de reflejos (parte especular de IBL y SPH)


void main()
{
    vec4 pos = vec4(aPosition, 1.0);

	vNormal = normalize(uNormalMatrix * aNormal);
	vPosition = vec3(uModelMatrix * pos);
	vReflectDir = reflect(vec3(vPosition) - uCameraPos, vNormal);

	if (uDrawEnvironment == 1)
		vTextCoordCuboEntorno = aPosition;
	else
		vAmbientLight = uAmbientLight;
	if (uLuzAmbienteHemisferio == 1)
	{
		vec3 tnorm = normalize(uNormalMatrix * vNormal);
		float costheta = tnorm.y;
		float a = costheta * 0.5 + 0.5;
		vec3 ambientColor = mix(uGroundLight, uSkyLight, a);
		vAmbientLight = ambientColor;
	}
		
	gl_Position = uModelViewProjMatrix * pos;
}