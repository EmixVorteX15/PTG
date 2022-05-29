#version 430

// Tarea por hacer: indicar el tipo de primitiva de entrada y el de salida.
layout(points) in;
layout(triangle_strip, max_vertices=256) out;

// Tamaño (lado / 2) del cuadrado donde pegamos la imagen de la esfera
const float uSize2 = 0.04;

// Le asignamos el ID directamente para no tener que localizar la variable uniform en el código de la aplicación
uniform mat4 uProjectionMatrix;

// Tarea (por hacer): definir los datos de entrada (color y normal) 
in vec3 vECNorm[]; // S.R. Vista
in vec3 vColor[];
// Tarea 2.1 (por hacer): definir el dato de salida del shader (color)
out vec3 gColor;

// Tarea 2.3 (por hacer): definir el dato de salida correspondiente a las coordenadas de textura
out vec2 gTexCoord;

void main()
{
	// Tarea 2.3 (por hacer): Sustituir lo anterior por
	// Generar como salida el siguiente strip de triángulos (con 2 triángulos)
	//     v2    v3
	//     *-----*
	//     |\    |
	//     | \   |
	//     |  \  |
	//     |   \ |
	//     *-----*
	//    v0     v1
	//
	//  Donde v0 = baricentro + vec4(-uSize2, -uSize2, 0.0, 0.0), etc.
	//  Para cada vértice de la primitiva de salida, fijar los datos de salida (gl_Position, coordenadas de textura y color) y emitir sus vértices
	vec4 p0 = gl_in[0].gl_Position;

	vec3 dir_normal = vECNorm[0];
	dir_normal = normalize(dir_normal);
	
	vec4 v0 = p0 + vec4(-uSize2, -uSize2, 0.0, 0.0);
	vec4 v1 = p0 + vec4(uSize2, -uSize2, 0.0, 0.0);
	vec4 v2 = p0 + vec4(-uSize2, uSize2, 0.0, 0.0);
	vec4 v3 = p0 + vec4(uSize2, uSize2, 0.0, 0.0);

	gColor = vColor[0];
	gTexCoord = vec2(0.0,0.0);
	gl_Position = uProjectionMatrix * v0;
	EmitVertex();

	gTexCoord = vec2(1.0,0.0);
	gl_Position = uProjectionMatrix * v1;
	EmitVertex();

	gTexCoord = vec2(0.0,1.0);
	gl_Position = uProjectionMatrix * v2;
	EmitVertex();

	gTexCoord = vec2(1.0,1.0);
	gl_Position = uProjectionMatrix * v3;
	EmitVertex();
	EndPrimitive();
}
