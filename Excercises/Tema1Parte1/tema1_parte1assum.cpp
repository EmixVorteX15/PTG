// Ejercicios del tema 1, parte 1.
#include <iostream>
#include <GL/glut.h>
#define M_PI 3.1415926535897932384626433832795
using namespace std;

void drawBox();
GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);
void printFPS();

// Variables globales (estados globales de la aplicación)
GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = true;
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLboolean fill_b = true, camera_b = true;
GLfloat Kw = 0.25;
GLfloat t, ang, w_ang;


GLint main(GLint argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Ejercicios tema 1 - parte 1");
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
 
void drawBox()
{
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f); //RED
		 
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		glColor3f(0.0f, 0.0f, 1.0f); //BLUE

		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);

		glColor3f(1.0f, 0.0f, 0.0f); //RED

		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

		glColor3f(0.0f, 1.0f, 0.0f); //GREEN
		
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 0.0f, 1.0f); //BLUE

		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		

		glColor3f(0.0f, 1.0f, 0.0f); //GREEN

		glVertex3f(0.5f, 0.5f, 0.5f);	
		glVertex3f(0.5f, 0.5f, -0.5f);
	glEnd();
	return;
}
 
GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
 
	return true;
}
 
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	if (camera_b)
	{
		gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	}
	else
	{
		glOrtho(-1, 1,1, -1, 1.0f, 100.0f);
	}
 
	gluLookAt(
	0.0f, 0.0f, 3.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);
 
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	
	glShadeModel(GL_FLAT);

	
	//Sol
	drawBox();
	
	
	//Tierra
	t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	w_ang = 1.0 / 365 * Kw;
	ang = w_ang * 360 / 2 * M_PI;
	ang *= t;
	glPushMatrix();
		glScalef(0.25, 0.25, 0.25);
		glRotatef(ang, 0, 1, 0);
		glTranslatef(10, 0, 0);
		
		drawBox();

		//Luna

		w_ang = 1.0 / 28 * Kw;
		ang = w_ang * 360 / 2 * M_PI;
		ang *= t;
		glPushMatrix();
			glScalef(0.5, 0.5, 0.5);
			glRotatef(ang, 0, 1, 0);
			glTranslatef(8, 0, 0);
			drawBox();
		glPopMatrix();

		//Perpendicular
		w_ang =  2 * Kw;
		ang = w_ang * 360 / 2 * M_PI;
		ang *= t;

		glPushMatrix();
			glScalef(0.25, 0.25, 0.25);
			glRotatef(ang, 1, 0, 0);
			glTranslatef(0, 4, 0);
			drawBox();
		glPopMatrix();
	
		//Geostacionario
		w_ang = Kw;
		ang = w_ang * 360 / 2 * M_PI;
		ang *= t;
		glPushMatrix();
			glScalef(0.25, 0.25, 0.25);
			glRotatef(ang, 0, 1, 1);
			glTranslatef(-4, 0, 0);
			drawBox();
		glPopMatrix();
	glPopMatrix();
 
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
 
	if (camera_b) 
	{
		gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	}
	else 
	{
		glOrtho(0, g_Width, 0, g_Height, 1.0f, 100.0f);
	}
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}
 
void idle()
{
	if (!mouseDown && animation)
	{
		// xrot += 0.050f; // 0.3f
		// yrot += 0.065f; // 0.4f
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
	case 'v': case 'V':
		if (fill_b) 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fill_b = false;
		}
		else 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fill_b = true;
		}
		break;
	case 'x': case 'X':
		if (camera_b) 
		{
			camera_b = false;
		}
		else 
		{
			camera_b = true;
		}
		
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