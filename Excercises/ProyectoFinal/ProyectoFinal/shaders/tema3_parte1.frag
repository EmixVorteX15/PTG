#version 150 core
#extension GL_EXT_gpu_shader4: enable

uniform sampler2D uBrickTex;
uniform sampler2D uMossTex;
uniform samplerCube uTexMap; 
uniform int uDrawSky;
uniform int reflejo;
uniform int radius;
uniform bool cromatica;
uniform bool kuwuhara;
uniform bool sky;

out vec4 fFragColor;

in vec3 vTexCoord;
in vec2 vTexCoord_vec2;
in vec3 vTexCoordR;
in vec3 vTexCoordG;
in vec3 vTexCoordB;

float Epsilon = 1e-10;
 


vec3 RGBtoHCV(vec3 RGB)
{
// Based on work by Sam Hocevar and Emil Persson
vec4 P = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0/3.0) : vec4(RGB.gb, 0.0, -1.0/3.0);
vec4 Q = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
float C = Q.x - min(Q.w, Q.y);
float H = abs((Q.w - Q.y) / (6.0 * C + Epsilon) + Q.z);
return vec3(H, C, Q.x);
}



vec3 RGBtoHSV(vec3 RGB)
{
vec3 HCV = RGBtoHCV(RGB);
float S = HCV.y / (HCV.z + Epsilon);
return vec3(HCV.x, S, HCV.z);
}

vec3 RGBtoHSL(vec3 RGB)
{
vec3 HCV = RGBtoHCV(RGB);
float L = HCV.z - HCV.y * 0.5;
float S = HCV.y / (1.0 - abs(L * 2.0 - 1.0) + Epsilon);
return vec3(HCV.x, S, L);
}

vec3 HUEtoRGB(float H)
  {
    float R = abs(H * 6.0 - 3.0) - 1.0;
    float G = 2.0 - abs(H * 6.0 - 2.0);
    float B = 2.0 - abs(H * 6.0 - 4.0);
    return clamp(vec3(R,G,B), 0.0,1.0);
  }

