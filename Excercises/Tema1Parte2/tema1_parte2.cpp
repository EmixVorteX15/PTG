// Ejercicios tema 1, parte 2 - PTG

#include <GL/glut.h>
#include <iostream>
#include "lodepng.h"

// Prototipos de las funciones

GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);

// Variables globales con los estados globales de la aplicaci�n

GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = false;
GLboolean iluminacion = false;				// Bool para activar/descativar iluminacion
GLboolean sombreado = false;				// Bool para cambiar el sombreado linear/plano
GLboolean textura = false;					// Bool para activar/desactivar la textura
GLboolean pegado = false;					// Bool para el modo de pegado
GLboolean repetido = false;					// Bool para alternar clamp/repeat


GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana
GLint interruptor_luces = 0;				  // Numero para saber que luz debe estar activa
GLboolean cambio_filtro = 0;			  // Bool para alternar near/linear/mipmap_linear

GLuint textures[2];


GLint main(GLint argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
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

void drawPlane()
{
	const GLuint numDivisiones = 100;
	const GLfloat TAM = 3.0f;
	for (GLuint i = 0; i < numDivisiones; i++)
	{
		GLfloat y0 = TAM * GLfloat(i) / numDivisiones - TAM / 2.0f;
		GLfloat yf = TAM * GLfloat(i + 1) / numDivisiones - TAM / 2.0f;
		glBegin(GL_TRIANGLE_STRIP);
		for (GLint j = 0; j <= numDivisiones; j++)
		{
			GLfloat x = TAM * GLfloat(j) / numDivisiones - TAM / 2.0f;
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(x, y0);
			glVertex3f(x, y0, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(x, yf);
			glVertex3f(x, yf, 0.0f);
		}
		glEnd();
	}
	return;
}

GLboolean init()
{

	const GLfloat luzLocal[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	const GLfloat luzDireccional[] = { 0.5f, 0.5f, 1.0f, 0.0f };
	const GLfloat direccionFoco[] = { -0.5f, -0.5f, -3.0f };
	const GLfloat amplitudFoco = 10.0f;							// En grados
	const GLfloat intensidadLuz[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color de la parte ambiente, difusa y especular de la luz.

	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	// Tarea por hacer: Activar la iluminaci�n.
	glEnable(GL_LIGHTING);

	// Tarea por hacer: definir 3 luces.

	// 1� luz: puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luzLocal);
	glLightfv(GL_LIGHT0, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, intensidadLuz);
	glEnable(GL_LIGHT0);

	// 2� luz: direccional
	glLightfv(GL_LIGHT1, GL_POSITION, luzDireccional);
	glLightfv(GL_LIGHT1, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, intensidadLuz);
	glLightfv(GL_LIGHT1, GL_SPECULAR, intensidadLuz);

	// 3� luz: foco
	glLightfv(GL_LIGHT2, GL_POSITION, luzLocal);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direccionFoco);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, amplitudFoco);
	glLightfv(GL_LIGHT2, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT2, GL_SPECULAR, intensidadLuz);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, intensidadLuz);



	// Tarea por hacer: Crear el obj. textura, cargar la imagen png, activar la textura, 
	// cargar los datos en el objeto textura y definir filtros, modo repetici�n y modo pegado

	glGenTextures(2, textures);

	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;

	const char* img_filename = "Copper_Rough_diffuse.png";
	//	const char* img_filename = "texturas/Craft_diffuse.png";
	//  const char* img_filename = "texturas/Fabric 02_diffuse.png";

	std::cout << "Cargando la textura " << img_filename << std::endl;
	GLuint error = lodepng::decode(img_data, img_width, img_height, img_filename);

	if (!error)
	{
		// Carga de la textura en repeat
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

		// Carga de la textura en clamp
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
		gluBuild2DMipmaps(GL_TEXTURE_2D,			// texture to specify
				GL_RGBA,				// internal texture storage format
				img_width,			// texture width
				img_height,			// texture height
				GL_RGBA,				// pixel format
				GL_UNSIGNED_BYTE,	// color component format
				&img_data[0]);		// pointer to texture image

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		std::cout << "tamanyo de la textura (w,h) = (" << img_width << "," << img_height << ")" << std::endl;


	}
	else
		std::cout << "Error al cargar la textura" << img_filename << std::endl;



	return true;
}

void display()
{
	GLfloat bronzeColorAmb[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat bronzeColorDiff[4] = { 0.8f, 0.6f, 0.0f, 1.0f };
	GLfloat bronzeColorSpec[4] = { 1.0f, 1.0f, 0.8f, 1.0f };
	GLfloat bronzeShininess = 50;
	GLfloat colorNone[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	if (repetido)
	{
		glBindTexture(GL_TEXTURE_2D, textures[0]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, textures[1]);
	}

	if (cambio_filtro % 3 == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (cambio_filtro % 3 == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}	

	glPushMatrix();
	glLoadIdentity();

	gluLookAt(
		0.0f, 0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	glColor4fv(bronzeColorDiff); // Color predefinidos de los objetos

	// Tarea por hacer: Activar el material antes de dibujar los objetos de la escena.
	glMaterialfv(GL_FRONT, GL_AMBIENT, bronzeColorAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bronzeColorDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bronzeColorSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, bronzeShininess);

	drawPlane();
	glutSolidTeapot(0.5f);
	//glutSolidCube(0.5f);
	//glutSolidSphere(0.5f, 20, 20);
	//glutSolidCone(0.25f, 0.5f, 20, 1);
	//glutSolidTorus(0.25, 0.5, 10, 20);

	glPopMatrix();
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

void keyboard(GLubyte key, GLint x, GLint y)
{
	static GLint tipoLuz = 0; // 0 - local, 1 - Direccional, 2- Foco
	const GLint NUM_TIPOS_LUZ = 3;
	static GLint modoPegadoTextura = 0; // 0 - GL_REPLACE, 1 - GL_MODULATE
	const GLint NUM_TIPOS_PEGADO = 2;
	static GLboolean  modoRepeticion = false;

	switch (key)
	{
	case 27: case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'l': case 'L':
		if (iluminacion)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		iluminacion = !iluminacion;
		break;
	case 's': case 'S':
		if (sombreado)
		{
			glShadeModel(GL_SMOOTH);
		}
		else
		{
			glShadeModel(GL_FLAT);
		}
		sombreado = !sombreado;
		break;
	case 't': case 'T':
		interruptor_luces++;
		if (interruptor_luces % 3 == 0)
		{
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
		}
		else if (interruptor_luces % 3 == 1)
		{
			glEnable(GL_LIGHT1);
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT2);
		}
		else
		{
			glEnable(GL_LIGHT2);
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
		}
		break;
	case 'x': case 'X':
		if (textura)
		{
			glEnable(GL_TEXTURE_2D);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
		textura = !textura;
		break;
	case 'p': case 'P':
		if (pegado)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}
		else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		}
		pegado = !pegado;
		break;
	case 'e': case 'E':
		repetido = !repetido;
		break;
	case 'f': case 'F':
		cambio_filtro++;
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