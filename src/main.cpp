/**
Implementa a movimentação simples de uma camera em primeira pessoa para um personagem que anda
sobre um plano.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

//#include <mmsystem.h>
//#pragma comment(lib, "winmm.lib")

#define PI 3.14159265

#include "../../src/ModelPorsche.h"

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

void mainInit();
void initSound();
void initTexture();
void initLight();
void createGLUI();
void mainRender();
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();
void initModel();


/**
Screen dimensions
*/
int windowWidth = 600;
int windowHeight = 480;

/**
Screen position
*/
int windowXPos = 100;
int windowYPos = 150;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;

float roty = 0.0f;
float rotx = 90.0f;

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;

bool spacePressed = false;


GLdouble cameraAngleH = 0; // camera angles, to handle the mouse movement.
GLdouble cameraAngleV = 0;


float hight = 0.0f;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;

float accelX = 0.0f;
float accelY = 0.0f;
float accelZ = 0.0f;

float posX = 0.0f;
float posY = 0.4f;
float posZ = 1.0f;

//Thaose are the camera coords. Latter, they shall be initiated according to the blue car position
float CamPos_x = 0.0f;
float CamPos_y = 0.0f;
float CamPos_z = 0.0f;


float axis[3];
float lastPos[3] = {0,0,0};
float angle; //camera angle
/*
variavel auxiliar pra dar variação na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 4.0f;

// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0,
						0.0,1.0,0.0};

// now the position and speed of the sound source
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */

// Maps et certera
BITMAPINFO	*mapinfo;           /* Bitmap information */
GLubyte	    *mapbits;           /* Bitmap RGB pixels */
GLubyte     *mapptr;            /* Pointer into bit buffer */
GLubyte	    *maprgba;           /* RGBA pixel buffer */
GLubyte	    *maprgbaptr;        /* Pointer into RGBA buffer */


//models

ModelPorsche modelPOR;

//those are for mapping test:
ModelPorsche porTest1;
ModelPorsche porTest2;

// Those are the coords where the cars will be drawn to when the map is loaded.
float blueCar_x;
float blueCar_z;

float redCar_x;
float redCar_z;

bool foundCars;

void setWindow() {
	//roty = 0.0f;
    //rotx = 90.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX,posY + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),posY + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);


}

/**
Atualiza a posição e orientação da camera
*/
void updateCam() {

	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;

	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;

    //makes the car "follow" the camera
    modelPOR.Translate(posX,posY,posZ);


    CamPos_x = posX;
    CamPos_y = posY + 1.33;
    CamPos_z = posZ + 3.33;

    gluLookAt(CamPos_x,CamPos_y,CamPos_z,
		posX /*+ sin(roty*PI/180)*/,posY /*+ 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180)*/,posZ /*-cos(roty*PI/180)*/,
		0.0,1.0,0.0);
/**
Parameters

eyeX, eyeY, eyeZ
Specifies the position of the eye point.

centerX, centerY, centerZ
Specifies the position of the reference point.

upX, upY, upZ
Specifies the direction of the up vector.
*/



}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}


/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	//initSound(); TODO Disabled until we got some real sounds.


	// Enables hidden surfaces removal
	glFrontFace (GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	initTexture();

	initModel();

	initLight();

	printf("w - acelerar \n");
	printf("a - esquerda \n");
	printf("d - direita \n");
}

/**
Initialize openal and check for errors
*/

