// File: config.h

#ifndef CONFIG_H
#define CONFIG_H

#include "debug.h"

namespace mygllib
{
    // Window position and size
    extern int WIN_X;
    extern int WIN_Y;
    extern int WIN_W;
    extern int WIN_H;

    // Window title
    extern const char * WIN_TITLE;

    // Display mode
    extern unsigned int DISPLAY_MODE;
 
    // RGBA for clear color
    extern GLfloat CLEAR_COLOR_R;
    extern GLfloat CLEAR_COLOR_G;
    extern GLfloat CLEAR_COLOR_B;
    extern GLfloat CLEAR_COLOR_A;
    
    // Parameters for gluOrtho2D and viewport
    extern GLfloat MIN_X;
    extern GLfloat MAX_X;
    extern GLfloat MIN_Y;
    extern GLfloat MAX_Y;

    // Eye
    extern GLfloat EYE_X;
    extern GLfloat EYE_Y;
    extern GLfloat EYE_Z;
    extern GLfloat REF_X;
    extern GLfloat REF_Y;
    extern GLfloat REF_Z;
    extern GLfloat UP_X;
    extern GLfloat UP_Y;
    extern GLfloat UP_Z;
    
    // Orthogonal view
    extern GLfloat LEFT;
    extern GLfloat RIGHT;
    extern GLfloat BOTTOM;
    extern GLfloat TOP;
    extern GLfloat Z_NEAR;
    extern GLfloat Z_FAR;
    
    // Perspective view
    // (NOTE: Z_NEAR and Z_FAR is under orthogonal viewing
    
    
    // TODO: Add function to process command line arguments
}

#endif
