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
#define DEFAULT_IP "172.21.18.192"
ViconRTClient viconClient;

string headName = "Head3:Head3";
string tabName = "Tab3:Tab3";
string handRootName = "hand:Root";

string indexFingername = "hand:Index";
string thumbName = "hand:Thumb";

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

bool obj1Selected = false;
bool objectGrabbed = false;

// Input variables 
int lastClickedX = 0;	// not used
int lastClickedY = 0;	// not used
int currentX = 0;	// not used
int currentY = 0;	// not used
int currentZ = -40;

// Transformation variables
GLfloat camTrans_X = 0.0;
GLfloat camTrans_Y = 0.0;
GLfloat camTrans_Z = 0.0;
GLfloat camRot_angle = 0.0;
GLfloat camRot_X = 0.0;
GLfloat camRot_Y = 0.0;
GLfloat camRot_Z = 0.0;

GLfloat cursorTrans_X = 0.0;
GLfloat cursorTrans_Y = 0.0;
GLfloat cursorTrans_Z = 0.0;
GLfloat cursorRot_angle = 0.0;
GLfloat cursorRot_X = 0.0;
GLfloat cursorRot_Y = 0.0;
GLfloat cursorRot_Z = 0.0;

GLfloat tabTrans_X = 0.0;
GLfloat tabTrans_Y = 0.0;
GLfloat tabTrans_Z = 0.0;
GLfloat tabRot_angle = 0.0;
GLfloat tabRot_X = 0.0;
GLfloat tabRot_Y = 0.0;
GLfloat tabRot_Z = 0.0;

