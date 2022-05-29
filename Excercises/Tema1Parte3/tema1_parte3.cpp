﻿// Ejercicios del tema 1, parte 3 - PTG
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "lodepng.h"


void printFPS();

void draw1Cube();
void drawMultipleCubes();

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

GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLboolean textureOn = false;
GLuint num_modo = 0;
GLuint textures[1];
GLuint VBOBuffers[7];
GLuint VBOindex[1];
GLuint vaoHandle[3];
GLuint vaoIndex[2];
GLuint vboHandle[8];

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
// tiene 36 v�rtices (6 lados * 2 trian * 3 vertices = 36 vertices). Y cada
// vertice tiene 3 components (x,y,z) de reales, por tanto, el tama�o del vertex
// array es 108 floats (36 * 3 = 108).
const GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
					   -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

						1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
						1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

						1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
					   -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

					   -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
					   -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

					   -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
						1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

						1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
					   -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4
// color array
const GLfloat colores1[] = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
						1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0

						1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
						0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0

						1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
						0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0

						1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
						0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1

						0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
						1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7

						0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
						0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4
// texture coord. array
const GLfloat texcoord1[] = { 1, 1,   0, 1,  0, 0,      // v0-v1-v2 (front)
						0, 0,   1, 0,  1, 1,      // v2-v3-v0

						1, 1,   0, 1,   0, 0,      // v0-v3-v4 (right)
						0, 0,   1, 0,   1, 1,      // v4-v5-v0

						1, 1,   1, 0,   0, 0,      // v0-v5-v6 (top)
						0, 0,   0, 1,   1, 1,      // v6-v1-v0

						1, 1,   1, 0,   0, 0,      // v1-v6-v7 (left)
						0, 0,   0, 1,   1, 1,      // v7-v2-v1

						0, 0,   1, 0,   1, 1,      // v7-v4-v3 (bottom)
						1, 1,   0, 1,   0, 0,      // v3-v2-v7

						1, 0,   0, 0,   0, 1,      // v4-v7-v6 (back)
						0, 1,   1, 1,   1, 0 };    // v6-v5-v4


// Coordenadas del vertex array eliminando los v�rtices repetidos (m�s compacto)
const GLfloat vertices2[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
						1,-1,-1,   1, 1,-1,  -1, 1,-1,  -1,-1,-1 };	// v4,v5,v6,v7 (back)

// color array
const GLfloat colores2[] = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
						0, 0, 1,   0, 1, 1,   0, 1, 0,   0, 0, 0 };	// v4,v5,v6,v7 (back)

// texture coord. array
const GLfloat texcoord2[] = { 1, 1,   0, 1,   0, 0,   1, 0,		// v0,v1,v2,v3 (front)
						1, 1,   1, 0,   0, 0,   0, 1 };		// v4,v5,v6,v7 (back)

// index array del vertex array
const GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
					   0, 3, 4,   4, 5, 0,      // right
					   0, 5, 6,   6, 1, 0,      // top
					   1, 6, 7,   7, 2, 1,      // left
					   7, 4, 3,   3, 2, 7,      // bottom
					   4, 7, 6,   6, 5, 4 };    // back
// Vertex array entrelazado
// Los dos atributos (posici�n y color) se emaquetan juntos. EL resultado es 
// algo similar a un conjunto de estructuras: ((V,C,T), (V,C,T), (V,C,T),...).
const GLfloat vertices3[] = { 1, 1, 1,  1, 1, 1,	 1, 1,	// v0
						-1, 1, 1,  1, 1, 0,	 0, 1,	// v1
						-1,-1, 1,  1, 0, 0,	 0, 0,	// v2
						 1,-1, 1,  1, 0, 1,	 1, 0,	// v3
						 1,-1,-1,  0, 0, 1,	 1, 1,	// v4
						 1, 1,-1,  0, 1, 1,	 1, 0,	// v5
						-1, 1,-1,  0, 1, 0,	 0, 0,	// v6
						-1,-1,-1,  0, 0, 0,  0, 1 };	// v7


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tema 1, parte 3");
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