void initSound() {

	printf("Initializing OpenAl \n");

	// Init openAL
	alutInit(0, NULL);

	alGetError(); // clear any error messages

    // Generate buffers, or else no sound will happen!
    alGenBuffers(NUM_BUFFERS, buffer);

    if(alGetError() != AL_NO_ERROR)
    {
        printf("- Error creating buffers !!\n");
        exit(1);
    }
    else
    {
        printf("init() - No errors yet.\n");
    }

	alutLoadWAVFile("..\\res\\Footsteps.wav",&format,&data,&size,&freq,false);
    alBufferData(buffer[0],format,data,size,freq);
    //alutUnloadWAV(format,data,size,freq);

	alGetError(); /* clear error*/
    alGenSources(NUM_SOURCES, source);

    if(alGetError() != AL_NO_ERROR)
    {
        printf("- Error creating sources !!\n");
        exit(2);
    }
    else
    {
        printf("init - no errors after alGenSources\n");
    }

	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;

	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;

	alListenerfv(AL_POSITION,listenerPos);
    alListenerfv(AL_VELOCITY,listenerVel);
    alListenerfv(AL_ORIENTATION,listenerOri);

	alSourcef(source[0], AL_PITCH, 1.0f);
    alSourcef(source[0], AL_GAIN, 1.0f);
    alSourcefv(source[0], AL_POSITION, source0Pos);
    alSourcefv(source[0], AL_VELOCITY, source0Vel);
    alSourcei(source[0], AL_BUFFER,buffer[0]);
    alSourcei(source[0], AL_LOOPING, AL_TRUE);

	printf("Sound ok! \n\n");
}

/**
Initialize the texture
*/
void initTexture(void)
{
	printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("..\\res\\tiledbronze.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}


    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);



    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }

    // Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT); // why not GL_REPEAT?
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );


    printf("Textura %d\n", texture);



    // Now loads the map:
    printf ("\nLoading map..\n");
    // Load a texture object (256x256 true color)
    mapbits = LoadDIBitmap("..\\res\\mapping.bmp", &mapinfo);
    if (mapbits == (GLubyte *)0) {
		printf ("Error loading map!\n\n");
		return;
	}

	int x, z;

	//printf("Map Height = %d\n", mapinfo->bmiHeader.biHeight);
    int carFound = 0;

	for (x = 0; x < mapinfo->bmiHeader.biWidth; x++) {
		for (z = 0; z < mapinfo->bmiHeader.biHeight; z++) {
			if ((mapbits + x * z)[0] >= (GLubyte)250) { // Blue car
				printf("found blue car at (%d, %d)\n", x, z);
				blueCar_x = (float)x / (float)mapinfo->bmiHeader.biWidth;
				blueCar_z = (float)z / (float)mapinfo->bmiHeader.biHeight;
                carFound++;
			}

			if ((mapbits + x * z)[2] >= (GLubyte)250) { // Red car
				printf("found red car at (%d, %d)\n", x, z);
				redCar_x = (float)x / (float)mapinfo->bmiHeader.biWidth;
				redCar_z = (float)z / (float)mapinfo->bmiHeader.biHeight;
                carFound++;
			}
		}
	}
    if (carFound == 2) //found both cars
    foundCars = true;
        else
        {
        foundCars = false;
        printf("Error loading map: car(s) not found\n");
        }


	printf("Textures ok.\n\n", texture);
}


void initModel() {
	printf("Loading models.. \n");

    modelPOR = ModelPorsche();
    modelPOR.Init();


    porTest1 = ModelPorsche();
    porTest1.Init();
    porTest2 = ModelPorsche();
    porTest1.Init();

	printf("Models ok. \n \n \n");
}

void initLight() {
    glEnable(GL_LIGHTING);

	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 3.0, 3.0, 3.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT0);

}

void renderFloor() {

	// i want some nice, smooth, antialiased lines
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	// set things up to render the floor with the texture
	glShadeModel(GL_FLAT);
	glEnable (type);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(0.4f,0.4f,0.4f,1.0f);

	glPushMatrix();

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;

	glBegin(GL_QUADS);
		glTexCoord2f(textureScaleX, 0.0f);   // coords for the texture
		glVertex3f(-planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, 0.0f);  // coords for the texture
		glVertex3f(planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, textureScaleY);  // coords for the texture
		glVertex3f(planeSize, 0.0f, -planeSize);

		glTexCoord2f(textureScaleX, textureScaleY);  // coords for the texture
		glVertex3f(-planeSize, 0.0f, -planeSize);
	glEnd();

	glDisable(type);

	glColor4f(0.8f,0.8f,0.8f,1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
		glVertex3f(planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
	}
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, -planeSize);
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, planeSize);
	}
	glEnd();

	glPopMatrix();
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();
	renderFloor();

    modelPOR.Draw();

    //Testing the map car draw
    if(foundCars)
    {
    porTest1.Translate(redCar_x,1.0,redCar_z);
    porTest1.Draw();
    porTest2.Translate(blueCar_x,1.0,blueCar_z);
    porTest2.Draw();
    }
}

