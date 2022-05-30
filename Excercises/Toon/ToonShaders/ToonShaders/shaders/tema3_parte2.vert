#version 150

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec2 aTexCoord;
out vec3 ecNorm, ecPos;
out vec2 vPos, vNorm, vTexCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;

uniform int sombreadoSuave;
uniform bool luzFoco;

out vec3 vColor;
flat out vec3 vColor_flat;

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


// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
vec3 ads(in vec3 pos, in vec3 n)
{
	vec3 ldir = normalize( vec3(uLight.lightPos) - pos );
	vec3 vdir = normalize( vec3(-pos) );
	vec3 r = reflect(-ldir, n);
	vec3 color;
	color = uLight.intensity * (   uMaterial.ambient 
								 + uMaterial.diffuse * max(dot(ldir,n), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir),0.0), uMaterial.shininess)
							   );
	return color;
}
vec3 adsSpotLight(in vec3 pos, in vec3 n)
{
	vec3 ldir = vec3(uLight.lightPos) - pos;
	float d = length(ldir);
	ldir = normalize(ldir);
	float angle = acos(dot(-ldir, uLight.dir));
	vec3 I = uLight.intensity / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);
	vec3 ambient = I * uMaterial.ambient;
	I *= 1 - smoothstep(uLight.cutOffInt, uLight.cutOffExt, angle);
	vec3 view = normalize(vec3(-pos));
	vec3 r = reflect(-ldir, n);
	return ambient + I * (uMaterial.diffuse * max(dot(ldir,n), 0.0) + uMaterial.specular * pow(max(dot(r,view), 0), uMaterial.shininess));
}

void main()
{	
	ecPos = (uModelViewMatrix * vec4(aPosition, 1.0)).xyz;
	ecNorm = normalize(uNormalMatrix * aNormal);
	vColor = vec3(0.0);
	
	if (luzFoco)
	{
		vColor = clamp( adsSpotLight(ecPos, ecNorm), 0.0, 1.0 );
	}
	else{
		vColor = clamp(ads(ecPos, ecNorm), 0.0, 1.0);
	}
	
	vTexCoord = aTexCoord;
	vColor_flat = vColor;

	gl_Position = uModelViewProjMatrix *  vec4(aPosition, 1.0);
}