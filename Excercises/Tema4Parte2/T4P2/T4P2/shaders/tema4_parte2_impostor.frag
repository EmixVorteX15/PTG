#version 430
const float sphere_size = 0.38;
in vec3 gColor;

// Tarea 2.3 (por hacer): recoger como dato de entada las coordenadas de textura.
in vec2 gTexCoord;
// Le asignamos el ID directamente para no tener que localizar la variable uniform en el código de la aplicación
layout(location = 5) uniform sampler2D uSpriteTex;

out vec4 fFragColor;

void main()
{
	vec4 color;
	vec4 sphere_texture;
	if(distance(gTexCoord, vec2(0.5,0.5)) <= sphere_size){
		sphere_texture =  texture(uSpriteTex, gTexCoord); 
		color = vec4(gColor, 1.0);
	}
	else
	{
		discard;
	}

	fFragColor = color * sphere_texture;	
}