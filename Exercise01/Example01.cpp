/* Used my Computer Graphics assignment template as a starting base for this assignment*/

// Core Libraries
#include <iostream>
#include <string>
#include <math.h>

#include <vector>

// 3rd Party Libraries
#include <GLUT\glut.h>

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame

class Vector3
{
public:
	float x, y, z;
	Vector3(float _x, float _y, float _z)
	{
		x = _x; y = _y; z = _z;
	}
	Vector3()
	{
		x = y = z = 0;
	}

	Vector3 operator*(const Vector3& v)
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}
	Vector3 operator*(const float f)
	{
		return Vector3(x*f, y*f, z*f);
	}
	Vector3 operator+(const Vector3& v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	bool operator==(const Vector3& v)
	{
		if (x == v.x && y == v.y && z == v.z)
		{
			return true;
		}
		else
			return false;
	}
};

int windowWidth = 800;
int windowHeight = 600;

int mousepositionX;
int mousepositionY;

// A few conversions to know
float degToRad = 3.14159f / 180.0f;
float radToDeg = 180.0f / 3.14159f;

float rotx, roty;

//A single entry in the arc length table. Contains a sample that is called Vec, the time value that the sample is taken at, the arc length, which is the length from the start to that point and the segment number.
struct CurveEntry
{
	Vector3 vec;
	float time;
	float arcLength;
	int segmentNumber;
};

std::vector<Vector3> coordsVect;
std::vector<CurveEntry> curve;
bool curveCalculated = true;
bool speedControl = true;

Vector3 objectVect(0, 0, -1);
float time = 0;
int stage = 0;
bool paused = false;
bool loop = true;
int state = 1;
int phases = 0;

int objIterator = 0;
float velocity = 0.00072;
float distance = 0;


void dumpCurve()
{
	while (!curve.empty())
		curve.pop_back();
}

float arclength(int curveIndex)
{
	Vector3 d = curve[curveIndex].vec + (curve[curveIndex - 1].vec * -1);
	float ad = sqrt(pow(d.x, 2) + pow(d.y, 2) + pow(d.z, 2));
	return ad + curve[curveIndex - 1].arcLength;
}

float ConvertRange(float originalStart, float originalEnd, float newStart, float newEnd, float value)
{
	float originalDiff = originalEnd - originalStart;
	float newDiff = newEnd - newStart;
	float ratio = newDiff / originalDiff;
	float newProduct = value * ratio;
	float finalValue = newProduct + newStart;
	return finalValue;
}

template <typename T>
T lerp(T d0, T d1, float t)
{
	return d0 * (1 - t) + d1 * t;
}

//This takes three variables, the begin, and the end of the LERP, and the result of the LERP to find the 't' (Time) value
template <typename T>
float inverselerp(T d0, T d1, T res)
{
	return (res + (d0*-1)) / (d1 + (d0*-1));
}

