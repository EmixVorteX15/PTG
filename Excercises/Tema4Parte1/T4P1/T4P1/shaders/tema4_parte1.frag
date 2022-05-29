#version 400

in vec3 vECPos; // S.R. Vista
in vec3 vECNorm; // S.R. Vista
in vec3 vColor;

struct LightInfo {
	vec3 lightDir; // Posición de la luz (S.R. de la vista)
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

uniform int uLighting = 0;

// ADS para luz direccional
vec3 ads()
{
	vec3 ldir = normalize(uLight.lightDir);
	vec3 vdir = -normalize(vECPos);
	vec3 n = normalize(vECNorm);
	vec3 r = reflect(-ldir, n);
	vec3 color;
	color = uLight.intensity * (   uMaterial.ambient 
								 + uMaterial.diffuse * max(dot(ldir,n), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir),0.0), uMaterial.shininess)
							   );

	return clamp(color, 0.0, 1.0);
}

out vec4 fFragColor;

void main()
{
	vec3 color;

	if ( uLighting == 1 )
		color = ads();
	else 
		color = vColor;

	fFragColor = vec4(color, 1.0);	
}