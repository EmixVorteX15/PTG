// Ejercicios tema 4, parte 1 - PTG

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void initCube();
void drawCube();

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
GLint tessVariable = 0; 
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLuint cubeVAOHandle;
GLuint programID;
GLuint locUniformMVPM, locUniformMVM, locUniformNM;
GLuint locUniformLightDir, locUniformLightIntensity;
GLuint locUniformMaterialAmbient, locUniformMaterialDiffuse, locUniformMaterialSpecular, locUniformMaterialShininess;
GLuint locUniformLighting, locUniformTessVariable;


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
GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,    // v0-v1-v2-v3 (front)

                        1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,    // v0-v3-v4-v5 (right)

                        1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,    // v0-v5-v6-v1 (top)

                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,    // v1-v6-v7-v2 (left)

                       -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,    // v7-v4-v3-v2 (bottom)

                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 };  // v4-v7-v6-v5 (back)

GLfloat normales1[] = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0-v1-v2-v3 (front)

                        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0-v3-v4-v5 (right)

                        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0-v5-v6-v1 (top)

                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1-v6-v7-v2 (left)

                        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7-v4-v3-v2 (bottom)

                        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4-v7-v6-v5 (back)


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

///////////////////////////////////////////////////////////////////////////////
// Init Cube
///////////////////////////////////////////////////////////////////////////////
void initCube() 
{
GLuint vboHandle;

	glGenVertexArrays (1, &cubeVAOHandle);
	glBindVertexArray (cubeVAOHandle);

	glGenBuffers(1, &vboHandle); 
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);    
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(normales1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(normales1), normales1);

	GLuint loc = glGetAttribLocation(programID, "aPosition");   
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (GLchar *)NULL + 0 ); 
	GLuint loc2 = glGetAttribLocation(programID, "aNormal"); 
	glEnableVertexAttribArray(loc2); 
	glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, (GLchar *)NULL + sizeof(vertices1) );

	// Tarea por hacer: descomentar la siguiente línea
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray (0);
	return;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

void drawCube()
{
	glBindVertexArray(cubeVAOHandle);
    glDrawArrays(GL_PATCHES, 0, 24);
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

	programID = glCreateProgram();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "shaders/tema4_parte1.vert");
	std::cout << "Compiling Vertex Shader" << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(programID, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "shaders/tema4_parte1.frag");
	std::cout << "Compiling Fragment Shader" << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(programID, fragmentShaderID);

	// TCS y TES
	GLuint tcsShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	loadSource(tcsShaderID, "shaders/tema4_parte1.tesc");
	std::cout << "Compiling Control Teselacion Shader" << std::endl;
	glCompileShader(tcsShaderID);
	printCompileInfoLog(tcsShaderID);
	glAttachShader(programID, tcsShaderID);

	GLuint tesShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	loadSource(tesShaderID, "shaders/tema4_parte1.tese");
	std::cout << "Compiling Evaluacion Teselacion Shader" << std::endl;
	glCompileShader(tesShaderID);
	printCompileInfoLog(tesShaderID);
	glAttachShader(programID, tesShaderID);

	glLinkProgram(programID);
	printLinkInfoLog(programID);
	validateProgram(programID);

	initCube();
	locUniformMVPM = glGetUniformLocation(programID, "uModelViewProjMatrix");
	locUniformMVM = glGetUniformLocation(programID, "uModelViewMatrix");
	locUniformNM = glGetUniformLocation(programID, "uNormalMatrix");

	locUniformLightDir = glGetUniformLocation(programID, "uLight.lightDir");
	locUniformLightIntensity = glGetUniformLocation(programID, "uLight.intensity");
	locUniformMaterialAmbient = glGetUniformLocation(programID, "uMaterial.ambient");
	locUniformMaterialDiffuse = glGetUniformLocation(programID, "uMaterial.diffuse");
	locUniformMaterialSpecular = glGetUniformLocation(programID, "uMaterial.specular");
	locUniformMaterialShininess = glGetUniformLocation(programID, "uMaterial.shininess");

	locUniformLighting = glGetUniformLocation(programID, "uLighting");
	locUniformTessVariable = glGetUniformLocation(programID, "uTessVariable");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT_FACE);

	return true;
}
 
void display()
{
	struct LightInfo {
	 glm::vec3 lightDir; // S.R. del mundo
	 glm::vec3 intensity;
	};
	LightInfo light = { glm::vec3(1.0f, 3.0f, 1.0f),  // Pos. S.R. mundo
						glm::vec3(1.0f, 1.0f, 1.0f)
	};

	struct MaterialInfo {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		GLfloat shininess;
	};
	MaterialInfo gold = {glm::vec3(0.24725f, 0.1995f, 0.0745f), glm::vec3(0.75164f, 0.60648f, 0.22648f), glm::vec3(0.628281f, 0.555802f, 0.366065f), 52.0f};
	MaterialInfo perl = {glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 12.0f};
	MaterialInfo bronze = {glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 25.0f};
	MaterialInfo brass = {glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 28.0f};
	MaterialInfo emerald = {glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 28.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	
	glm::vec3 cameraPos = glm::vec3( 9.0f * cos( yrot / 100 ), 2.0f * sin(xrot / 100) + 2.0f, 9.0f * sin( yrot / 100 ) * cos(xrot /100) );
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ModelCube = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 mvp; // Model-view-projection matrix
	glm::mat4 mv;  // Model-view matrix
	glm::mat3 nm;  // Normal matrix

	glUseProgram(programID);
	// Paso al shader de las propiedades de la luz
	mv = View;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	glm::vec3 ldir = nm * light.lightDir;	// Posición de la luz en el S.R. vista
	glUniform3fv(locUniformLightDir, 1, &(ldir.x));
	glUniform3fv(locUniformLightIntensity, 1, &(light.intensity.r));
	glUniform1i(locUniformLighting, lighting);
	glUniform1i(locUniformTessVariable, tessVariable);


	// Dibuja Cubo
	mvp = Projection * View * ModelCube;
	mv = View * ModelCube;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	glUniformMatrix4fv( locUniformMVPM, 1, GL_FALSE, &mvp[0][0] );
	glUniformMatrix4fv( locUniformMVM, 1, GL_FALSE, &mv[0][0] );
	glUniformMatrix3fv( locUniformNM, 1, GL_FALSE, &nm[0][0] );
	glUniform3fv(locUniformMaterialAmbient, 1, &(gold.ambient.r));
	glUniform3fv(locUniformMaterialDiffuse, 1, &(gold.diffuse.r));
	glUniform3fv(locUniformMaterialSpecular, 1, &(gold.specular.r));
	glUniform1f(locUniformMaterialShininess, gold.shininess);
	drawCube();

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
		exit(EXIT_SUCCESS); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'w': case 'W':
		wireframe = !wireframe;
		if ( wireframe )
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		break;
	case 'l': case 'L':
		lighting = (lighting + 1) % 2;
		break;
	case 't': case 'T':
		tessVariable++;
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
