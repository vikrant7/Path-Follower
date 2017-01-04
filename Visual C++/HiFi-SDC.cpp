#include <GL/glut.h>
#include<Windows.h>
#include<GL/freeglut.h>
#include<fstream>
#include<iostream>
#include<vector>
#include<math.h>
#include <cstdlib>
#include<dos.h>
#include<stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "tserial.h"
#include "bot_control.h"
serial comm; //serial is a class type defined in these files, used for referring to the communication device
using namespace std;
GLint gFramesPerSecond = 0;
class coordinates
{
public:
	double X;double Y;
	};
vector<double> steps;
vector<char> command;
vector<coordinates> points;
GLsizei MOUSEx = 0, MOUSEy = 0;
GLfloat SIDE = 5;
GLfloat BLUE[3] = { 0,0,1 };
GLfloat RED[3] = { 1,0,0 };
double k1=1, k2=1;
double distance(coordinates P1, coordinates P2)
{
	coordinates difference;
	difference.X = P1.X - P2.X;
	difference.Y = P1.Y - P2.Y;
	double Distance = sqrt(pow(difference.X, 2) + pow(difference.Y, 2));
	double forwardStep = k1*Distance;
	command.push_back('f');
	steps.push_back(forwardStep);
	return forwardStep;
}
double angle(double &theta, coordinates P1, coordinates P2)
{
	double theta2 = theta;
	//coordinates difference;
	double differenceX = P2.X - P1.X;
	double differenceY = P2.Y - P1.Y;
	double phi = atan2(differenceY, differenceX);
	double phi1 = phi * 180 / 3.14;
	theta = phi1;
	double anglesteps = k2*(phi1 - theta2);
	if (anglesteps > 0) command.push_back('r');
	else command.push_back('l');
	steps.push_back(anglesteps);
	return anglesteps;
}
void FPS(void) {
	static GLint Frames = 0;         // frames averaged over 1000mS
	static GLuint Clock;             // [milliSeconds]
	static GLuint PreviousClock = 0; // [milliSeconds]
	static GLuint NextClock = 0;     // [milliSeconds]

	++Frames;
	Clock = glutGet(GLUT_ELAPSED_TIME); //has limited resolution, so average over 1000mS
	if (Clock < NextClock) return;

	gFramesPerSecond = Frames / 1; // store the averaged number of frames per second

	PreviousClock = Clock;
	NextClock = Clock + 1000; // 1000mS=1S in the future
	Frames = 0;
}


void timer(int value)
{
	const double desiredFPS = 1000000;
	glutTimerFunc(1000 / desiredFPS, timer, ++value);

	//put your specific idle code here
	//... this code will run at desiredFPS
	//char spinner[] = { '|','/','-','~','\\' };
	//printf("%c", spinner[value % sizeof(spinner) / sizeof(char)]);
	//end your specific idle code here

	FPS(); //only call once per frame loop to measure FPS 
	glutPostRedisplay();
}

void drawSquare1(GLsizei MOUSEx ,GLsizei MOUSEy)
{
	glColor3fv(RED);
	glBegin(GL_POLYGON);
	glVertex3f(MOUSEx, MOUSEy, 0);
	glVertex3f(MOUSEx + SIDE, MOUSEy, 0);
	glVertex3f(MOUSEx + SIDE, MOUSEy + SIDE, 0);
	glVertex3f(MOUSEx, MOUSEy + SIDE, 0);
	glEnd();
	//glFlush();
}
void draw_dda(coordinates p1, coordinates p2) {
	GLfloat dx = p2.X - p1.X;
	GLfloat dy = p2.Y - p1.Y;

	GLfloat x1 = p1.X;
	GLfloat y1 = p1.Y;

	GLfloat step = 0;

	if (abs(dx) > abs(dy)) {
		step = abs(dx);
	}
	else {
		step = abs(dy);
	}

	GLfloat xInc = dx / step;
	GLfloat yInc = dy / step;

//	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (float i = 1; i <= step; i++) {
		glVertex2i(x1, y1);
		x1 += xInc;
		y1 += yInc;
	}
	glEnd();
	//glFlush();
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	for (int u = 0; u < points.size(); u++)
	{
		drawSquare1(points[u].X, points[u].Y);
		
	}
	for (int u = 1; u < points.size(); u++)
	{
		draw_dda(points[u-1], points[u]);
	}

	glFlush();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glOrtho(0.0, 1366, 768, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void spindisplay(void)
{
	glutPostRedisplay();
	//glutHideOverlay();
	//glutUseLayer(GLUT_NORMAL);
}

void setX(int x)
{
	MOUSEx = x;
}

void setY(int y)
{
	MOUSEy = y;
}
void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		setX(x);
		setY(y);
		//drawSquare(MOUSEx,HEIGHT-MOUSEy);
		coordinates P;
		P.X = x;P.Y = y;
		points.push_back(P);
		//glutPostRedisplay();
	}
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		double theta = 0;
		for (int i = 0;i < points.size()-1; i++)
		{
			//myfile << points[i].X << "\t" << points[i].Y << endl;
			angle(theta, points[i], points[i + 1]);
			distance(points[i], points[i + 1]);
		}
				
	ofstream myfile;
		myfile.open("commands5.txt");
		for (int i = 0;i < 2*(points.size()-1); i++)
		{
			myfile << command[i] << "\t" << steps[i] << endl<<i<<endl;
		}
		myfile.close();
		comm.startDevice("COM6", 9600);
		/* “COM 6” refers to the com port in which the USB to SERIAL port is attached. It is shown by right clicking on my computer, then going to properties and then device manager
		9600 is the baud-rate in bits per second */
		for (int i = 0;i < 2 * (points.size() - 1);i++) {
			if (command[i] == 'f') {
				comm.send_data(command[i]);
				Sleep(2000);
				comm.send_data(steps[i]/6);
				//Sleep(2*(steps[i]+100));
				Sleep(10000);
			}
			else {
				comm.send_data(command[i]);
				Sleep(2000);
				comm.send_data(abs(steps[i])*32/9);
				//Sleep(2*((abs(steps[i]) * 32 / 9) +100));
				Sleep(10000);
			}
		}		  //comm.stopDevice(); 
		exit(1);   // To Exit the Program
	}
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1366, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("GUI");
	coordinates Q;
	Q.X = 0;Q.Y = 0;
	points.push_back(Q);
	//glutTimerFunc(0, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(spindisplay);
	glutMainLoop();	
}