void updateState() {

	if (leftPressed) {
		roty -= 3.0f;
		modelPOR.RotateX(3.0);
	}

	if (rightPressed) {
		roty += 3.0f;
		modelPOR.RotateX(-3.0);
	}

//TODO: gottta improve that, get it smoother. DAT SHIT IS RUNNING LIKE KRAZY DHAUG
	if (upPressed){
        // TODO: When breaking, it should stay some time with movind (at speed zero) and then start backwarding, like a kart or something
        if(downPressed){ // Breaking condition
            accelX = -0.015 * sin(roty*PI/180);
            accelZ = +0.015 * cos(roty*PI/180);

            speedX += accelX;
            speedZ += accelZ;
        }
         else{
            accelX = 0.030 * sin(roty*PI/180);
            accelZ = -0.030 * cos(roty*PI/180);

            speedX += accelX;
            speedZ += accelZ;
        }


	}
        // TODO: When breaking, it should stay some time with movind (at speed zero) and then start backwarding, like a kart or something
    if(downPressed){ // Breaking/backwards
            accelX = -0.015 * sin(roty*PI/180);
            accelZ = +0.015 * cos(roty*PI/180);
            speedX += accelX;
            speedZ += accelZ;
        }


    ///TODO: there is something wrong in here, sometimes the car accelerates instead of breaking.
    if (spacePressed) //breaking
    {

        if (speedZ > speedX) // the car was heading backwards and started breaking
        {
            accelX = +0.015 * sin(roty*PI/180);
            accelZ = -0.015 * cos(roty*PI/180);
            speedX += accelX;
            speedZ += accelZ;

            if (speedX > speedZ)
            {
            speedX = 0;
            speedZ = 0;
            }

        }
        else // the car was heading forward and started breaking
        {
        accelX = -0.015 * sin(roty*PI/180);
        accelZ = +0.015 * cos(roty*PI/180);
        speedX += accelX;
        speedZ += accelZ;

            if (speedZ > speedX)
            {
            speedX = 0;
            speedZ = 0;
            }
        }


    }

// TODO: Once the car starts to moves, it's also never stoping LOL
// (acctly it's not funny, it's a very degrading project problem,
// and I'm very ashamed of it :'(  )
    posX += speedX;
    posZ += speedZ;


}



/**
Render scene
*/
void mainRender() {
	updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();


	Sleep(30);
}


/**
Mouse move while button pressed event handler
*/
void onMouseMove(int x, int y) {

	mouseLastX = x;
	mouseLastY = y;

	glutPostRedisplay();


}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = true;
			break;
		case 119: //w
			if (!upPressed) {
				alSourcePlay(source[0]);
			}
			upPressed = true;
			break;
		case 115: //s
			downPressed = true;
			break;
		case 97: //a
			leftPressed = true;
			break;
		case 100: //d
			rightPressed = true;
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = false;
			break;
		case 119: //w
			if (upPressed) {
				alSourceStop(source[0]);
				//PlaySound(NULL, 0, 0);
			}
			upPressed = false;
			break;
		case 115: //s
			downPressed = false;
			break;
		case 97: //a
			leftPressed = false;
			break;
		case 100: //d
			rightPressed = false;
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("TF");

	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);



	mainInit();

	glutMainLoop();

    return 0;
}
