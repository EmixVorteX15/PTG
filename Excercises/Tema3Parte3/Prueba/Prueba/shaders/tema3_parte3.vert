#version 150

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec2 aTexCoord;
in vec3 aTangent;
//out vec3 ecNorm, ecPos;


uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

out vec3 vColor;

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

out vec3 vLightDir;
out vec3 vViewDir;
out vec2 vST;

// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
/*vec3 ads(in vec3 pos, in vec3 n)
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
}*/

void main()
{	
	vec3 n = normalize(uNormalMatrix * aNormal);
	vec3 t = normalize(uNormalMatrix * aTangent);
	vec3 b = cross(n, t);
	mat3 TBN = transpose(mat3(t, b, n));

	vec4 eyeVertex = uModelViewMatrix * vec4(aPosition, 1.0);
	vec3 eyeViewDir = vec3(0., 0., 0) - eyeVertex.xyz;
	vec3 eyeLightDir = vec3(uLight.lightPos - eyeVertex);
	vLightDir = TBN * eyeLightDir;
	vViewDir = TBN * eyeViewDir;
	vST = aTexCoord;
	gl_Position =  uModelViewProjMatrix *  vec4(aPosition, 1.0);

	//ecPos = (uModelViewMatrix * vec4(aPosition, 1.0)).xyz;
	//ecNorm = normalize(uNormalMatrix * aNormal);
	
	//gl_Position = uModelViewProjMatrix *  vec4(aPosition, 1.0);
}