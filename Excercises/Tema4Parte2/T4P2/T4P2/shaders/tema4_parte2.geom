#version 430

// Tarea por hacer: indicar el tipo de primitiva de entrada y el de salida.
layout(triangles) in;
layout(points, max_vertices=256) out;

// Tamaño (lado / 2) del cuadrado donde pegamos la imagen de la esfera
const float uSize2 = 0.04;

// Le asignamos el ID directamente para no tener que localizar la variable uniform en el código de la aplicación
layout(location = 3) uniform mat4 uProjectionMatrix;
layout(location = 4) uniform float uExplosionFactor;

// Tarea (por hacer): definir los datos de entrada (color y normal) 
in vec3 vECNorm[]; // S.R. Vista
in vec3 vColor[];
// Tarea 2.1 (por hacer): definir el dato de salida del shader (color)
out vec3 gColor;

// Tarea 2.3 (por hacer): definir el dato de salida correspondiente a las coordenadas de textura

void main()
{
	// Tarea 2.1 (por hacer): obtener el baricentro del triángulo
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;

	vec4 baricentro = (p0+p1+p2)/3;
	vec3 color_medio = (vColor[0] + vColor[1] + vColor[2]) / 3;

	// Tarea 2.2 (por hacer): obtener la media de las normales de los tres vértices y desplazar el baricentro en esa direccion

	vec3 dir_normal = (vECNorm[0] + vECNorm[1] + vECNorm[2]) / 3;
	dir_normal = normalize(dir_normal);
	vec3 desplazamiento = dir_normal * uExplosionFactor;
	baricentro += vec4(desplazamiento,1.0);
	
	// Tarea 2.1 (por hacer): fijar los datos de salida del vértice y emitir el vértice
	gl_Position = uProjectionMatrix * baricentro;
	gColor = color_medio;

	EmitVertex();
	EndPrimitive();	
}