//Takes in a time variable and assigns the object the correct position depending on the LERP mode
void interpolate(float dTime)
{
	if (coordsVect.size() != 0)
	{
		std::vector<Vector3> lerpStorage;
		if (stage < coordsVect.size() - 1 && state == 1)
		{
			objectVect = lerp(coordsVect[stage], coordsVect[stage + 1], time);
		}

		if (stage < coordsVect.size() - 2 && state == 2)
		{
			//LERP the first line
			lerpStorage.push_back(lerp(coordsVect[stage], coordsVect[stage + 1], time));
			//LERP the second line
			lerpStorage.push_back(lerp(coordsVect[stage + 1], coordsVect[stage + 2], time));
			//LERP between the two new lines for our object
			objectVect = lerp(lerpStorage[0], lerpStorage[1], time);


			//draw the lines
			glBegin(GL_LINES);
				glColor3f(0.556, 1.0, 0.7);
				glVertex3f(lerpStorage[0].x, lerpStorage[0].y, lerpStorage[0].z);
				glVertex3f(lerpStorage[1].x, lerpStorage[1].y, lerpStorage[1].z);
			glEnd();

			//empty the lerpStorage
			lerpStorage.pop_back();
			lerpStorage.pop_back();
		}

		if (stage < coordsVect.size() - 1 && state == 3)
		{
			//
			float t = time;
			float t2 = time*time;
			float t3 = time*time*time;

			/* Catmull Calculation */

			//create vectors at 0,0,0 incase issues
			Vector3 prev(0, 0, 0);
			Vector3 last(0, 0, 0);
			//check to make sure that there is a point
			if (stage == 0)
			{
				//if there is no previous point use the first point
				prev = coordsVect[stage];
			}
			else
			{
				prev = coordsVect[stage - 1];
			}

			//check the last point
			if (stage + 2 == coordsVect.size())
			{
				last = coordsVect[coordsVect.size() - 1];
			}
			else
			{
				last = coordsVect[stage + 2];
			}
			//CATMULL ROM
			objectVect.x = ((-t3 + 2 * t2 - t)*(prev.x) + (3 * t3 - 5 * t2 + 2)*(coordsVect[stage].x) + (-3 * t3 + 4 * t2 + t)* (coordsVect[stage + 1].x) + (t3 - t2)*(last.x)) / 2;
			objectVect.y = ((-t3 + 2 * t2 - t)*(prev.y) + (3 * t3 - 5 * t2 + 2)*(coordsVect[stage].y) + (-3 * t3 + 4 * t2 + t)* (coordsVect[stage + 1].y) + (t3 - t2)*(last.y)) / 2;
		}

		if (stage + 3 < coordsVect.size() && state == 4)
		{
			//First Stage LERP
			//create the first line
			lerpStorage.push_back(lerp(coordsVect[stage], coordsVect[stage + 1], time));
			lerpStorage.push_back(lerp(coordsVect[stage + 1], coordsVect[stage + 2], time));
			//create the second line
			lerpStorage.push_back(lerp(coordsVect[stage + 2], coordsVect[stage + 3], time));

			//Second Stage LERP
			//lerp the first line
			lerpStorage.push_back(lerp(lerpStorage[0], lerpStorage[1], time));
			//lerp the second line
			lerpStorage.push_back(lerp(lerpStorage[1], lerpStorage[2], time));

			//lerp the final position
			objectVect = lerp(lerpStorage[3], lerpStorage[4], time);


			//Draw our lines
			glBegin(GL_LINES);
			glColor3f(1.0, 0.741, 0.298);
			glVertex3f(lerpStorage[0].x, lerpStorage[0].y, lerpStorage[0].z);
			glVertex3f(lerpStorage[1].x, lerpStorage[1].y, lerpStorage[1].z);
			glEnd();

			glBegin(GL_LINES);
			glColor3f(1.0, 0.407, 0.427);
			glVertex3f(lerpStorage[1].x, lerpStorage[1].y, lerpStorage[1].z);
			glVertex3f(lerpStorage[2].x, lerpStorage[2].y, lerpStorage[2].z);
			glEnd();

			glBegin(GL_LINES);
			glColor3f(1.0, 0.478, 0.9);
			glVertex3f(lerpStorage[3].x, lerpStorage[3].y, lerpStorage[3].z);
			glVertex3f(lerpStorage[4].x, lerpStorage[4].y, lerpStorage[4].z);
			glEnd();

			//empty the storage
			lerpStorage.pop_back();
			lerpStorage.pop_back();
			lerpStorage.pop_back();
			lerpStorage.pop_back();
			lerpStorage.pop_back();
		}

		//Looping for lerp and catmull rom, make sure there is a point after the current phase
		if (phases >= coordsVect.size() - 1)
		{
			if (loop)
			{
				stage = 0;
				time = 0;
				phases = 0;
			}
			curveCalculated = true;
		}
		else if (state == 2)
		{
			//if the size is odd
			if (coordsVect.size() % 2 == 0)
			{
				//and there are not enough valid points to lerp again
				if (phases >= coordsVect.size() - 3)
				{
					if (loop)
					{
						stage = 0;
						time = 0;
						phases = 0;
					}
					curveCalculated = true;
				}
			}
			//do it again
			else if (phases >= coordsVect.size() - 2)
			{
				if (loop)
				{
					stage = 0;
					time = 0;
					phases = 0;
				}
				curveCalculated = true;
			}
		}
		//cubic bezier loop checking
		else if (state == 4)
		{
			//check to see if the last points are invalid
			//since the phase increments by 3 each time there wont be an issue with valid ends
			if (phases >= coordsVect.size() - 3)
			{
				if (loop)
				{
					stage = 0;
					time = 0;
					phases = 0;
				}
				curveCalculated = true;
			}
		}

		//increment time by the framerate
		//the dTime is in miliseconds and is converted to seconds
		time += (float)dTime / 1000;
		if (time >= 1.0f)
		{
			//reset time
			time = 0;
			//bezier increments by 2 since it requires 2 lines
			if (state == 2)
			{
				stage += state;
				phases += state;
			}
			//cubic increments by 3 since it requires 3 lines
			else if (state == 4)
			{
				stage += 3;
				phases += 3;
			}
			//catmul and linear both increment by 1
			else
			{
				stage++;
				phases++;
			}
		}
	}
}

