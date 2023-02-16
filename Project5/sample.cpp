#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996)
#endif

// #include "glew.h"
#include "bmptotexture.cpp"
#include "glslprogramP5.h"
#include "glut.h"
#include "loadobjfile.cpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char *WINDOWTITLE = {"OpenGL / GLUT Sample -- Joe Graphics"};
const char *GLUITITLE = {"User Interface Window"};

// what the glui package defines as true and false:

const int GLUITRUE = {true};
const int GLUIFALSE = {false};

// the escape key:

#define ESCAPE 0x1b

// timing

const int MS_PER_CYCLE = 5000;

// initial window size:

const int INIT_WINDOW_SIZE = {600};

// size of the box:

const float BOXSIZE = {2.f};

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = {1.};
const float SCLFACT = {0.005f};

// minimum allowable scale factor:

const float MINSCALE = {0.05f};

// active mouse buttons (or them together):

const int LEFT = {4};
const int MIDDLE = {2};
const int RIGHT = {1};

// which projection:

enum Projections
{
    ORTHO,
    PERSP
};

// which button:

enum ButtonVals
{
    RESET,
    QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = {0., 0., 0., 1.};

// line width for the axes:

const GLfloat AXES_WIDTH = {3.};

// the color numbers:
// this order must match the radio button order

enum Colors
{
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    MAGENTA,
    WHITE,
    BLACK
};

char *ColorNames[] =
    {
        "Red",
        "Yellow",
        "Green",
        "Cyan",
        "Blue",
        "Magenta",
        "White",
        "Black"};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
    {
        {1., 0., 0.}, // red
        {1., 1., 0.}, // yellow
        {0., 1., 0.}, // green
        {0., 1., 1.}, // cyan
        {0., 0., 1.}, // blue
        {1., 0., 1.}, // magenta
        {1., 1., 1.}, // white
        {0., 0., 0.}, // black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = {.0, .0, .0, 1.};
const GLenum FOGMODE = {GL_LINEAR};
const GLfloat FOGDENSITY = {0.30f};
const GLfloat FOGSTART = {1.5};
const GLfloat FOGEND = {4.};
GLuint TexName;
GLuint Tex0;
// non-constant global variables:

int ActiveButton; // current button that is down
GLuint AxesList;  // list to hold the axes
GLuint Curtain;   // list to hold the axes
GLuint Quad;      // list to hold the axes
int AxesOn;       // != 0 means to draw the axes
float Color[3];
int DebugOn;    // != 0 means to print debugging info
int DepthCueOn; // != 0 means to use intensity depth cueing
float D;        // square dimensions
bool Freeze;
GLuint SphereList; // object display list
int MainWindow;    // window id for main graphics window
GLSLProgram *Pattern;
float S0, T0;
float Scale; // scaling factor
float Time;
int WhichColor;      // index into Colors[ ]
int WhichProjection; // ORTHO or PERSP
int Xmouse, Ymouse;  // mouse values
float Xrot, Yrot;    // rotation angles in degrees

float White[] = {1., 1., 1., 1.};
GLSLProgram *PatternCube;
GLuint CubeName;

char *FaceFiles[] = {"kec.posx.bmp", "kec.negx.bmp", "kec.posy.bmp", "kec.negy.bmp", "kec.posz.bmp", "kec.negz.bmp"};
// function prototypes:

void Animate();
void Display();
void DoAxesMenu(int);
void DoChangeMenu(int);
void DoColorMenu(int);
void DoDepthMenu(int);
void DoDebugMenu(int);
void DoMainMenu(int);
void DoProjectMenu(int);
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MjbSphere(float, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Resize(int, int);
void Visibility(int);

void Axes(float);
void HsvRgb(float[3], float[3]);

float *Array3(float, float, float);
float *Array4(float, float, float, float);
float *BlendArray3(float, float[3], float[3]);
float *MulArray3(float, float[3]);
float xmin = -1.f; // set this to what you want it to be
float xmax = 1.f;  // set this to what you want it to be
float ymin = -1.f; // set this to what you want it to be
float ymax = 1.f;  // set this to what you want it to be
float dx = xmax - xmin;
float dy = ymax - ymin;
float z = 0.f;  // set this to what you want it to be
int numy = 128; // set this to what you want it to be
int numx = 128; // set this to what you want it to be
// main program:

int main(int argc, char *argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)

    glutInit(&argc, argv);

    // setup all the graphics stuff:

    InitGraphics();

    // create the display structures that will not change:

    InitLists();

    // init all the global variables used by Display( ):
    // this will also post a redisplay

    Reset();

    // setup all the user interface stuff:

    InitMenus();

    // draw the scene once and wait for some interaction:
    // (this will never return)

    glutSetWindow(MainWindow);
    glutMainLoop();

    // this is here to make the compiler happy:

    return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void Animate()
{
    int ms = glutGet(GLUT_ELAPSED_TIME);
    ms %= MS_PER_CYCLE;
    Time = (float)ms / (float)(MS_PER_CYCLE - 1);

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// draw the complete scene:

void Display()
{
    if (DebugOn != 0)
    {
        fprintf(stderr, "Display\n");
    }

    // set which window we want to do the graphics into:

    glutSetWindow(MainWindow);

    // erase the background:

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // specify shading to be flat:

    glShadeModel(GL_FLAT);

    // set the viewport to a square centered in the window:

    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy; // minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);

    // set the viewing volume:
    // remember that the Z clipping  values are actually
    // given as DISTANCES IN FRONT OF THE EYE
    // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO)
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    else
        gluPerspective(90., 1., 0.1, 1000.);

    // place the objects into the scene:

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set the eye position, look-at position, and up-vector:

    gluLookAt(0., 0., 3., 0., 0., 0., 0., 1., 0.);

    // rotate the scene:

    glRotatef((GLfloat)Yrot, 0., 1., 0.);
    glRotatef((GLfloat)Xrot, 1., 0., 0.);

    // uniformly scale the scene:

    if (Scale < MINSCALE)
        Scale = MINSCALE;
    glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

    // set the fog parameters:

    if (DepthCueOn != 0)
    {
        glFogi(GL_FOG_MODE, FOGMODE);
        glFogfv(GL_FOG_COLOR, FOGCOLOR);
        glFogf(GL_FOG_DENSITY, FOGDENSITY);
        glFogf(GL_FOG_START, FOGSTART);
        glFogf(GL_FOG_END, FOGEND);
        glEnable(GL_FOG);
    }
    else
    {
        glDisable(GL_FOG);
    }

    // possibly draw the axes:

    if (AxesOn != 0)
    {
        glColor3fv(&Colors[WhichColor][0]);
        glCallList(AxesList);
    }

    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);

    // enable the shader:
    S0 = 0.5f;
    T0 = 0.5f;
    D = 0.1f;
    float updateValue = sin(2. * M_PI * Time);
    updateValue = 0;
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, TexName);
    Pattern->Use();
    Pattern->SetUniformVariable("uAlpha", 1.f * (float)sin(0.2 * M_PI * Time));
    Pattern->SetUniformVariable("uTexUnit", 3);
    Pattern->SetUniformVariable("uNoiseFreq", 1.f + 1 * updateValue);
    Pattern->SetUniformVariable("uNoiseMag", 2.f + 1 * updateValue);
    Pattern->SetUniformVariable((char *)"uKa", 0.1f);
    Pattern->SetUniformVariable((char *)"uKd", 0.6f);
    Pattern->SetUniformVariable((char *)"uKs", 0.3f);
    Pattern->SetUniformVariable((char *)"uShininess", 8.f);
    Pattern->SetUniformVariable((char *)"uS0", S0);
    Pattern->SetUniformVariable((char *)"uT0", T0);
    Pattern->SetUniformVariable((char *)"uD", D * (float)(.5 + .5 * updateValue));
    Pattern->SetUniformVariable((char *)"diam", 0.2f * (float)(.5 + .5 * updateValue));
    Pattern->SetUniformVariable((char *)"Ar", 0.7f + 0.3f * (float)(.5 + .5 * updateValue));
    Pattern->SetUniformVariable((char *)"Br", 0.3f + 0.7f * (float)(.5 + .5 * updateValue));
    Pattern->SetUniformVariable((char *)"uTol", 0.005f * (float)(.5 + .5 * updateValue));
    // vertex
    Pattern->SetUniformVariable((char *)"uTime", 0.8f + 0.2f * updateValue);
    Pattern->SetUniformVariable((char *)"K", 0.2f + 0.4f * updateValue);
    Pattern->SetUniformVariable((char *)"P", 0.8f + 0.2f * updateValue);
    // draw the current object:
    glCallList(SphereList);
    glCallList(Curtain);
    Pattern->Use(0);

    //****project5
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, Tex0);
    PatternCube->Use();
    PatternCube->SetUniformVariable("uImageUnit", 7);
    glCallList(Quad);
    PatternCube->Use(0);
    //****project5

    glutSwapBuffers();
    glFlush();
}

void DoAxesMenu(int id)
{
    AxesOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoColorMenu(int id)
{
    WhichColor = id - RED;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDebugMenu(int id)
{
    DebugOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthMenu(int id)
{
    DepthCueOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// main menu callback:

void DoMainMenu(int id)
{
    switch (id)
    {
    case RESET:
        Reset();
        break;

    case QUIT:
        // gracefully close out the graphics:
        // gracefully close the graphics window:
        // gracefully exit the program:
        glutSetWindow(MainWindow);
        glFinish();
        glutDestroyWindow(MainWindow);
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoProjectMenu(int id)
{
    WhichProjection = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void DoRasterString(float x, float y, float z, char *s)
{
    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

// use glut to display a string of characters using a stroke font:

void DoStrokeString(float x, float y, float z, float ht, char *s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

// return the number of seconds since the start of the program:

float ElapsedSeconds()
{
    // get # of milliseconds since the start of the program:

    int ms = glutGet(GLUT_ELAPSED_TIME);

    // convert it to seconds:

    return (float)ms / 1000.f;
}

// initialize the glui window:

void InitMenus()
{
    glutSetWindow(MainWindow);

    int numColors = sizeof(Colors) / (3 * sizeof(int));
    int colormenu = glutCreateMenu(DoColorMenu);
    for (int i = 0; i < numColors; i++)
    {
        glutAddMenuEntry(ColorNames[i], i);
    }

    int axesmenu = glutCreateMenu(DoAxesMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthcuemenu = glutCreateMenu(DoDepthMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int debugmenu = glutCreateMenu(DoDebugMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int projmenu = glutCreateMenu(DoProjectMenu);
    glutAddMenuEntry("Orthographic", ORTHO);
    glutAddMenuEntry("Perspective", PERSP);

    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Axes", axesmenu);
    glutAddSubMenu("Colors", colormenu);
    glutAddSubMenu("Depth Cue", depthcuemenu);
    glutAddSubMenu("Projection", projmenu);
    glutAddMenuEntry("Reset", RESET);
    glutAddSubMenu("Debug", debugmenu);
    glutAddMenuEntry("Quit", QUIT);

    // attach the pop-up menu to the right mouse button:

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
unsigned char *ReadTexture3D(char *filename, int *width, int *height, int *depth)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return NULL;
    int nums, numt, nump;
    fread(&nums, 4, 1, fp);
    fread(&numt, 4, 1, fp);
    fread(&nump, 4, 1, fp);
    fprintf(stderr, "Texture size = % d x % d x % d\n", nums, numt, nump);
    *width = nums;
    *height = numt;
    *depth = nump;
    unsigned char *texture = new unsigned char[4 * nums * numt * nump];
    fread(texture, 4 * nums * numt * nump, 1, fp);
    fclose(fp);
    return texture;
}

void InitGraphics()
{
    // request the display modes:
    // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // set the initial window configuration:

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

    // open the window and set its title:

    MainWindow = glutCreateWindow(WINDOWTITLE);
    glutSetWindowTitle(WINDOWTITLE);

    // set the framebuffer clear values:

    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

    // setup the callback functions:
    // DisplayFunc -- redraw the window
    // ReshapeFunc -- handle the user resizing the window
    // KeyboardFunc -- handle a keyboard input
    // MouseFunc -- handle the mouse button going down or up
    // MotionFunc -- handle the mouse moving with a button down
    // PassiveMotionFunc -- handle the mouse moving with a button up
    // VisibilityFunc -- handle a change in window visibility
    // EntryFunc	-- handle the cursor entering or leaving the window
    // SpecialFunc -- handle special keys on the keyboard
    // SpaceballMotionFunc -- handle spaceball translation
    // SpaceballRotateFunc -- handle spaceball rotation
    // SpaceballButtonFunc -- handle spaceball button hits
    // ButtonBoxFunc -- handle button box hits
    // DialsFunc -- handle dial rotations
    // TabletMotionFunc -- handle digitizing tablet motion
    // TabletButtonFunc -- handle digitizing tablet button hits
    // MenuStateFunc -- declare when a pop-up menu is in use
    // TimerFunc -- trigger something to happen a certain time from now
    // IdleFunc -- what to do when nothing else is going on

    glutSetWindow(MainWindow);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(NULL);
    glutVisibilityFunc(Visibility);
    glutEntryFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpaceballMotionFunc(NULL);
    glutSpaceballRotateFunc(NULL);
    glutSpaceballButtonFunc(NULL);
    glutButtonBoxFunc(NULL);
    glutDialsFunc(NULL);
    glutTabletMotionFunc(NULL);
    glutTabletButtonFunc(NULL);
    glutMenuStateFunc(NULL);
    glutTimerFunc(-1, NULL, 0);
    glutIdleFunc(Animate);

    // init glew (a window must be open to do this):

#ifdef WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "glewInit Error\n");
    }
    else
        fprintf(stderr, "GLEW initialized OK\n");
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
    bool valid = false;
    glGenTextures(1, &TexName);
    int nums, numt, nump;
    unsigned char *texture = ReadTexture3D("noise3d.064.tex", &nums, &numt, &nump);
    if (texture == NULL)
    {
        return;
    }
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, nums, numt, nump, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);

    Pattern = new GLSLProgram();
    valid = Pattern->Create((char *)"pattern.vert", (char *)"pattern.frag");
    if (!valid)
    {
        fprintf(stderr, "Shader cannot be created!\n");
    }
    else
    {
        fprintf(stderr, "Shader pattern created.\n");
    }
    Pattern->SetVerbose(false);

    PatternCube = new GLSLProgram();
    valid = PatternCube->Create((char *)"texture.vert", (char *)"texture.frag");
    if (!valid)
    {
        fprintf(stderr, "Shader cannot be created!\n");
    }
    else
    {
        fprintf(stderr, "Shader texture created.\n");
    }
    PatternCube->SetVerbose(false);

    glGenTextures(1, &CubeName);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeName);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    for (int file = 0; file < 6; file++)
    {
        int nums, numt;
        unsigned char *texture2d = BmpToTexture(FaceFiles[file], &nums, &numt);
        if (texture2d == NULL)
            fprintf(stderr, "Could not open BMP 2D texture %s", FaceFiles[file]);
        else
            fprintf(stderr, "BMP 2D texture '%s' read -- nums = %d, numt = %d\n", FaceFiles[file], nums, numt);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + file, 0, 3, nums, numt, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, texture2d);
        delete[] texture2d;
    }
    //****project5
    int Width, Height;
    unsigned char *image_data = BmpToTexture("kec.bmp", &Width, &Height);
    if (image_data == NULL)
        fprintf(stderr, "Could not open BMP 2D texture kec.bmp");
    else
        fprintf(stderr, "BMP 2D texture kec.bmp read -- nums = %d, numt = %d\n", Width, Height);
    glGenTextures(1, &Tex0);
    glBindTexture(GL_TEXTURE_2D, Tex0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    //****project5
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void InitLists()
{
    glutSetWindow(MainWindow);

    // create the object:
    SphereList = glGenLists(1);
    float rgb[3] = {1., 1., 1.};
    glNewList(SphereList, GL_COMPILE);
    glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
    glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1.f, White));
    glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
    glMaterialf(GL_BACK, GL_SHININESS, 3.);
    glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));

    glMaterialfv(GL_FRONT, GL_AMBIENT, MulArray3(1.f, rgb));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MulArray3(1.f, rgb));
    glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.7f, White));
    glMaterialf(GL_FRONT, GL_SHININESS, 8.);
    glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));

    // LoadObjFile( "bunny010n.obj" );
    MjbSphere(1.3f, 72, 72);
    glEndList();

    // create Pleats
    Curtain = glGenLists(1);
    glNewList(Curtain, GL_COMPILE);
    for (int iy = 0; iy < numy; iy++)
    {
        glBegin(GL_QUAD_STRIP);
        glNormal3f(1., 1., 1.);
        for (int ix = 0; ix <= numx; ix++)
        {
            glTexCoord2f((float)ix / (float)numx + 2.f, (float)(iy + 0) / (float)numy);
            glVertex3f(xmin + dx * (float)ix / (float)numx + 2.f, ymin + dy * (float)(iy + 0) / (float)numy, z);
            glTexCoord2f((float)ix / (float)numx + 2.f, (float)(iy + 1) / (float)numy);
            glVertex3f(xmin + dx * (float)ix / (float)numx + 2.f, ymin + dy * (float)(iy + 1) / (float)numy, z);
        }
        glEnd();
    }
    glEndList();
    // create a quad
    int width = 2;
    Quad = glGenLists(1);
    glNewList(Quad, GL_COMPILE);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 2.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 4.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(2.0f, 4.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(2.0f, 2.0f);
    glEnd();
    glEndList();

    // create the axes:
    AxesList = glGenLists(1);
    glNewList(AxesList, GL_COMPILE);
    glLineWidth(AXES_WIDTH);
    Axes(1.5);
    glLineWidth(1.);
    glEndList();
}

// the keyboard callback:

void Keyboard(unsigned char c, int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

    switch (c)
    {
    case 'f':
    case 'F':
        Freeze = !Freeze;
        if (Freeze)
            glutIdleFunc(NULL);
        else
            glutIdleFunc(Animate);
        break;

    case 'o':
    case 'O':
        WhichProjection = ORTHO;
        break;

    case 'p':
    case 'P':
        WhichProjection = PERSP;
        break;

    case 'q':
    case 'Q':
    case ESCAPE:
        DoMainMenu(QUIT); // will not return here
        break;            // happy compiler

    default:
        fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
    }

    // force a call to Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y)
{
    int b = 0; // LEFT, MIDDLE, or RIGHT

    if (DebugOn != 0)
        fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

    // get the proper button bit mask:

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        b = LEFT;
        break;

    case GLUT_MIDDLE_BUTTON:
        b = MIDDLE;
        break;

    case GLUT_RIGHT_BUTTON:
        b = RIGHT;
        break;

    default:
        b = 0;
        fprintf(stderr, "Unknown mouse button: %d\n", button);
    }

    // button down sets the bit, up clears the bit:

    if (state == GLUT_DOWN)
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b; // set the proper bit
    }
    else
    {
        ActiveButton &= ~b; // clear the proper bit
    }
}

// called when the mouse moves while a button is down:

void MouseMotion(int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "MouseMotion: %d, %d\n", x, y);

    int dx = x - Xmouse; // change in mouse coords
    int dy = y - Ymouse;

    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
    }

