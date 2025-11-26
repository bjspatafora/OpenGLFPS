
#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

namespace mygllib
{
    class OpenGLError
    {};
    
    inline
    void debug()
    {
        GLenum errno;
        bool error = false;
        while ((errno = glGetError()) != GL_NO_ERROR)
        {
            std::cout << "\nOpenGL error: [" << errno << " -- "
                      << gluErrorString(errno) << ']' << std::endl;
            error = true;
        }
        if (error)
        {
            throw OpenGLError();
        }
    }
}

#endif
