// Core Libraries
#include <iostream>
#include <string>
#include <math.h>

// 3rd Party Libraries
#include <GLUT\glut.h>
#include <IL\ilut.h>

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame

int windowWidth = 800;
int windowHeight = 800;

int mousepositionX;
int mousepositionY;

/* This is our texture handle */
GLuint textureHandle;

// A few conversions to know
float degToRad = 3.14159f / 180.0f;
float radToDeg = 180.0f / 3.14159f;

void drawSceneObjects(void)
{
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glutSolidCube(0.1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -2.0f);
		glColor4f(0.0, 0.5, 1.0, 1.0);
		glutSolidSphere(1, 30, 30);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(3.0f, 0.0f, 0.0f);
		glColor4f(1.0, 0.5, 0.0, 1.0);
		glutSolidSphere(1, 30, 30);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-3.0f, 0.0f, -2.0f);
		glColor4f(1.0, 0.5, 1.0, 1.0);
		glutSolidTorus(0.1, 0.5, 15, 15);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-3.0f, 0.0f, -20.0f);
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glutSolidDodecahedron();
	glPopMatrix();
}

/* function DisplayCallbackFunction(void)
* Description:
*  - this is the openGL display routine
*  - this draws the sprites appropriately
*/
void DisplayCallbackFunction(void)
{
	/* clear the screen */
	glClearColor(0.2f, 0.2f, 0.2f, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, windowWidth, windowHeight);	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // clear our the transform matrix
	
	// reset perspective

	
	/* Move the Camera */
//	gluLookAt(1,2,8, 0, 0, 0, 0, 1, 0);

	/* This is where we draw things */
	drawSceneObjects();

	/* Swap Buffers to Make it show up on screen */
	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	std::cout << "Key Down:" << (int)key << std::endl;

	switch (key)
	{
	case 32: // the space bar
		break;
	case 27: // the escape key
	case 'q': // the 'q' key
		exit(0);
		break;
	}
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{
	/* this call makes it actually show up on screen */
	glutPostRedisplay();
	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*  - currently set up for an orthographic view (2D only)
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	// switch to projection because we're changing projection
	float asp = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//gluPerspective(90.0f, 1.0f, 1.0f, 100.f);

	//switch back to modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	// Handle mouse clicks
	if (state == GLUT_DOWN)
	{
		std::cout << "Mouse x:" << x << " y:" << y << std::endl;
	}
}


/* function MouseMotionCallbackFunction()
* Description:
*   - this is called when the mouse is clicked and moves
*/
void MouseMotionCallbackFunction(int x, int y)
{
}

/* function MousePassiveMotionCallbackFunction()
* Description:
*   - this is called when the mouse is moved in the window
*/
void MousePassiveMotionCallbackFunction(int x, int y)
{
	mousepositionX = x;
	mousepositionY = y;
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	/* initialize the window and OpenGL properly */
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Supercool dope title game");

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);

	/* Call some OpenGL parameters */
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	/* Turn on the lights! */
	


	/* Init Image Library */
	glEnable(GL_TEXTURE_2D);
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	/* Load a texture */
	textureHandle = ilutGLLoadImage("..//img//win.png");
	glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, NULL);

	/* start the event handler */
	glutMainLoop();
	return 0;
}