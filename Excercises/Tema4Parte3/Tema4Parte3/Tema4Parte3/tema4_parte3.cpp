// Ejercicios del tema 4, parte 3 - PTG

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lodepng.h"


void printFPS();

void initPoints(GLint);
void drawPoints(GLint);

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
GLint lighting = 0; 
GLfloat amortiguacion = 0.0f; 
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLuint cubeVAOHandle, pointsVAOHandle;
GLuint graphicProgramID, computeProgramID;
GLuint locUniformMVPM, locUniformMVM, locUniformPM;
GLuint locUniformSpriteTex;
GLuint locUniformAmortiguacion;

const GLint NUM_PARTICLES = 32*1024;
const GLint WORK_GROUP_SIZE = 256;

inline GLfloat ranf( GLfloat min = 0.0f, GLfloat max = 1.0f )
{
	return ((max - min) * rand() / RAND_MAX + min);
}


// BEGIN: Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////

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
	source = new std::string( std::istreambuf_iterator<GLchar>(f),   
						std::istreambuf_iterator<GLchar>() );
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

// END:   Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

void initPoints(GLint numPoints) 
{
	GLfloat *points = new GLfloat[NUM_PARTICLES * 4];
	GLfloat *colors = new GLfloat[NUM_PARTICLES * 4];
	GLfloat *velocs = new GLfloat[NUM_PARTICLES * 4];

	for ( GLint i = 0; i < NUM_PARTICLES; i ++ )
	{
		points[4 * i + 0] = ranf(-1.0f, 1.0f); // x
		points[4 * i + 1] = ranf(-1.0f, 1.0f); // y
		points[4 * i + 2] = ranf(-1.0f, 1.0f); // z
		points[4 * i + 3] = 1.0f;
		colors[4 * i + 0] = ranf(); // r
		colors[4 * i + 1] = ranf(); // g
		colors[4 * i + 2] = ranf(); // b
		colors[4 * i + 3] = 1.0f;
		velocs[4 * i + 0] = ranf(-4.0f, 4.0f); // x
		velocs[4 * i + 1] = ranf(-4.0f, 4.0f); // y
		velocs[4 * i + 2] = ranf(-4.0f, 4.0f); // z
		velocs[4 * i + 3] = 0.0f;
	}

	GLuint posSSbo;
	GLuint velSSbo;
	GLuint colSSbo;

	// Tarea por hacer: Crear SSBO en lugar de VBO
	glGenBuffers( 1, &posSSbo);
	glGenBuffers( 1, &velSSbo); // Tarea por hacer: descomentar cuando se utilice compute shaders
	glGenBuffers( 1, &colSSbo);

	
	// Tarea por hacer: Dentro del VAO, utilizar los SSBO como VBO (no será necesario volver a pasarle los datos)
	
	// Tarea por hacer: Activarlos para ser indexados dentro del compute shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLfloat) * NUM_PARTICLES, points, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLfloat) * NUM_PARTICLES, velocs, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLfloat) * NUM_PARTICLES, colors, GL_STATIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, colSSbo);

	glGenVertexArrays (1, &pointsVAOHandle);
	glBindVertexArray (pointsVAOHandle);

	glBindBuffer(GL_ARRAY_BUFFER, posSSbo);    
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * NUM_PARTICLES, points, GL_STATIC_DRAW);
	GLuint loc = glGetAttribLocation(graphicProgramID, "aPosition");   
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( loc, 4, GL_FLOAT, GL_FALSE, 0, (GLchar *)NULL + 0 ); 

	glBindBuffer(GL_ARRAY_BUFFER, colSSbo);    
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * NUM_PARTICLES, colors, GL_STATIC_DRAW);
	GLuint loc2 = glGetAttribLocation(graphicProgramID, "aColor"); 
	glEnableVertexAttribArray(loc2); 
	glVertexAttribPointer( loc2, 4, GL_FLOAT, GL_FALSE, 0, (GLchar *)NULL + 0 );

	glBindVertexArray (0);

	delete []points;
	delete []colors;
	delete []velocs;
	return;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

