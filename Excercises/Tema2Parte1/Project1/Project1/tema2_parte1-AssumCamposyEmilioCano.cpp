// Ejercicios tema 2, parte 1 - PTG

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vboteapot.h"


void initCube(); 
GLint initTeapot(GLint grid, glm::mat4 transform);
void drawCube();
void drawTeapot();

void loadSource(GLuint &shaderID, std::string name);
void printCompileInfoLog(GLuint shadID);
void printLinkInfoLog(GLuint programID);
void validateProgram(GLuint programID);

GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);


GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = false;
GLboolean teapot = false;
GLboolean teapot_esfera = false;
GLboolean teapot_cvertices = false;
GLboolean teapot_fragmentos = false;


GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLuint vaoCube, vaoTeapot;
GLuint programID, programIDTeapot;
GLuint loc1, loc2, loc3, loc4, loc5, loc6, loc7, loc8, loc9, loc10; // id de las variables uniform

GLint numVertTeapot; // número de vértices de la tetera

GLfloat timeElapsed = 0;
GLfloat uDensity;
GLfloat uFrequency = 8.0f;


// cubo ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// Coordenadas del vertex array  =====================================
// Un cubo tiene 6 lados y cada lado tiene 2 triangles, por tanto, un cubo
// tiene 36 vértices (6 lados * 2 trian * 3 vertices = 36 vertices). Y cada
// vertice tiene 4 components (x,y,z) de reales, por tanto, el tamaño del vertex
// array es 144 floats (36 * 4 = 144).
GLfloat vertices1[] = { 1, 1, 1, 1,  -1, 1, 1, 1,  -1,-1, 1, 1,     // v0-v1-v2 (front)
                       -1,-1, 1, 1,   1,-1, 1, 1,   1, 1, 1, 1,     // v2-v3-v0

                        1, 1, 1, 1,   1,-1, 1, 1,   1,-1,-1, 1,      // v0-v3-v4 (right)
                        1,-1,-1, 1,   1, 1,-1, 1,   1, 1, 1, 1,      // v4-v5-v0

                        1, 1, 1, 1,   1, 1,-1, 1,  -1, 1,-1, 1,      // v0-v5-v6 (top)
                       -1, 1,-1, 1,  -1, 1, 1, 1,   1, 1, 1, 1,      // v6-v1-v0

                       -1, 1, 1, 1,  -1, 1,-1, 1,  -1,-1,-1, 1,      // v1-v6-v7 (left)
                       -1,-1,-1, 1,  -1,-1, 1, 1,  -1, 1, 1, 1,      // v7-v2-v1

                       -1,-1,-1, 1,   1,-1,-1, 1,   1,-1, 1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1, 1,  -1,-1, 1, 1,  -1,-1,-1, 1,      // v3-v2-v7

                        1,-1,-1, 1,  -1,-1,-1, 1,  -1, 1,-1, 1,      // v4-v7-v6 (back)
                       -1, 1,-1, 1,   1, 1,-1, 1,   1,-1,-1, 1 };    // v6-v5-v4
// color array
GLfloat colores1[]   = {1, 1, 1, 1,   1, 1, 0, 1,   1, 0, 0, 1,      // v0-v1-v2 (front)
                        1, 0, 0, 1,   1, 0, 1, 1,   1, 1, 1, 1,      // v2-v3-v0

                        1, 1, 1, 1,   1, 0, 1, 1,   0, 0, 1, 1,      // v0-v3-v4 (right)
                        0, 0, 1, 1,   0, 1, 1, 1,   1, 1, 1, 1,      // v4-v5-v0

                        1, 1, 1, 1,   0, 1, 1, 1,   0, 1, 0, 1,      // v0-v5-v6 (top)
                        0, 1, 0, 1,   1, 1, 0, 1,   1, 1, 1, 1,      // v6-v1-v0

                        1, 1, 0, 1,   0, 1, 0, 1,   0, 0, 0, 1,      // v1-v6-v7 (left)
                        0, 0, 0, 1,   1, 0, 0, 1,   1, 1, 0, 1,      // v7-v2-v1

                        0, 0, 0, 1,   0, 0, 1, 1,   1, 0, 1, 1,      // v7-v4-v3 (bottom)
                        1, 0, 1, 1,   1, 0, 0, 1,   0, 0, 0, 1,      // v3-v2-v7

                        0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 0, 1,      // v4-v7-v6 (back)
                        0, 1, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1 };    // v6-v5-v4



