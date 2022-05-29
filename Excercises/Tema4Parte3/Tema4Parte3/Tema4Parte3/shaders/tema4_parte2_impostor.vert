#version 430  

// Le asignamos el ID directamente para no tener que localizar el atributo en el código de la aplicación
in vec3 aPosition;
in vec3 aNormal;
in vec3 aColor;

// Idem con las variables uniform
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;


out vec3 vECNorm; // S.R. Vista
out vec3 vColor;

void main()
{
	vECNorm = normalize(uNormalMatrix * aNormal);
	vColor = abs(normalize(aColor));

	// Tarea 2.1: guardar en gl_Position la posición del vértice en el S.R. de la vista 
	// (en lugar del espacio de la ventana, tal como se hace ahora)
	gl_Position = uModelViewMatrix * vec4(aPosition, 1.0);
}