#version 150 

const int DESP = 2;
const int INC = 1;

in vec3 vECPos; // S.R. Vista
in vec3 vECNorm; // S.R. Vista
in vec4 vShadowCoord;

out vec4 fFragColor;

uniform int uDrawingShadowMap;
uniform sampler2DShadow uShadowMap;
uniform bool uSimpleShadow;
uniform int uPCF_samples;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
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

vec3 phongModelDiffAndSpec () 
{
	vec3 ldir = normalize(vec3(uLight.lightPos) - vECPos);
	vec3 view = normalize(vec3(-vECPos));
	vec3 r = reflect(-ldir,vECNorm);

	vec3 color = uLight.intensity * ( uMaterial.diffuse * max(dot(ldir,vECNorm), 0.0) +
									  uMaterial.specular * pow(max(dot(r,view),0),uMaterial.shininess) );

	return color;
}

void main()
{
	vec3 ambient = uLight.intensity * uMaterial.ambient;
	vec3 diffAndSpec = phongModelDiffAndSpec();
	float shadow;
	if(uPCF_samples%4 == 2)
	{
		float sum = 0.0f;
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1,-1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1,1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(1,1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(1,-1));

		shadow = sum * 0.25;  
	}
	else if(uPCF_samples%4 == 3)
	{
		float sum = 0.0f;
		float count = 0.0f;
		for(int i = -DESP; i<=DESP; i+=INC)
		{
			for(int j = -DESP; j<=DESP; j+=INC)
			{
				sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1*i,-1*j));
				sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1*i,1*j));
				sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(1*i,1*j));
				sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(1*i,-1*j));
				
				count += 4;
			}
		}
		shadow = sum * 1/count;	
	}
	else
		shadow = textureProj(uShadowMap, vShadowCoord);

	
	fFragColor = vec4( ambient + diffAndSpec * shadow, 1.0 );
}