// BEGIN: Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////

void loadSource(GLuint &shaderID, std::string name) 
{
	std::ifstream f(name.c_str());
	if (!f.is_open()) 
	{
		std::cerr << "File not found " << name.c_str() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	// now read in the data
	std::string *source;
	source = new std::string( std::istreambuf_iterator<char>(f),   
						std::istreambuf_iterator<char>() );
	f.close();
   
	// add a null to the string
	*source += "\0";
	const GLchar * data = source->c_str();
	glShaderSource(shaderID, 1, &data, NULL);
	delete source;
	return;
}

void printCompileInfoLog(GLuint shadID) 
{
GLint compiled;
	glGetShaderiv( shadID, GL_COMPILE_STATUS, &compiled );
	if (compiled == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetShaderiv( shadID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetShaderInfoLog( shadID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader compiling failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void printLinkInfoLog(GLuint programID)
{
GLint linked;
	glGetProgramiv( programID, GL_LINK_STATUS, &linked );
	if(! linked)
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader linking failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void validateProgram(GLuint programID)
{
GLint status;
    glValidateProgram( programID );
    glGetProgramiv( programID, GL_VALIDATE_STATUS, &status );

    if( status == GL_FALSE ) 
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

        if( infoLength > 0 ) 
		{
			GLchar *infoLog = new GLchar[infoLength];
			GLint chsWritten = 0;
            glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );
			std::cerr << "Program validating failed:" << infoLog << std::endl;
			system("pause");
            delete [] infoLog;

			exit(EXIT_FAILURE);
		}
    }
	return;
}

// END:   Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////


// BEGIN: Inicializa primitivas de dibujo //////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Init para Cube
///////////////////////////////////////////////////////////////////////////////
void initCube()
{
	GLuint vboHandle;

	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	// Add nosotros
	GLuint indiceVertex = glGetAttribLocation(programID, "aVertex");
	GLuint indiceColor = glGetAttribLocation(programID, "aColor");
	//glEnableClientState(GL_VERTEX_ARRAY);	// Tarea por hacer (ejer. 1.2): sustituir por glEnableVertexAttribArray
	glEnableVertexAttribArray(indiceVertex);
	glEnableVertexAttribArray(indiceColor);
	glEnableClientState(GL_COLOR_ARRAY);	// Idem.

	glGenBuffers(1, &vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(colores1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(colores1), colores1);

	//glVertexPointer(4, GL_FLOAT, 0, (GLubyte *)NULL + 0);	// Tarea por hacer (ejer. 1.2): sustituir por glVertexAttribPointer
	glVertexAttribPointer(indiceVertex, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(indiceColor, 4, GL_FLOAT, GL_FALSE, 0, (char*)NULL + sizeof(vertices1));
	glColorPointer(4, GL_FLOAT, 0, (GLubyte *)NULL + sizeof(GL_FLOAT) * 4 * 36); // Idem.

	glBindVertexArray(0);
	return;
}

///////////////////////////////////////////////////////////////////////////////
// Init Teapot
// parametros: 
//		grid - número de rejillas
//		transform - matriz de tranformación del modelo
// return:
//		número de vertices del modelo
///////////////////////////////////////////////////////////////////////////////
GLint initTeapot(GLint grid, glm::mat4 transform)
{
	GLint verts = 32 * (grid + 1) * (grid + 1);
	GLint faces = grid * grid * 32;
	GLfloat * v = new GLfloat[verts * 3];
	GLfloat * n = new GLfloat[verts * 3];
	GLfloat * tc = new GLfloat[verts * 2];
	GLuint * el = new GLuint[faces * 6];

	generatePatches(v, n, tc, el, grid);
	moveLid(grid, v, transform);

	// Tarea por hacer (ejer. 1.4): Crear y activar VAO
	glGenVertexArrays(1, &vaoTeapot);
	glBindVertexArray(vaoTeapot);

	GLuint handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), v, GL_STATIC_DRAW); // Datos de la posición de los vértices
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint teapotIDPos = glGetAttribLocation(programIDTeapot, "aPosition");
	glEnableVertexAttribArray(teapotIDPos);
	glVertexAttribPointer(teapotIDPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), n, GL_STATIC_DRAW); // Datos de las normales de los vértices
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint teapotIDNormal = glGetAttribLocation(programIDTeapot, "aNormal");
	glEnableVertexAttribArray(teapotIDNormal);
	glVertexAttribPointer(teapotIDNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(GLfloat), tc, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint teapotIDtc = glGetAttribLocation(programIDTeapot, "aTexCoord0");
	glEnableVertexAttribArray(teapotIDtc);
	glVertexAttribPointer(teapotIDtc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW); // Array de índices

	delete[] v;
	delete[] n;
	delete[] el;
	delete[] tc;

	// Tarea por hacer (ejer. 1.4): Desactivar VAO
	glBindVertexArray(0);
	return 6 * faces;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////


// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dibuja cubo utilizando VAO (no indexado)
///////////////////////////////////////////////////////////////////////////////
void drawCube()
{
	glBindVertexArray(vaoCube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); 
	return;
}

///////////////////////////////////////////////////////////////////////////////
// Dibuja tetera utilizando VAO (indexado)
///////////////////////////////////////////////////////////////////////////////
void drawTeapot() {
	// Tarea por hacer (ejer. 1.4): Dibujar la tetera
	glBindVertexArray(vaoTeapot);
	glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, (char*)NULL + 0);
	glBindVertexArray(0);
	return;
}

// END: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	  system("pause");
	  exit(-1);
	}
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
 
	glutMainLoop();
 
	return EXIT_SUCCESS;
}

GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	// Tarea por hacer (ejer. 1.1): Descomentar el siguiente código
	
	//  Creamos el objeto shader compilado para el shader de vertice.
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "./shaders/tema2_parte1.vert");
	std::cout << "Compilando shader de vertice ..." << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	
	// Tarea por hacer (ejer. 1.1): Creamos el objeto shader compilado para el shader de fragmento.
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "./shaders/tema2_parte1.frag");
	std::cout << "Compilando shader de fragmento ..." << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);

	// Tarea por hacer (ejer. 1.1): Creamos el objeto programa.
	programID = glCreateProgram();
	// Tarea por hacer (ejer. 1.1): Adjuntamos los shaders compilados al objeto programa
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	// Tarea por hacer (ejer. 1.1): Linkamos el objeto programa
	glLinkProgram(programID);
	// Tarea por hacer (ejer. 1.1): Descomentar el siguiente código (verifica que no hay errores en el linkado)
	
	std::cout << "Linkando objeto programa ..." << std::endl;
	printLinkInfoLog(programID);
	validateProgram(programID);

	// Tarea por hacer (ejer. 1.3): Localizar las variables uniform definidas en los shaders.
	loc1 = glGetUniformLocation(programID, "uModelViewProjectionMatrix");
	initCube();

	// Tarea por hacer (ejer. 1.4): Descomentar el siguiente código
	GLuint vertexShaderIDteapot = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderIDteapot, "./shaders/tema2_parte1_teapot.vert");
	std::cout << "Compilando shader de vertice de teapot ..." << std::endl;
	glCompileShader(vertexShaderIDteapot);
	printCompileInfoLog(vertexShaderIDteapot);

	GLuint fragShaderIDteapot = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragShaderIDteapot, "./shaders/tema2_parte1_teapot.frag");
	std::cout << "Compilando shader de fragmento de teapot ..." << std::endl;
	glCompileShader(fragShaderIDteapot);
	printCompileInfoLog(fragShaderIDteapot);

	// Creamos el objeto programa.
	programIDTeapot = glCreateProgram();
	// Adjuntamos los shaders compilados al objeto programa
	glAttachShader(programIDTeapot, vertexShaderIDteapot);
	glAttachShader(programIDTeapot, fragShaderIDteapot);
	// Linkamos el objeto programa
	glLinkProgram(programIDTeapot);
	printLinkInfoLog(programIDTeapot);
	validateProgram(programIDTeapot);
	
	loc2 = glGetUniformLocation(programIDTeapot, "uModelViewProjectionMatrix");
	loc3 = glGetUniformLocation(programIDTeapot, "timeElapsed");
	loc4 = glGetUniformLocation(programIDTeapot, "teapot_esfera");
	loc5 = glGetUniformLocation(programIDTeapot, "teapot_cvertices");
	loc6 = glGetUniformLocation(programIDTeapot, "trans");
	loc7 = glGetUniformLocation(programIDTeapot, "teapot_fragmentos");
	loc8 = glGetUniformLocation(programIDTeapot, "vST");
	loc9 = glGetUniformLocation(programIDTeapot, "uDensity");
	loc10 = glGetUniformLocation(programIDTeapot, "uFrequency");
	
	numVertTeapot = initTeapot(10, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f)));
	
	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	timeElapsed = (float)glutGet(GLUT_ELAPSED_TIME);
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
	0.0f, 0.0f, 5.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	*/
	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	Model = glm::rotate(Model, xrot, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, yrot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mvp = Projection * View * Model;
	// Tarea por hacer (ejer. 1.3): Sustituir el anterior código por el código siguiente.
	if (!teapot) {
		

		// Tarea por hacer (ejer. 1.1): Activar el objeto programa
		glUseProgram(programID);
		// Tarea por hacer (ejer. 1.3): Pasarle a los shaders las variables uniform.	
		if (loc1 >= 0) {
			glUniformMatrix4fv(loc1, 1, GL_FALSE, &mvp[0][0]);
		}
		drawCube();
	}
	else 
	{
		// Tarea por hacer (ejer. 1.4): Activar el objeto programa
		glUseProgram(programIDTeapot);
		// Tarea por hacer (ejer. 1.4): Pasarle a los shaders las variables uniform.
		glm::mat4 trans = glm::mat4(1.0f);
		float angle = (float)timeElapsed;
		trans = glm::rotate(trans, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::vec2 vST = glm::vec2(0, 0);
		uDensity = abs(sin(timeElapsed/1000.0f));
		
		if (loc2 >= 0 or loc3 >= 0 or loc4 >= 0 or loc5 >= 0 or loc6 >= 0 or loc7 >= 0 or loc8 >= 0 or loc9 >= 0 or loc10 >= 0) {
			glUniformMatrix4fv(loc2, 1, GL_FALSE, &mvp[0][0]);
			glUniform1f(loc3, timeElapsed);
			glUniform1i(loc4, teapot_esfera);
			glUniform1i(loc5, teapot_cvertices);
			glUniformMatrix4fv(loc6, 1, GL_FALSE, &trans[0][0]);
			glUniform1i(loc7, teapot_fragmentos);
			glUniform2fv(loc8, 1, &vST[0]);
			glUniform1f(loc9, uDensity);
			glUniform1f(loc10, uFrequency);
			
		}
		drawTeapot();
	}
	// Tarea por hacer (ejer. 1.1): Desactivar el objeto programa	
	glUseProgram(0);

	glutSwapBuffers();
	return;
}
 
void resize(GLint w, GLint h)
{
	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
	return;
}
 
void idle()
{
	if (!mouseDown && animation)
	{
		xrot += 0.010f;
		yrot += 0.012f;
	}

	glutPostRedisplay();
	return;
}
 
void keyboard(GLubyte key, GLint x, GLint y)
{
	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(EXIT_SUCCESS); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 't': case 'T':
		teapot = !teapot;
		break;
	case 'p': case 'P':
		teapot_esfera = !teapot_esfera;
		break;
	case 'c': case 'C':
		teapot_cvertices = !teapot_cvertices;
		break;
	case 'f': case 'F':
		teapot_fragmentos = !teapot_fragmentos;
		break;
	}
	return;
}
 
void specialKeyboard(GLint key, GLint x, GLint y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;
 
		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(g_Width, g_Height);
			glutPositionWindow(50, 50);
		}
	}
	return;
}
 
void mouse(GLint button, GLint state, GLint x, GLint y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;
 
		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
	return;
}
 
void mouseMotion(GLint x, GLint y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;
 
		glutPostRedisplay();
	}
	return;
}

