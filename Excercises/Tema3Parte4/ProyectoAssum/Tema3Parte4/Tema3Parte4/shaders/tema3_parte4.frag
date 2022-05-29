#version 150 

in vec3 vTextCoordCuboEntorno;
in vec3 vNormal;
in vec3 vPosition;
in vec3 vAmbientLight;

in vec3 vReflectDir; // Para ser usado en los reflejos del mapa de reflejos (parte especular de IBL y SPH)


uniform samplerCube uEnvironmentMap;
uniform samplerCube uDiffuseEnvMap;
uniform samplerCube uSpecularEnvMap;
uniform vec3 uCameraPos;
uniform int uDrawEnvironment;

uniform int uSpecularActivated;
uniform int uLuzAmbiente;
uniform int uLuzArmonicoEsferico;
uniform int uLuzImagenes;

// Armónicos esféricos
uniform mat4 Mred;
uniform mat4 Mgreen;
uniform mat4 Mblue;
uniform mat3 uNormalMatrix;

struct LightInfo {
	vec3 lightDir; // Dirección de la luz (S.R. mundo)
	vec3 intensity;
};
uniform LightInfo uLight;
struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;

out vec4 fFragColor;

// Factor de escala por los que hay que multiplicar el resultado de la ilum. basada en imágenes (difusa)
const float IBL_Scale = 1.2;
// Factor de escala por los que hay que multiplicar el resultado de la ilum. basada en armónicos (difusa)
const float SPH_Scale = 0.90f;

void main()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 norm = normalize(vNormal);

	if ( uDrawEnvironment == 1 )
		fFragColor = texture(uEnvironmentMap, vTextCoordCuboEntorno); 
	else
	{
		if (uLuzAmbiente == 1)
		{
			// Parte ambiente de la iluminación local
			ambient = uMaterial.ambient * vAmbientLight;
			fFragColor = vec4(clamp(ambient, 0.0, 1.0), 1.0); 
		}
		else if(uLuzArmonicoEsferico == 1 || uLuzImagenes == 1)
		{
			vec3 vDiffuseColor;
			vec4 n = vec4(vNormal, 1.0);
			if(uLuzArmonicoEsferico == 1)
				vDiffuseColor = SPH_Scale * vec3(dot(n, Mred*n), dot(n, Mgreen*n), dot(n, Mblue*n));
			else
				vDiffuseColor = IBL_Scale * texture(uDiffuseEnvMap, normalize(vNormal)).xyz;

			if (uSpecularActivated == 1)
			{
				vec3 vSpecularColor = vec3(texture(uSpecularEnvMap, normalize(vReflectDir)));
				fFragColor = vec4(vDiffuseColor * uMaterial.diffuse + vSpecularColor * uMaterial.specular, 1.0);
			}
			else
			{
				fFragColor = vec4(vDiffuseColor * uMaterial.diffuse, 1.0);
			}
		}
		else
		{
			// Parte difusa de la iluminación local
			vec3 ldir = vec3(uLight.lightDir);
			diffuse = uMaterial.diffuse * uLight.intensity * max(dot(ldir,norm), 0.0);
	
			// Parte especular de la iluminación local
			if (uSpecularActivated == 1)
			{
				vec3 view = normalize(uCameraPos - vPosition);
				vec3 refl = normalize(reflect(-ldir, norm));
				specular = uMaterial.specular * uLight.intensity * pow(max(dot(refl,view),0),uMaterial.shininess);
				
			}
			fFragColor = vec4(clamp(ambient + diffuse + specular, 0.0, 1.0), 1.0); 
		}
		 
	}

	
}