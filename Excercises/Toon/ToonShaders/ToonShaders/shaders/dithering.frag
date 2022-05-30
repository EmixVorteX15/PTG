#version 150 core
#ifdef GL_OES_standard_derivatives
#extension GL_OES_standard_derivatives : enable
#endif
out vec4 fFragColor;

uniform float uScale; // For imperfect, isotropic anti-aliasing in
uniform float uYrot;  // absence of dFdx() and dFdy() functions
uniform int sombreadoSuave;
uniform bool luzFoco;

float frequency = 250.0f;
float frequency_low = 100.0f;
float diff = -0.01;
vec2 intervals = vec2(0.55f, 0.05f);
in vec3 aNormal;
in vec3 aPosition;
varying vec2 vTexCoord;
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

struct ColorInfo{
	vec3 brightest;
	vec3 bright;
	vec3 dark;
	vec3 darkest;
};

struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;
uniform ColorInfo uColor;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;

 
float aastep(float threshold, float value) {
#ifdef GL_OES_standard_derivatives
  float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));
#else
  float afwidth = frequency * (1.0/200.0) / uScale / cos(uYrot);
#endif
  return smoothstep(threshold-afwidth, threshold+afwidth, value);
}

vec3 halfpoint(float f, vec3 dots, vec3 color)
{
    // Distance to nearest point in a grid of
    // (frequency x frequency) points over the unit square
    vec2 st2 = mat2(0.707, -0.707, 0.707, 0.707) * vTexCoord;
    vec2 nearest = 2.0*fract(f * st2) - 1.0;
    float dist = length(nearest);
    float radius = 0.6f;
    vec3 fragcolor = mix(dots, color, aastep(radius, dist));
	
	return fragcolor;
}

const int indexMatrix8x8[64] = int[](0,  32, 8,  40, 2,  34, 10, 42,
                                     48, 16, 56, 24, 50, 18, 58, 26,
                                     12, 44, 4,  36, 14, 46, 6,  38,
                                     60, 28, 52, 20, 62, 30, 54, 22,
                                     3,  35, 11, 43, 1,  33, 9,  41,
                                     51, 19, 59, 27, 49, 17, 57, 25,
                                     15, 47, 7,  39, 13, 45, 5,  37,
                                     63, 31, 55, 23, 61, 29, 53, 21);

float indexValue() {
    int x = int(mod(vTexCoord.x, 8));
    int y = int(mod(vTexCoord.y, 8));
    return indexMatrix8x8[(x + y * 8)] / 64.0;
}

float dither(float color) {
	float closestColor;
	if(color < 0.5)
		closestColor = 0.0f;
	else
		closestColor = 1.0f;

    float secondClosestColor = 1 - closestColor;
    float d = indexValue();
    float distance = abs(closestColor - color);
	float return_color;
    if (distance < d) 
		return_color = closestColor;
	else 
		secondClosestColor;
    return return_color;
}
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
	vec3 return_color = vec3(dither(color.x));
	return return_color;
}
vec3 ads_comic()
{
	vec3 color;
	vec3 ldir = normalize(vec3(uLight.lightPos) - ecPos);
	vec3 vdir = normalize( -ecPos );
	vec3 r = reflect(-ldir, ecNorm);
	vec3 halfp = vec3(0.0,0.0,0.0);

	float prod_escalar = dot(normalize(ecNorm), normalize(ldir));
	float prod_escalar_vista = dot(normalize(vdir), normalize(r));

	if (prod_escalar > intervals.x)
	{
		color  = uColor.brightest;
	}
	else if (prod_escalar <= intervals.x && prod_escalar > intervals.x+diff)
	{
		float prev = intervals.x+diff;
		float val = prod_escalar - prev;
		val /= diff;
		halfp = halfpoint(frequency, uColor.brightest, uColor.bright);
		color = mix(halfp ,uColor.brightest, smoothstep(prev, intervals.x, prod_escalar));
		//color = mix(uColor.dark,uColor.bright, smoothstep(prev, intervals.y, prod_escalar));
	}
	else if (prod_escalar > intervals.y)
	{

		halfp = halfpoint(frequency, uColor.brightest, uColor.bright);
		color = halfp;
		//color = mix(uColor.dark,uColor.bright, smoothstep(prev, intervals.y, prod_escalar));

	}
	else if (prod_escalar <= intervals.y && prod_escalar > intervals.y+diff)
	{
		float prev = intervals.y+diff;
		float val = prod_escalar - prev;
		val /= diff;
		halfp = halfpoint(frequency_low, uColor.bright, uColor.dark);
		color = mix(halfp ,uColor.bright, smoothstep(prev, intervals.y, prod_escalar));
		//color = mix(uColor.dark,uColor.bright, smoothstep(prev, intervals.y, prod_escalar));
	}
	else
	{
		halfp = halfpoint(frequency_low, uColor.bright, uColor.dark);
		color = halfp ;
		//color = uColor.dark ;
	}
	/*else if (prod_escalar_vista <= intervals.z && prod_escalar_vista > intervals.z+diff)
	{
		float prev = intervals.z+diff;
		float val = prod_escalar_vista - prev;
		val /= diff;
		vec3 halfp2 = halfpoint(frequency_low, uColor.dark, uColor.bright);
		color = mix(uColor.dark, halfp2, smoothstep(prev, intervals.z, prod_escalar_vista));
	}
	else
	{
		halfp = halfpoint(frequency_low, uColor.darkest, uColor.dark);
		color = uColor.dark ;
		//color = uColor.darkest;
		//color = vec3(1.0,0.0,0.0);
	}*/

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
	fFragColor = vec4(ads(), 1);
}


