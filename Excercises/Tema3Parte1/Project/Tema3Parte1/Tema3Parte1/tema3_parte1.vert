#version 150 core

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPos;
uniform int uDrawSky;
uniform int reflejo;
uniform bool cromatica;
uniform bool sky;
uniform float nu;
uniform bool fresnel;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vTexCoord;
out vec2 vTexCoord_vec2;
out vec3 vTexCoordR;
out vec3 vTexCoordG;
out vec3 vTexCoordB;

void main()
{
	if(reflejo%3 == 0)
	{
		vTexCoord_vec2 = aTexCoord;
		gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
		
	}
	else if(reflejo%3 == 1)
	{
		vec3 ecNormal = normalize(uNormalMatrix * aNormal);
		vec3 ecView = (uModelMatrix * vec4(aPosition, 1.0)).xyz - uCameraPos;
		if (!sky)
			vTexCoord = reflect(ecView, ecNormal);
		else
			vTexCoord = ecView;
		gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
	}
	else
	{
		vec3 ecNormal = normalize(uNormalMatrix * aNormal);
		vec3 ecView = (uModelMatrix * vec4(aPosition, 1.0)).xyz - uCameraPos;
		if(cromatica)
		{
			vTexCoordR = refract(ecView, ecNormal, 1.0f/(2.0f * 1.506f));
			vTexCoordG = refract(ecView, ecNormal, 1.0f/(2.0f * 1.52f));
			vTexCoordB = refract(ecView, ecNormal, 1.0f/(2.0f * 1.535f));
		}
		else if(fresnel)
		{
			vec3 ecNormal = normalize(uNormalMatrix * aNormal);
			vec3 ecView = (uModelMatrix * vec4(aPosition, 1.0)).xyz - uCameraPos;
			float f = pow((1.0f-nu),2)/pow((1.0f+nu),2);
			float F = f+(1-f)*pow(1-dot(ecNormal, ecView),5);

			if (!sky)
				vTexCoord = refract(ecView, ecNormal, F);
			else
				vTexCoord = ecView;
		}
		else
		{
			
			if (!sky)
				vTexCoord = refract(ecView, ecNormal, nu);
			else
				vTexCoord = ecView;
		}
		gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
	}
}