GLfloat obj1Trans_X = 0.0;
GLfloat obj1Trans_Y = 0.0;
GLfloat obj1Trans_Z = -600.0;
GLfloat obj1Rot_angle = 90.0;
GLfloat obj1Rot_X = 1.0;
GLfloat obj1Rot_Y = 0.0;
GLfloat obj1Rot_Z = 0.0;
GLfloat obj1Scale = 2.0;



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
	camTrans_Y = head.TZ;
	camTrans_Z = -head.TY;
	camRot_angle = sqrtf(head.RX*head.RX + head.RY*head.RY + head.RZ*head.RZ);
	if(camRot_angle){
		camRot_X = head.RX/camRot_angle;
		camRot_Y = head.RZ/camRot_angle;
		camRot_Z = -head.RY/camRot_angle;
	}

	BodyData tab = viconClient.getBodyData(tabName);
	tabTrans_X = tab.TX - camTrans_X; // Translations are relative to camera
	tabTrans_Y = tab.TZ - camTrans_Y;
	tabTrans_Z = -(tab.TY + camTrans_Z);
	tabRot_angle = sqrtf(tab.RX*tab.RX + tab.RY*tab.RY + tab.RZ*tab.RZ);
	if(tabRot_angle){
		tabRot_X = tab.RX/tabRot_angle;
		tabRot_Y = tab.RZ/tabRot_angle;
		tabRot_Z = -tab.RY/tabRot_angle;
	}

	BodyData handRoot = viconClient.getBodyData(handRootName);
	cursorTrans_X = handRoot.TX - camTrans_X; // Translations are relative to camera
	cursorTrans_Y = handRoot.TZ - camTrans_Y;
	cursorTrans_Z = -(handRoot.TY + camTrans_Z);
	cursorRot_angle = sqrtf(handRoot.RX*handRoot.RX + handRoot.RY*handRoot.RY + handRoot.RZ*handRoot.RZ);
	if(cursorRot_angle){
		cursorRot_X = handRoot.RX/cursorRot_angle;
		cursorRot_Y = handRoot.RZ/cursorRot_angle;
		cursorRot_Z = -handRoot.RY/cursorRot_angle;
	}
	// for each object
	GLfloat distanceToObj = (cursorTrans_X - obj1Trans_X)*(cursorTrans_X - obj1Trans_X)
		+ (cursorTrans_Y - obj1Trans_Y)*(cursorTrans_Y - obj1Trans_Y)
		+ (cursorTrans_Z - obj1Trans_Z)*(cursorTrans_Z - obj1Trans_Z);
	if(distanceToObj != 0){
		distanceToObj = sqrt(distanceToObj);
	}
	if(distanceToObj < 50){
		obj1Selected = true;
		cout << "Object selected" << endl;
	} else{
		obj1Selected = false;
	}
	// end of for

	MarkerData thumb = viconClient.getMarkerData(thumbName);
	MarkerData indexFinger = viconClient.getMarkerData(indexFingername);
	if(thumb.Visible && indexFinger.Visible ){
		GLfloat fingerDistance = (thumb.X - indexFinger.X)*(thumb.X - indexFinger.X)
			+ (thumb.Y - indexFinger.Y)*(thumb.Y - indexFinger.Y)
			+ (thumb.Z - indexFinger.Z)+(thumb.Z - indexFinger.Z);
		if(fingerDistance != 0){
			fingerDistance = sqrt(fingerDistance);
		}
		if(obj1Selected && fingerDistance < 80){
			objectGrabbed = true;
			cout << "GRABBED.." << endl;
		} else {
			objectGrabbed = false;
		}
	} else {
		objectGrabbed = false;
	}

	if(objectGrabbed){
		obj1Trans_X = cursorTrans_X;
		obj1Trans_Y = cursorTrans_Y;
		obj1Trans_Z = cursorTrans_Z;
		obj1Rot_X = handRoot.RX/obj1Rot_angle;
		obj1Rot_Y = handRoot.RZ/obj1Rot_angle;
		obj1Rot_Z = -handRoot.RY/obj1Rot_angle;
	}

	// Vicon data testing
	//cout << "hand trans (" << handRoot.TX << ", " << handRoot.TY << ", " << handRoot.TZ << ")" << endl;
	//cout << " hand rot (" << obj1Rot_angle*180/PI << ")" << endl;
	//cout << obj1Rot_X << ", " << obj1Rot_Y << ", " << obj1Rot_Z << ")" << endl;

	//cout << "Start.........................................." << endl;
	//viconClient.showBodies(VERBOSE);
	//viconClient.showMarkers(VERBOSE);
	//cout << "End.........................................." << endl;


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
		glRotatef(-camRot_angle*180/PI, camRot_X, camRot_Y, camRot_Z);
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
		glRotatef(-camRot_angle*180/PI, camRot_X, camRot_Y, camRot_Z);

		// Cursor
		glPushMatrix();
			glDisable(GL_TEXTURE_2D);
			glTranslatef(cursorTrans_X/30, cursorTrans_Y/30, cursorTrans_Z/30);
			glRotatef(cursorRot_angle*180/PI, cursorRot_X, cursorRot_Y, cursorRot_Z);
			glRotatef(-90, 1.0, 0.0, 0.0);	
			glScalef(1.0, 1.0, 1.0);
			drawUnitCube();
			glEnable(GL_TEXTURE_2D);
		glPopMatrix();

		// Tab
		glPushMatrix();		
			glTranslatef(tabTrans_X/30, tabTrans_Y/30, tabTrans_Z/30);
			glRotatef(tabRot_angle*180/PI, tabRot_X, tabRot_Y, tabRot_Z);	
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
			glTranslatef(obj1Trans_X/30, obj1Trans_Y/30, obj1Trans_Z/30);
			glRotatef(obj1Rot_angle*180/PI, obj1Rot_X, obj1Rot_Y, obj1Rot_Z);
			glRotatef(-90, 1.0, 0.0, 0.0);	
			glScalef(obj1Scale, obj1Scale, 1.0);
			glPushMatrix();	
				glBindTexture(GL_TEXTURE_2D, _textureId4);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				drawUnitSquare();
			glPopMatrix();
			if(obj1Selected){
				glPushMatrix();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(30.0);
				drawUnitSquare();
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPopMatrix();
			}
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