vec3 HSVtoRGB(vec3 HSV)
{
vec3 RGB = HUEtoRGB(HSV.x);
return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

vec3 HSLtoRGB( vec3 HSL)
  {
    vec3 RGB = HUEtoRGB(HSL.x);
    float C = (1.0 - abs(2.0 * HSL.z - 1.0)) * HSL.y;
    return (RGB - 0.5) * C + HSL.z;
  }

const int indexMatrix8x8[64] = int[](0,  32, 8,  40, 2,  34, 10, 42,
                                    48, 16, 56, 24, 50, 18, 58, 26,
                                    12, 44, 4,  36, 14, 46, 6,  38,
                                    60, 28, 52, 20, 62, 30, 54, 22,
                                    3,  35, 11, 43, 1,  33, 9,  41,
                                    51, 19, 59, 27, 49, 17, 57, 25,
                                    15, 47, 7,  39, 13, 45, 5,  37,
                                    63, 31, 55, 23, 61, 29, 53, 21);

vec3 greyscale(vec3 color, float str) {
    float g = dot(color, vec3(0.299, 0.587, 0.114));
    return mix(color, vec3(g), str);
}
float indexValue() {
    int x = int(mod(vTexCoord_vec2.x, 8));
    int y = int(mod(vTexCoord_vec2.y, 8));
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


int paletteSize =8;

vec3 palette[8] = vec3[](
		vec3(0.706,0.282,0.),
        vec3(0.682,0.82,0.275),
        vec3(0.024,0.22,0.),
        vec3(0.212,0.525,0.012),
        vec3(0.373,0.463,0.161),
        vec3(0.553,0.62,0.486),
        vec3(0.137,0.094,0.055),
        vec3(0.612,0.424,0.255) 
		);
		
/*vec3 palette[8] = vec3[](
		vec3(58,68,26),
        vec3(173,155,137),
        vec3(125,111,90),
        vec3(94,90,83),
        vec3(95,118,41),
        vec3(141,158,124),
        vec3(35,24,14),
        vec3(156,108,65) 
		);*/




float hueDistance(float h1, float h2) {
    float diff = abs((h1 - h2));
    return min(abs((1.0 - diff)), diff);
}

vec3[2] closestColors(float hue) {
    vec3 ret[2];
    vec3 closest = vec3(-2, 0, 0);
    vec3 secondClosest = vec3(-2, 0, 0);
    vec3 temp;
    for (int i = 0; i < paletteSize; i++) {
        temp = palette[i];
        float tempDistance = hueDistance(temp.x, hue);
        if (tempDistance < hueDistance(closest.x, hue)) {
            secondClosest = closest;
            closest = temp;
        } else {
            if (tempDistance < hueDistance(secondClosest.x, hue)) {
                secondClosest = temp;
            }
        }
    }
    ret[0] = closest;
    ret[1] = secondClosest;
    return ret;
}

/*vec3 dither(vec3 color) {
    vec3 hsl = RGBtoHSV(color);
    vec3 colors[2] = closestColors(hsl.x);
    vec3 closestColor = colors[0];
    vec3 secondClosestColor = colors[1];
    float d = indexValue();
    float hueDiff = hueDistance(hsl.x, closestColor.x) /
                    hueDistance(secondClosestColor.x, closestColor.x);
    return (HSVtoRGB(hueDiff < d ? closestColor : closestColor));
}*/

void main()
{
	if(!kuwuhara)
	{
		if(reflejo%3 == 0)
		{
			vec4 brickTexColor = texture(uBrickTex, vTexCoord_vec2);
			vec4 mossTexColor = texture(uMossTex, vTexCoord_vec2);
			vec4 grayscale = vec4(greyscale(brickTexColor.rgb, 1.0), 1.0);
			//fFragColor = vec4(vec3(dither(brickTexColor.r)), 1.0);
			fFragColor = vec4( brickTexColor.rgb, 1.0);
		}
		else
		{
			if(cromatica && !sky)
			{
				float texColorR = texture(uTexMap, vTexCoordR).r;
				float texColorG = texture(uTexMap, vTexCoordG).g;
				float texColorB = texture(uTexMap, vTexCoordB).b;
				fFragColor = vec4(texColorR, texColorG, texColorB, 1.0f);
			}
			else
			{
				vec4 texColor = texture(uTexMap, vTexCoord);
				fFragColor = texColor;
			}
		}
	}
	else 
	{
		vec2 src_size = textureSize2D(uBrickTex , 0);
		vec2 uv = vTexCoord_vec2.xy / src_size ;
		float n = float (( radius + 1) * (radius + 1));
		vec3 m[4];
		vec3 s[4];
		for (int k = 0; k < 4; ++k) {
			m[k] = vec3 (0.0);
			s[k] = vec3 (0.0);
		}

		struct Window { int x1 , y1 , x2 , y2; };
		Window W[4] = Window [4](
			Window( -radius, -radius, 0, 0),
			Window( 0, -radius , radius, 0),
			Window( 0, 0, radius , radius ),
			Window( -radius, 0, 0, radius )
		);

		for (int k = 0; k < 4; ++k) {
			for (int j = W[k].y1; j <= W[k].y2; ++j) {
				for (int i = W[k].x1; i <= W[k].x2; ++i) {
					vec3 c = texture(uBrickTex, uv + vec2(i ,j) / src_size ).rgb ;
					m[k] += c;
					s[k] += c * c;
				}
			}
		}

		float min_sigma2 = 1e+2;
		for (int k = 0; k < 4; ++k) {
			m[k] /= n;
			s[k] = abs (s[k] / n - m[k] * m[k]);
			float sigma2 = s[k].r + s[k].g + s[k].b;
			if (sigma2 < min_sigma2) {
			min_sigma2 = sigma2 ;
			fFragColor = vec4(m[k], 1.0);
			}
		}
	}


	
}