void drawPoints(GLint numPoints)
{
	glBindVertexArray(pointsVAOHandle);
    glDrawArrays(GL_POINTS, 0, numPoints);
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
	srand (time(NULL));

	glClearColor(0.093f, 0.093f, 0.093f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Tarea por hacer: Crear el objeto programa para el compute shader
	computeProgramID = glCreateProgram();

	GLuint compShaderID = glCreateShader(GL_COMPUTE_SHADER);
	loadSource(compShaderID, "shaders/tema4_parte3_comp.glsl");
	std::cout << "Compiling Comput Shader" << std::endl;
	glCompileShader(compShaderID);
	printCompileInfoLog(compShaderID);
	glAttachShader(computeProgramID, compShaderID);

	locUniformAmortiguacion = 0;

	glLinkProgram(computeProgramID);
	printLinkInfoLog(computeProgramID);
	validateProgram(computeProgramID);

	// Graphic shaders program
	graphicProgramID = glCreateProgram();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "shaders/tema4_parte2_impostor.vert");
	std::cout << "Compiling Vertex Shader" << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(graphicProgramID, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "shaders/tema4_parte2_impostor.frag");
	std::cout << "Compiling Fragment Shader" << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(graphicProgramID, fragmentShaderID);

	// Tarea por hacer: Realizar la misma tarea para el geometry shader (que para el vertex y el fragment shader)
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	loadSource(geometryShaderID, "shaders/tema4_parte2_impostor.geom");
	std::cout << "Compiling Geometry Shader Impostor" << std::endl;
	glCompileShader(geometryShaderID);
	printCompileInfoLog(geometryShaderID);
	glAttachShader(graphicProgramID, geometryShaderID);

	glLinkProgram(graphicProgramID);
	printLinkInfoLog(graphicProgramID);
	validateProgram(graphicProgramID);

	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;
	GLuint error;
	const GLchar img_filename[] = "texturas/white_sphere.png";

	GLuint textId;
	glGenTextures(1, &textId);

	error = lodepng::decode(img_data, img_width, img_height, img_filename);
	if (!error)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
		std::cout << "Error al cargar la textura " << img_filename << std::endl;
	img_data.clear();

	initPoints(NUM_PARTICLES);
	locUniformMVPM = glGetUniformLocation(graphicProgramID, "uModelViewProjMatrix");
	locUniformMVM = glGetUniformLocation(graphicProgramID, "uModelViewMatrix");
	locUniformPM = glGetUniformLocation(graphicProgramID, "uProjectionMatrix");
	locUniformSpriteTex = glGetUniformLocation(graphicProgramID, "uSpriteTex");
	

	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	
	glm::vec3 cameraPos = glm::vec3( 9.0f * cos( yrot / 100 ), 2.0f * sin(xrot / 100) + 2.0f, 9.0f * sin( yrot / 100 ) * cos(xrot /100) );
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ModelCube = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)), glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 mvp; // Model-view-projection matrix
	glm::mat4 mv;  // Model-view matrix

	// Tarea por hacer: Activar el objeto programa con el compute shader
	glUseProgram(computeProgramID);
	glUniform1f(locUniformAmortiguacion, amortiguacion);
	// Tarea por hacer: Lanzar su ejecución, indicando el tamaño del dispatch
	glDispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
	// Tarea por hacer: Colocar un barrera de memoria antes de activar el objeto programa con los shaders gráficos
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	glUseProgram(graphicProgramID);

	// Dibuja Puntos
	mvp = Projection * View * ModelCube;
	mv = View * ModelCube;
	glUniformMatrix4fv( locUniformMVPM, 1, GL_FALSE, &mvp[0][0] );
	glUniformMatrix4fv( locUniformMVM, 1, GL_FALSE, &mv[0][0] );
	glUniformMatrix4fv( locUniformPM, 1, GL_FALSE, &Projection[0][0] );

	glUniform1i (locUniformSpriteTex, 0);


	drawPoints(NUM_PARTICLES);

	glUseProgram(0);
	
	printFPS();

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
		xrot += 0.3f;
		yrot += 0.4f;
	}
	
	glutPostRedisplay();
	return;
}
 
void keyboard(GLubyte key, GLint x, GLint y)
{
	static GLboolean wireframe = false;
	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(1); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case '+':
		if(amortiguacion <= 1.0)
		amortiguacion += 0.01;
		break;
	case '-':
		if(amortiguacion >= 0.0)
		amortiguacion -= 0.01;
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

//-------------------------------------------------------------------------
//  Print FPS
//-------------------------------------------------------------------------
void printFPS()
{
	static GLint frameCount = 0;			//  Número de frames
	static GLfloat fps = 0;
	static GLint currentTime = 0, previousTime = 0;

	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);

	GLint timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);

		previousTime = currentTime;

		frameCount = 0;

		std::cout.precision(4);
		std::cout << "FPS: " << fps << std::endl;
	}
	return;
}
