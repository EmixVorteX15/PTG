#version 150

out vec4 fFragColor;

uniform int sombreadoSuave;
uniform bool luzFoco;

in vec3 aNormal;
in vec3 aPosition;
in vec3 vColor;
flat in vec3 vColor_flat;
in vec3 ecNorm, vNorm;
in vec3 ecPos, vPos;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
	vec3 k;
	vec3 dir; // Dirección del foco (S.R. de la vista)
	float cutOffInt; // Ángulo interior (límite zona iluminada)
	float cutOffExt; // Ángulo exterior (límite zona penumbra)
};
uniform LightInfo uLight;

struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
vec3 ads()
{	
	vec3 ldir = normalize( vec3(uLight.lightPos) - ecPos );
	vec3 vdir = normalize( vec3(-ecPos) );
	vec3 r = reflect(-ldir, ecNorm);
	vec3 color;
	color = uLight.intensity * (   uMaterial.ambient 
								 + uMaterial.diffuse * max(dot(ldir,ecNorm), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir),0.0), uMaterial.shininess)
							   );
	
	return color;
}
vec3 ads_comic()
{
	vec3 color;
	vec3 ldir = normalize(vec3(uLight.lightPos) - ecPos);
	vec3 vdir = normalize( -ecPos );
	vec3 r = reflect(-ldir, ecNorm);
	

	if (dot(normalize(vdir), normalize(r)) > 0.94)
	{
		color  = uMaterial.diffuse + uMaterial.specular;
	}
	else if (dot(normalize(ecNorm), normalize(ldir)) > 0)
	{
		color = uMaterial.diffuse;
	}
	else
	{
		color = uMaterial.ambient * 1.5;
	}

	if(luzFoco)
	{
		float angle = acos(dot(-ldir, uLight.dir));
		if(angle > uLight.cutOffExt)
			color = uMaterial.ambient * 1.5;
	}
	

	return color;
}
vec3 adsSpotLight()
{
	vec3 ldir = vec3(uLight.lightPos) - ecPos;
	float d = length(ldir);
	ldir = normalize(ldir);
	float angle = acos(dot(-ldir, uLight.dir));
	vec3 I = uLight.intensity / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);
	vec3 ambient = I * uMaterial.ambient;
	I *= 1 - smoothstep(uLight.cutOffInt, uLight.cutOffExt, angle);
	vec3 view = normalize(vec3(-ecPos));
	vec3 r = reflect(-ldir, ecNorm);
	return ambient + I * (uMaterial.diffuse * max(dot(ldir,ecNorm), 0.0) + uMaterial.specular * pow(max(dot(r,view), 0), uMaterial.shininess));
}


void main()
{
	if (sombreadoSuave % 4 == 0)
		fFragColor = vec4(vColor, 1.0);
	else if(sombreadoSuave % 4 == 1)
		fFragColor = vec4(vColor_flat, 1.0);
	else if (sombreadoSuave % 4 == 2)
	{
		if (luzFoco)
		{
			fFragColor = vec4(clamp(adsSpotLight(), 0.0, 1.0 ), 1.0);
		}
		else
		{
			fFragColor = vec4(clamp(ads(), 0.0, 1.0 ), 1.0);
		}
	}
	else
	{
		float contorno = dot(normalize(-ecPos), normalize(ecNorm) );

		fFragColor = vec4(ads_comic(), 1.0);

		if (contorno < 0.14)
		{
			fFragColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
}