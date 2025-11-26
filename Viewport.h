#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <GL/freeglut.h>

/*
  
  USAGE:
  Viewport viewport(0, 0, 400, 400);
  ...
  viewport(10, 10, 200, 200);
  ...

 */
namespace mygllib
{
    class Viewport
    {
    public:
        Viewport(GLint x_,
                 GLint y_,
                 GLsizei w_,
                 GLsizei h_)
            : x_(x), y_(y), w_(w), h_(h)
        {
            glViewport(x_, y_, w_, h_);
        }
        void operator()()
        {
            glViewport(x_, y_, w_, h_);
        }
        void operator()(GLint x_,
                        GLint y_,
                        GLsizei w_,
                        GLsizei h_
            )
        {
            x_ = x;
            y_ = y;
            w_ = w;
            h_ = h;
            glViewport(x_, y_, w_, h_);
        }
        
    private:
        GLint x_;
        GLint y_;
        GLsizei w_;
        GLsizei h_;
    };
}

#endif