///////////////////////////////////////////////////////////////////////////////
// dibuja 1 cubo: modo inmediato
// 108 llamadas = 36 glVertex*() + 36 glColor*() + 36 glTexCoord*() 
///////////////////////////////////////////////////////////////////////////////
void draw1Cube()
{
	glBegin(GL_TRIANGLES);

	// front face

	// triangle v0-v1-v2
	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	glColor3f(1, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 1);

	glColor3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 1);

	// triangle v2-v3-v0
	glColor3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 1);

	glColor3f(1, 0, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 1);

	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	// right face

	// triangle v0-v3-v4
	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	glColor3f(1, 0, 1);
	glTexCoord2f(0, 1);
	glVertex3f(1, -1, 1);

	glColor3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex3f(1, -1, -1);

	// triangle v4-v5-v0
	glColor3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex3f(1, -1, -1);

	glColor3f(0, 1, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, 1, -1);

	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	// top face

	// triangle v0-v5-v6
	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	glColor3f(0, 1, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, 1, -1);

	glColor3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 1, -1);

	// triangle v6-v1-v0
	glColor3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 1, -1);

	glColor3f(1, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 1);

	glColor3f(1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);

	// left face

	// triangle  v1-v6-v7
	glColor3f(1, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1, 1);

	glColor3f(0, 1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(-1, 1, -1);

	glColor3f(0, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -1);

	// triangle v7-v2-v1
	glColor3f(0, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -1);

	glColor3f(1, 0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, -1, 1);

	glColor3f(1, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1, 1);

	// bottom face
	// triangle v7-v4-v3
	glColor3f(0, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -1);

	glColor3f(0, 0, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, -1);

	glColor3f(1, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, -1, 1);

	// triangle v3-v2-v7
	glColor3f(1, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, -1, 1);

	glColor3f(1, 0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, -1, 1);

	glColor3f(0, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -1);

	// back face
	// triangle v4-v7-v6
	glColor3f(0, 0, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, -1);

	glColor3f(0, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -1);

	glColor3f(0, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, -1);

	// triangle v6-v5-v4
	glColor3f(0, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, -1);

	glColor3f(0, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, -1);

	glColor3f(0, 0, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, -1);

	glEnd();
	return;
}

////////////////////////////////////////////////////////////////////////////////////
// Dibuja m�ltiples cubos
////////////////////////////////////////////////////////////////////////////////////
void drawMultipleCubes0()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);
				draw1Cube();  // Si no se utiliza VA, VBO ni VAO
				// glDrawArrays o glDrawElements si se utiliza VA, VBO ni VAO
				glPopMatrix();
			}
		}
	}
	return;
}
void drawMultipleCubes1()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices1);
	glColorPointer(3, GL_FLOAT, 0, colores1);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord1);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}
void drawMultipleCubes2()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices2);
	glColorPointer(3, GL_FLOAT, 0, colores2);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord2);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);
				
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}
void drawMultipleCubes3()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), vertices3);
	glColorPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), &vertices3[3]);
	glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GL_FLOAT), &vertices3[6]);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);
				
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	

	return;
}
void drawMultipleCubes4()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[1]);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[2]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	return;
}

void drawMultipleCubes5()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[3]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[4]);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[5]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex[0]);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	return;
}

void drawMultipleCubes6()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[6]);
	glVertexPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 0);
	glColorPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 12);
	glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 24);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex[0]);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

				glPopMatrix();
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}

void drawMultipleCubes7()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vaoHandle[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);
				
				glDrawArrays(GL_TRIANGLES, 0, 36);

				glPopMatrix();
			}
		}
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}

void drawMultipleCubes8()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vaoHandle[1]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);
				
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

				glPopMatrix();
			}
		}
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}

void drawMultipleCubes9()
{
	const GLint N = 40; // N�mero de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin = 1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vaoHandle[2]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for (GLint i = 0; i < N; i++)
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for (GLint j = 0; j < N; j++)
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for (GLint k = 0; k < N; k++)
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
				glTranslatef(x, y, z);
				glScalef(scale, scale, scale);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

				glPopMatrix();
			}
		}
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return;
}

GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);


	// Carga e inicializa la textura
	glGenTextures(1, textures);
	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;
	const char* img_filename = "./texturas/wood_box.png";

	std::cout << "Cargando la textura " << img_filename << std::endl;
	unsigned int error = lodepng::decode(img_data, img_width, img_height, img_filename);

	if (!error)
	{
		// Carga de la textura 
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
		gluBuild2DMipmaps(GL_TEXTURE_2D,			// texture to specify
			GL_RGBA,				// internal texture storage format
			img_width,			// texture width
			img_height,			// texture height
			GL_RGBA,				// pixel format
			GL_UNSIGNED_BYTE,	// color component format
			&img_data[0]);		// pointer to texture image
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	else
		std::cout << "Error al cargar la textura" << img_filename << std::endl;

	// VBO
		// Volcado de datos en los buffers de datos
	glGenBuffers(7, VBOBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), vertices1, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), colores1, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(GL_FLOAT), texcoord1, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[3]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), vertices2, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[4]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), colores2, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[5]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(GL_FLOAT), texcoord2, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers[6]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 8 * sizeof(GL_FLOAT), vertices3, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Volcado de datos en el buffer de indices
	glGenBuffers(1, VBOindex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_FLOAT), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// VAO
	glGenBuffers(7, vboHandle);
	glGenBuffers(2, vaoIndex);
	glGenVertexArrays(3, vaoHandle);

	// Lista de operaciones de Tecla 7
	glBindVertexArray(vaoHandle[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), vertices1, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), colores1, GL_STATIC_DRAW);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(GL_FLOAT), texcoord1, GL_STATIC_DRAW);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glBindVertexArray(0);

	// Lista de operaciones de Tecla 8
	glBindVertexArray(vaoHandle[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[3]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), vertices2, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[4]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), colores2, GL_STATIC_DRAW);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[5]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(GL_FLOAT), texcoord2, GL_STATIC_DRAW);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vaoIndex[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_FLOAT), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Lista de operaciones de Tecla 9
	glBindVertexArray(vaoHandle[2]);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[6]);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GL_FLOAT), vertices3, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 0);
	glColorPointer(3, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 12);
	glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GL_FLOAT), (char*)NULL + 24);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vaoIndex[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_FLOAT), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	return true;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	gluLookAt(
		0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	switch (num_modo)
	{
	case '0':
		drawMultipleCubes0();
		break;
	case '1':
		drawMultipleCubes1();
		break;
	case '2':
		drawMultipleCubes2();
		break;
	case '3':
		drawMultipleCubes3();
		break;
	case '4':
		drawMultipleCubes4();
		break;
	case '5':
		drawMultipleCubes5();
		break;
	case '6':
		drawMultipleCubes6();
		break;
	case '7':
		drawMultipleCubes7();
		break;
	case '8':
		drawMultipleCubes8();
		break;
	case '9':
		drawMultipleCubes9();
		break;
	}

	printFPS();
	glBindTexture(GL_TEXTURE_2D, 0);
	glutSwapBuffers();
	return;
}

void resize(GLint w, GLint h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);

	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'x': case 'X':
		if (textureOn)
		{
			glEnable(GL_TEXTURE_2D);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
		textureOn = !textureOn;
		// ...
		break;
	case '0':
		//Inmediato
		num_modo = '0';
		break;
	case '1':
		num_modo = '1';
		break;
	case '2':
		num_modo = '2';
		break;
	case '3':
		num_modo = '3';
		break;
	case '4':
		num_modo = '4';
		break;
	case '5':
		num_modo = '5';
		break;
	case '6':
		num_modo = '6';
		break;
	case '7':
		num_modo = '7';
		break;
	case '8':
		num_modo = '8';
		break;
	case '9':
		num_modo = '9';
		break;
		/*opcion = key - '0';
		break;*/
	}
	return;
}

void specialKeyboard(int key, int x, int y)
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

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;

		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}

void mouseMotion(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;

		glutPostRedisplay();
	}
	return;
}

void printFPS()
{
	static int frameCount = 0;			//  N�mero de frames
	static float fps = 0;
	static int currentTime = 0, previousTime = 0;

	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);

	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);

		previousTime = currentTime;

		frameCount = 0;

		std::cout.precision(4);
		std::cout << "FPS: " << fps << std::endl;
	}
}