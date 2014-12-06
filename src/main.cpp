#include "ViconRTClient.h"
#include "imageloader.h"
#include "Matrices.h"

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>


/* -------------------- variables -------------------- */

// Vicon variables
#define DEFAULT_IP "172.21.12.17"
ViconRTClient viconClient;

string headName = "Head3:Head3";
string tabName = "Tab3:Tab3";

// GL variables
int windowID;
int Win[2];

GLuint _textureId0;	// not a good way to define textures. use a vector or something for dynamically adding textures
GLuint _textureId1;
GLuint _textureId2;
GLuint _textureId3;
GLuint _textureId4;
GLuint _textureId5;
GLuint _textureId6;
GLuint _textureId7;
GLuint _textureId8;
GLuint _textureId9;
GLuint _textureId10;
GLuint _textureId11;
GLuint _textureId12;
GLuint _textureId13;

// Input variables 
int lastClickedX = 0;	// not used yet
int lastClickedY = 0;	// not used yet
int currentX = 0;	// not used yet
int currentY = 0;	// not used yet
int currentZ = -40;

// Transformation variables
GLfloat camTrans_X = 0.0;
GLfloat camTrans_Y = 0.0;
GLfloat camTrans_Z = 0.0;
GLfloat camRot_angle = 0.0;
GLfloat camRot_X = 0.0;
GLfloat camRot_Y = 0.0;
GLfloat camRot_Z = 0.0;

GLfloat tabTrans_X = 0.0;
GLfloat tabTrans_Y = 0.0;
GLfloat tabTrans_Z = 0.0;
GLfloat tabRot_angle = 0.0;
GLfloat tabRot_X = 0.0;
GLfloat tabRot_Y = 0.0;
GLfloat tabRot_Z = 0.0;

/* --------------- function definitions -------------- */
const float PI = 3.141592;
void initGlut(char* winName);
void initGl();

void idle(void);
void myReshape(int w, int h);
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

void drawUnitFrustum();
void drawUnitCube();
void drawUnitSquare();
GLuint loadTexture(Image* image) ;

/* --------------- function declarations -------------- */

int main(int argc, char** argv) {
	if(argc == 4) {
		Win[0] = atoi(argv[1]);
		Win[1] = atoi(argv[2]);
	} else 
	if (argc == 3){
		Win[0] = atoi(argv[1]);
		Win[1] = atoi(argv[2]);
		argv[3] = DEFAULT_IP;
		printf("Using default IP: %s \n", DEFAULT_IP);
	} else {
		Win[0] = 600;
		Win[1] = 400;
		argv[3] = DEFAULT_IP;
		printf("Using default window of size 600x400\n");
		printf("Using default IP: %s \n", DEFAULT_IP);
	}

	if (viconClient.initialize(argv[3])) return 1;

	glutInit(&argc, argv);
	initGlut(argv[0]);
	initGl();

	viconClient.getChannelInfo();

	glutMainLoop();

	return 0;
}

void initGlut(char* winName) {
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition (0, 0);
	glutInitWindowSize(Win[0],Win[1]);
	windowID = glutCreateWindow(winName);

	glutIdleFunc(idle);
	glutReshapeFunc(myReshape); 
	glutDisplayFunc(display); 
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
}

void initGl(void) {
	glClearColor(0.7f,0.7f,0.9f,1.0f);

	Image* image0 = loadBMP("../res/images/background.bmp");
	Image* image1 = loadBMP("../res/images/chrome.bmp");
	Image* image2 = loadBMP("../res/images/PLAY.bmp");
	Image* image3 = loadBMP("../res/images/skydrive.bmp");
	Image* image4 = loadBMP("../res/images/plus.bmp");
	Image* image5 = loadBMP("../res/images/facebook.bmp");
	Image* image6 = loadBMP("../res/images/maps.bmp");
	Image* image7 = loadBMP("../res/images/hangouts.bmp");
	Image* image8 = loadBMP("../res/images/gmail.bmp");
	Image* image9 = loadBMP("../res/images/earth.bmp");
	Image* image10 = loadBMP("../res/images/calendar.bmp");
	Image* image11 = loadBMP("../res/images/blogger.bmp");
	Image* image12 = loadBMP("../res/images/appengine.bmp");
	Image* image13 = loadBMP("../res/images/tab.bmp");

	_textureId0 = loadTexture(image0);
	_textureId1 = loadTexture(image1);
	_textureId2 = loadTexture(image2);
	_textureId3 = loadTexture(image3);
	_textureId4 = loadTexture(image4);
	_textureId5 = loadTexture(image5);
	_textureId6 = loadTexture(image6);
	_textureId7 = loadTexture(image7);
	_textureId8 = loadTexture(image8);
	_textureId9 = loadTexture(image9);
	_textureId10 = loadTexture(image10);
	_textureId11 = loadTexture(image11);
	_textureId12 = loadTexture(image12);
	_textureId13 = loadTexture(image13);
	delete image1, image2, image3, image4, image5, image6, image7, image8, image9, image10, image11, image12, image13;
}

