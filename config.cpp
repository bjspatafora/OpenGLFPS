// File: config.cpp

#include <GL/freeglut.h> 

namespace mygllib
{
    // Window position and size
    int WIN_X = 0;
    int WIN_Y = 0;
    int WIN_W = 400;
    int WIN_H = 400;

    // Window title
    const char * WIN_TITLE = "OpenGL!!!";

    // Display mode
    unsigned int DISPLAY_MODE = GLUT_DOUBLE
                              | GLUT_DEPTH
                              | GLUT_RGBA;

    // RGBA for clear color
    GLfloat CLEAR_COLOR_R = 1.0f;
    GLfloat CLEAR_COLOR_G = 1.0f;
    GLfloat CLEAR_COLOR_B = 1.0f;
    GLfloat CLEAR_COLOR_A = 1.0f;
    
    // Parameters for gluOrtho2D and viewport
    GLfloat MIN_X = 0.0f;
    GLfloat MAX_X = 400.0f;
    GLfloat MIN_Y = 0.0f;
    GLfloat MAX_Y = 400.0f;

    // Eye
    GLfloat EYE_X = 1;
    GLfloat EYE_Y = 1;
    GLfloat EYE_Z = 1;
    GLfloat REF_X = 0;
    GLfloat REF_Y = 0;
    GLfloat REF_Z = 0;
    GLfloat UP_X = 0;
    GLfloat UP_Y = 1;
    GLfloat UP_Z = 0;

    // Orthogonal view
    GLfloat LEFT = -5;
    GLfloat RIGHT = 5;
    GLfloat BOTTOM = -5;
    GLfloat TOP = 5;
    GLfloat Z_NEAR = 1;
    GLfloat Z_FAR = 5;

    //  Perspective view
    // (NOTE: Z_NEAR and Z_FAR is under orthogonal viewing
}