//This creates the arc length table. 
void remakeCurve()
{
	dumpCurve(); //Empties the curve
	curveCalculated = false; //resets the variable of whether it's calculated or not
	while (!curveCalculated && coordsVect.size() > 1) // calculates until the curve is complete
	{
		interpolate(FRAME_DELAY);
		CurveEntry c;
		c.vec = objectVect;
		c.segmentNumber = stage;
		c.time = time;
		c.arcLength = 0;
		curve.push_back(c);//Stores the arc length entry into vectorCurve
	}
	//This computes the arc length
	for (int i = 1; i < curve.size(); i++)
	{
		curve[i].arcLength = arclength(i);
	}
	//This normalizes all the lengths
	for (int i = 0; i < curve.size(); i++)
	{
		curve[i].arcLength /= curve.back().arcLength;
	}
}

//Resets and also remakes the curve
void reset()
{
	stage = 0;
	time = 0;
	objectVect = coordsVect[0];
	phases = 0;
	curveCalculated = false;
	remakeCurve();
}

void DisplayCallbackFunction(void)
{
	/* clear the screen */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // clear our the transform matrix

	if (!paused)
	{
		if (speedControl && curve.size() > 1) //Checking to make sure that we are speed controlling and that the curve is of a proper size
		{
			float lerpdVal = lerp(0.0f, 1.0f, distance); //This will determine the value that we need to pass to be sent into the inverse LERP functions
			int p1 = 0, p2 = 0;
			for (int i = 1; i < curve.size(); i++) //Calculates the closest two points to our current time
			{
				if (curve[i].arcLength > lerpdVal)
				{
					p2 = i;
					p1 = i - 1;
					break;
				}
			}

			float inversel = inverselerp(curve[p1].arcLength, curve[p2].arcLength, lerpdVal); //Returns the time for the point that we want
			objectVect = lerp(curve[p1].vec, curve[p2].vec, inversel); //Assigns the object's position to our new point

			distance += velocity; //Increments the distance
			if (distance > 1)
				distance = 0;
		}
		else //otherwise, revert to normal
		{
			interpolate(FRAME_DELAY);
		}
	}

	/* This is where we draw things */


	//Transformations

	//glTranslatef(0.f, 0.f, -200.f);
	//glRotatef(rotx, 0.f, 1.f, 0.f);
	//glRotatef(roty, 1.f, 0.f, 0.f);
	//glRotatef(rotz, 0.f, 0.f, 0.f);


	glBegin(GL_LINE_STRIP);
		glColor3f(0.576, 0.592, 1.0);
		for (unsigned int i = 0, s = coordsVect.size() - 1; i < s; i++)
		{
			glVertex3f(coordsVect[i].x, coordsVect[i].y, -1);
		}
		if (state == 2 && coordsVect.size() % 2 == 0)
			//if we cannot lerp to the final point, make it a brighter blue
			glColor3f(0.419, 0.87, 1.0);
		if (state == 4 && (coordsVect.size() - 4) % 3 != 0)
			glColor3f(0.2, 0.2, 0.2);
		glVertex3f(coordsVect[coordsVect.size() - 1].x, coordsVect[coordsVect.size() - 1].y, -1);
	glEnd();


	/*glBegin(GL_LINES);
	glVertex3f(0, 0, -1);
	glVertex3f(1.5, 1.1, -1);
	glEnd();*/

	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.843f, 0.419f);
		glVertex3f(objectVect.x - 0.1, objectVect.y - 0.05, -1);
		glVertex3f(objectVect.x - 0.05, objectVect.y + 0.05, -1);
		glVertex3f(objectVect.x + 0.05, objectVect.y + 0.05, -1);
		glVertex3f(objectVect.x + 0.1, objectVect.y - 0.05, -1);
	glEnd();




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
	//case 32: // the space bar
	//	paused == 0 ? paused = true : paused = false;
	//	break;
	case 'c':
	case 'C':
		reset(); //resets the LERP
		break;
	case 'e':
	case 'E':
		while (!coordsVect.empty())
			coordsVect.pop_back();
		coordsVect.push_back(objectVect);
		stage = 0;
		time = 0; // empties the vector, adds the objectVector as position 0, resets stage and time
		break;
	/*case 'o':
	case 'O': //disables / enables looping
		loop == 0 ? loop = true : loop = false;
		std::cout << loop << "\n";
		break;*/
	case 'w':
	case 'W': //Linear
		state = 1;
		reset();
		break;
	case 'a':
	case 'A': // Bezier
		state = 2;
		reset();
		break;
	case 's':
	case 'S': //Catmull rom
		state = 3;
		reset();
		break;
	case 'd':
	case 'D': //Cubic Bezier
		state = 4;
		reset();
		break;
	case ' ': //This toggles between LERP and Speed control for LERP
		speedControl = !speedControl;
		break;
	case '-': //Slows down (Halves the speed)
		velocity /= 2;
		break;
	case '=': //Speeds up (Doubles the speed)
		velocity *= 2;
		break;
	case 27: // the escape key
	case 'q': // the 'q' key
	case 'Q':
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

void idleFunc()
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
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	// switch to projection because we're changing projection
	float asp = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Setup a perspective projection
	gluPerspective(90.f, asp, 1.f, 500.f); // (fov, aspect, near z, far z)	

	//gluOrtho2D(0, w, 0, h);

	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);

	//switch back to modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		float nx = ConvertRange(0, (float)windowWidth, -1.5, 1.5, (float)x);
		float ny = ConvertRange(0, (float)windowHeight, -1.1, 1.1, (float)y);
		std::cout << "Mouse x:" << nx << " y:" << -ny << std::endl;
		coordsVect.push_back(Vector3(nx, -ny, -1));

		remakeCurve(); //We need to caluclate the curve for every new point
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

void init()
{
	///// INIT OpenGL /////
	// Set color clear value
	glClearColor(0.717f, 0.172f, 0.509f, 1.f);

	// Enable Z-buffer read and write (for hidden surface removal)
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D); // textures for future use

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
	glutCreateWindow("INFR1350U - Example");

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);

	init(); //Setup OpenGL States

	coordsVect.push_back(objectVect);

	/* start the event handler */
	glutMainLoop();
	return 0;
}