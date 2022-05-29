#version 430  

layout( std140, binding=0 ) buffer Pos {
   vec4 Positions[ ]; 
};
layout( std140, binding=1 ) buffer Vel {
   vec4 Velocities[ ]; 
};
layout( std140, binding=2 ) buffer Col {
   vec4 Colors[ ]; 
};

layout(location = 0) uniform float uAmortiguacion;

// Tarea por hacer: definir el tamaño del grupo de trabajo local.
layout( local_size_x = 1024, local_size_y = 1, local_size_z = 1 ) in;

const float dt = 0.001;
const float tamCube = 1.0; // El cubo que engloba las particulas x,y,z = [-tamCube, tamCube]
const vec3 Cube = vec3(tamCube);
const vec3 g = vec3( 0.0, -9.8, 0.0 );

vec3 bounce( vec3 vin, vec3 n ) {
	return reflect( vin, n )*(1-uAmortiguacion);
}

vec3 isOutsideCube( vec3 p, vec3 Cube ) {

	bvec3 pos_bounds = greaterThanEqual(p, Cube);
	bvec3 neg_bounds = lessThanEqual(p, -Cube);

	vec3 i_pos_bounds = vec3(int(pos_bounds[0]),int(pos_bounds[1]),int(pos_bounds[2]));
	vec3 i_neg_bounds = vec3(int(neg_bounds[0]),int(neg_bounds[1]),int(neg_bounds[2]));
	vec3 bounds = vec3((-i_pos_bounds + i_neg_bounds));

	return bounds;
}

void main()
{
	// Tarea por hacer: obtener el id del hilo (partícula)
	uint gid = gl_GlobalInvocationID.x;
	// leer su posición y velocidad de los buffers
	vec3 p = Positions[gid].xyz;
	vec3 v = Velocities[gid].xyz;
	// calcular la nueva posición y velocidad (si choca con las paredes del cubo la partícula cambia de dirección)
	vec3 pp = p + v * dt + 0.5 * dt * dt * g;
	vec3 vp = v + g*dt;

	vec3 normal = isOutsideCube(pp, Cube);
	if (normal != vec3(0,0,0))
	{
		vp = bounce(v, normalize(normal)) + g*dt;
		pp = p + vp * dt + 0.5 * dt * dt * g;
	}


	// Actualizar los valores de los buffers.
	Positions[gid].xyz = pp;
	Velocities[ gid ].xyz = vp;
}