void idle(void) {
	viconClient.getFrame();

	BodyData head = viconClient.getBodyData(headName);
	camTrans_X = head.TX;
	camTrans_Y = head.TY;
	camTrans_Z = head.TZ;
	camRot_angle = sqrtf(head.RX*head.RX + head.RY*head.RY + head.RZ*head.RZ);
	if(camRot_angle){
		camRot_X = head.RX/camRot_angle;
		camRot_Y = head.RY/camRot_angle;
		camRot_Z = head.RZ/camRot_angle;
	}
	//cout << " (" << camTrans_X << ", " << camTrans_Y << ", " << camTrans_Z << ")" << endl;
	//cout << " cam rotation (" << camRot_angle*180/PI << ")" << endl;
	
	BodyData tab = viconClient.getBodyData(tabName);
	tabTrans_X = tab.TX - camTrans_X; // Translations are relative to camera
	tabTrans_Y = tab.TY - camTrans_Y;
	tabTrans_Z = tab.TZ - camTrans_Z;
	tabRot_angle = sqrtf(tab.RX*tab.RX + tab.RY*tab.RY + tab.RZ*tab.RZ);
	if(tabRot_angle){
		tabRot_X = tab.RX/tabRot_angle;
		tabRot_Y = tab.RY/tabRot_angle;
		tabRot_Z = tab.RZ/tabRot_angle;
	}
	//cout << "tab trans (" << tabTrans_X << ", " << tabTrans_Y << ", " << tabTrans_Z << ")" << endl;
	//cout << " tab rot (" << tabRot_angle*180/PI << ")" << endl;
	//viconClient.showBodies(VERBOSE);
	
	glutSetWindow(windowID);
	glutPostRedisplay();
}

void myReshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, w/h, 0.5, 1000);

	glViewport(0,0, w,h);
	Win[0] = w;
	Win[1] = h;
}

