#version 150

const float scale = 0.05;
const float bias = -0.025;

out vec4 fFragColor;

in vec3 aNormal;
in vec3 aPosition;
in vec3 vColor;

uniform sampler2D uNormalMap;
uniform sampler2D uColorMap;
uniform sampler2D uGlossMap;
uniform sampler2D uHeightMap;

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

in vec3 vLightDir;
in vec3 vViewDir;
in vec2 vST;

uniform bool uTexture;
uniform bool uGloss;
uniform int uParallax;

// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
vec3 ads(vec3 normal, vec2 st)
{	
	vec3 ldir_norm = normalize(vLightDir);
	vec3 vdir_norm = normalize(vViewDir);
	vec3 r = reflect(-vdir_norm, normal);
	vec3 color;
	vec3 gloss;
	vec3 texColor = vec3(texture(uColorMap, st));
	if(uGloss)
	{
		gloss = vec3(texture(uGlossMap, st));
	}
	else
	{
		gloss = vec3(0.5);
	}
	if(uTexture)
	{
		color = uLight.intensity * (   texColor * 0.2
								 + texColor * max(dot(ldir_norm,normal), 0.0) 
								 + gloss * pow(max(dot(r,vdir_norm),0.0), uMaterial.shininess)
							   );
	}
	else
	{
		color = uLight.intensity * (   uMaterial.ambient
								 + uMaterial.diffuse * max(dot(ldir_norm,normal), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir_norm),0.0), uMaterial.shininess)
							   );
	}
	
	
	return color;
}


void main()
{
	vec3 normal;
	vec3 lcolor;

	if(uParallax%3 == 1)
	{
		vec3 vdir_norm = normalize(vViewDir);

		float height = texture(uHeightMap, vST).r;
		float hsb = height * scale + bias;
		vec2 SToffset = vST + hsb * vdir_norm.xy;
		normal = normalize(2.0 * vec3(texture(uNormalMap, SToffset)) - vec3(1.0));
		lcolor = ads(normal, SToffset);
	}
	else if(uParallax%3 ==2)
	{
		vec3 vdir_norm = normalize(vViewDir);

		float height;
		float hsb;
		float hsb1 = 1 * scale + bias;
		float hsb2 = 0 * scale + bias;

		vec2 SToffset;
		vec2 SToffset1 = vST + hsb1 * vdir_norm.xy;
		vec2 SToffset2 = vST + hsb2 * vdir_norm.xy;
		
		for(int i = 0; i < 8; i++)
		{
			SToffset = (SToffset1 + SToffset2) / 2;
			hsb = (hsb1 + hsb2) / 2;
			height = texture(uHeightMap, SToffset).r * scale + bias;

			if(height < hsb)
			{
				SToffset1 = SToffset;
				hsb1 = hsb;
			}
			else
			{
				SToffset2 = SToffset;
				hsb2 = hsb;
			}
		}
		normal = normalize(2.0 * vec3(texture(uNormalMap, SToffset)) - vec3(1.0));
		lcolor = ads(normal, SToffset);
	}
	else
	{
		normal = normalize(2.0 * vec3(texture(uNormalMap, vST)) - vec3(1.0));
		lcolor = ads(normal, vST);
	}

	fFragColor = vec4(lcolor, 1.0);
}