    if ((ActiveButton & MIDDLE) != 0)
    {
        Scale += SCLFACT * (float)(dx - dy);

        // keep object from turning inside-out or disappearing:

        if (Scale < MINSCALE)
            Scale = MINSCALE;
    }

    Xmouse = x; // new current position
    Ymouse = y;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void Reset()
{
    ActiveButton = 0;
    AxesOn = 1;
    DebugOn = 0;
    DepthCueOn = 0;
    Freeze = false;
    Scale = 1.0;
    WhichColor = WHITE;
    WhichProjection = PERSP;
    Xrot = Yrot = 0.;
}

// called when user resizes the window:

void Resize(int width, int height)
{
    if (DebugOn != 0)
        fprintf(stderr, "ReSize: %d, %d\n", width, height);

    // don't really need to do anything since window size is
    // checked each time in Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// handle a change to the window's visibility:

void Visibility(int state)
{
    if (DebugOn != 0)
        fprintf(stderr, "Visibility: %d\n", state);

    if (state == GLUT_VISIBLE)
    {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
    0.f, 1.f, 0.f, 1.f};

static float xy[] = {
    -.5f, .5f, .5f, -.5f};

static int xorder[] = {
    1, 2, -3, 4};

static float yx[] = {
    0.f, 0.f, -.5f, .5f};

static float yy[] = {
    0.f, .6f, 1.f, 1.f};

static int yorder[] = {
    1, 2, 3, -2, 4};

static float zx[] = {
    1.f, 0.f, 1.f, 0.f, .25f, .75f};

static float zy[] = {
    .5f, .5f, -.5f, -.5f, 0.f, 0.f};

static int zorder[] = {
    1, 2, 3, 4, -5, 6};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void Axes(float length)
{
    glBegin(GL_LINE_STRIP);
    glVertex3f(length, 0., 0.);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., length, 0.);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., length);
    glEnd();

    float fact = LENFRAC * length;
    float base = BASEFRAC * length;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++)
    {
        int j = xorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 5; i++)
    {
        int j = yorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++)
    {
        int j = zorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
    }
    glEnd();
}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void HsvRgb(float hsv[3], float rgb[3])
{
    // guarantee valid input:

    float h = hsv[0] / 60.f;
    while (h >= 6.)
        h -= 6.;
    while (h < 0.)
        h += 6.;

    float s = hsv[1];
    if (s < 0.)
        s = 0.;
    if (s > 1.)
        s = 1.;

    float v = hsv[2];
    if (v < 0.)
        v = 0.;
    if (v > 1.)
        v = 1.;

    // if sat==0, then is a gray:

    if (s == 0.0)
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    // get an rgb from the hue itself:

    float i = (float)floor(h);
    float f = h - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * f);
    float t = v * (1.f - (s * (1.f - f)));

    float r, g, b; // red, green, blue
    switch ((int)i)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = v;
        b = p;
        break;

    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;

    case 4:
        r = t;
        g = p;
        b = v;
        break;

    case 5:
        r = v;
        g = p;
        b = q;
        break;
    }

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

float *
Array3(float a, float b, float c)
{
    static float array[4];

    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = 1.;
    return array;
}

float *
Array4(float a, float b, float c, float d)
{
    static float array[4];

    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = d;
    return array;
}

float *
BlendArray3(float factor, float array0[3], float array1[3])
{
    static float array[4];

    array[0] = factor * array0[0] + (1.f - factor) * array1[0];
    array[1] = factor * array0[1] + (1.f - factor) * array1[1];
    array[2] = factor * array0[2] + (1.f - factor) * array1[2];
    array[3] = 1.;
    return array;
}

float *
MulArray3(float factor, float array0[3])
{
    static float array[4];

    array[0] = factor * array0[0];
    array[1] = factor * array0[1];
    array[2] = factor * array0[2];
    array[3] = 1.;
    return array;
}

#include "glslprogramP5.cpp"
#include "sphere.cpp"