void display(void) {
	Matrix4 transMatrix;
	//Vector3 globalTranslation(0.0, 0.0, -10);
	//Vector3 localTranslation;
	//Vector3 globalRotation();
	//Vector3 localRotation;

	glClearColor(0.7f,0.7f,0.9f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_TEXTURE_2D);
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// TODO: Defining the background. To be removed after getting AR glasses
	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// Apply all transformations applied for objects here as well
		glRotatef(-camRot_angle*180/PI, -camRot_Y, camRot_Z, -camRot_X);
		glScalef(2.5, 1.5,1.0);
		glBindTexture(GL_TEXTURE_2D, _textureId0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glPushMatrix();
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(90, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(180, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-90, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(90, 1.0, 0.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-90, 1.0, 0.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			drawUnitSquare();
		glPopMatrix();
	glPopMatrix();
	//glEnable(GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	/////////////////////////////////////////////////////////////////////////////////////////////

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		// Camera
		glRotatef(-camRot_angle*180/PI, -camRot_Y, camRot_Z, -camRot_X);

		// Tab
		glPushMatrix();		
			glTranslatef(-tabTrans_Y/30, tabTrans_Z/30, -tabTrans_X/30);
			glRotatef(tabRot_angle*180/PI, -tabRot_Y, tabRot_Z, -tabRot_X);	
			glRotatef(-90, 1.0, 0.0, 0.0);	
			glScalef(6.0, 3.0, 1.0);
			glPushMatrix();	
				glBindTexture(GL_TEXTURE_2D, _textureId13);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				drawUnitSquare();
			glPopMatrix();
			glPushMatrix();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(3.0);
				glPushMatrix();
					glTranslatef(2.0, 0.0, 0.0);
					drawUnitSquare();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(-2.0, 0.0, 0.0);
					drawUnitSquare();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0.0, 2.0, 0.0);
					drawUnitSquare();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0.0, -2.0, 0.0);
					drawUnitSquare();
				glPopMatrix();
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPopMatrix();
		glPopMatrix();

		// Apps
		glPushMatrix();
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(30.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId2);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(60.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId3);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();

		glPushMatrix(); ///////////////////////////////////
			//glTranslatef(-20.0, 0.0, -30.0);
			//transMatrix.identity();
			
			//transMatrix.rotate(90.0, 0.0, 1.0, 0.0);
			glTranslatef(currentZ, 0.0, 0.0);
			glRotatef(90.0, 0.0, 1.0, 0.0);
			//transMatrix.translate(0.0, 0.0, currentZ);
			//glRotatef(30.0, -1.0, 0.0, 0.0);
			//glRotatef(30.0, 0.0, 0.0, -1.0);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId4);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix(); ////////////////////////////////////

		glPushMatrix();
			glRotatef(120.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId5);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(150.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId6);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(180.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId7);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-30.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-60.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId9);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId10);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-120.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId11);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();
		glPushMatrix();
			glRotatef(-150.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, currentZ);
			glScalef(5.0, 5.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, _textureId12);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			drawUnitSquare();
		glPopMatrix();

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//lastClickedX = x;
		//lastClickedY = y;
	} 
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
	}
	else if(button == 3)
	{
		currentZ = currentZ - 1.0;
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
	else if(button == 4)
	{
		currentZ = currentZ + 1.0;
		glutSetWindow(windowID);
		glutPostRedisplay();
	}

}

void motion(int x, int y) {
	//currentX = x;
	//currentY = y;
	//glutSetWindow(windowID);
	//glutPostRedisplay();
}

void drawUnitFrustum() {
	glBegin(GL_QUADS);
	// draw front face
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f( 1.0, -1.0, 1.0);
	glVertex3f( 0.75,  1.0, 0.75);
	glVertex3f(-0.75,  1.0, 0.75);

	// draw back face
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-0.75,  1.0, -0.75);
	glVertex3f( 0.75,  1.0, -0.75);

	// draw left face
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0,  1.0);
	glVertex3f(-0.75,  1.0,  0.75);
	glVertex3f(-0.75,  1.0, -0.75);

	// draw right face
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f( 1.0, -1.0,  1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f( 0.75,  1.0, -0.75);
	glVertex3f( 0.75,  1.0,  0.75);

	// draw top
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-0.75,  1.0,  0.75);
	glVertex3f( 0.75,  1.0,  0.75);
	glVertex3f( 0.75,  1.0, -0.75);
	glVertex3f(-0.75,  1.0, -0.75);

	// draw bottom
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f( 1.0, -1.0,  1.0);
	glVertex3f(-1.0, -1.0,  1.0);
	glEnd();
}

void drawUnitCube() {
	glBegin(GL_QUADS);
	// draw front face
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f( 1.0, -1.0, 1.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f( 1.0,  1.0, 1.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0,  1.0, 1.0);

	// draw back face
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0,  1.0, -1.0);
	glVertex3f( 1.0,  1.0, -1.0);

	// draw left face
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0,  1.0);
	glVertex3f(-1.0,  1.0,  1.0);
	glVertex3f(-1.0,  1.0, -1.0);

	// draw right face
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f( 1.0, -1.0,  1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f( 1.0,  1.0, -1.0);
	glVertex3f( 1.0,  1.0,  1.0);

	// draw top
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-1.0,  1.0,  1.0);
	glVertex3f( 1.0,  1.0,  1.0);
	glVertex3f( 1.0,  1.0, -1.0);
	glVertex3f(-1.0,  1.0, -1.0);

	// draw bottom
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f( 1.0, -1.0,  1.0);
	glVertex3f(-1.0, -1.0,  1.0);
	glEnd();
}

void drawUnitSquare() {
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
}

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width, image->height,
		0, 
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);               
	return textureId;
}