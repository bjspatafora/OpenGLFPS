// File: gl3d.h

#ifndef GL3D_H
#define GL3D_H

#include <GL/freeglut.h>
#include "config.h"
#include "View.h"
#include "Material.h"

namespace mygllib
{
    using namespace mygllib;

    inline
    void init3d()
    {
        int argc = 0;
        char ** argv = NULL;
        glutInit(&argc, argv);    
        glutInitWindowPosition(WIN_X, WIN_Y);
        glutInitWindowSize(WIN_W, WIN_H);
        glutInitDisplayMode(GLUT_DEPTH
                            | GLUT_DOUBLE
                            | GLUT_RGBA
                            | GLUT_STENCIL
            );
        glutCreateWindow(WIN_TITLE);
    }

    //-------------------------------------------------------------------------
    // Draw x-, y-, z- axes in red, green, blue respectively.
    // Lights should probably be turned off (if it's on) before calling this
    // function.
    //
    // USAGE:
    // void display()
    // {
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //    glDisable(GL_LIGHTING);
    //    mygllib::draw_axes();
    //    glEnable(GL_LIGHTING);
    //
    //    // draw objects
    // }
    //-------------------------------------------------------------------------
    inline
    void draw_axes(float length=10, float line_width=1.0)
    {
        glLineWidth(line_width);
        glBegin(GL_LINES);
        {
            glColor3f(1, 0, 0); // red
            glVertex3f(0, 0, 0);
            glVertex3f(length, 0, 0); 
            glColor3f(0, 1, 0); // green
            glVertex3f(0, 0, 0);
            glVertex3f(0, length, 0); 
            glColor3f(0, 0, 1); // blue
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, length);
        }
        glEnd();
    }


    inline
    void draw_xz_plane(int minx = -20, int maxx = 20,
                       int minz = -20, int maxz = 20,
                       GLfloat dx = 1.0f,
                       GLfloat dz = 1.0f)
    {
        glColor3f(0.5, 0.5, 0.5);
        glBegin(GL_LINES);
        for (float x = minx; x <= maxx; x += 1)
        {
            glVertex3f(x, 0, minz);
            glVertex3f(x, 0, maxz);
        }
        for (float z = minz; z <= maxz; z += 1)
        {
            glVertex3f(minx, 0, z);
            glVertex3f(maxx, 0, z);
        }
        glEnd();
    }
}

#endif
