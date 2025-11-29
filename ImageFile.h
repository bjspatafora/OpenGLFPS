// ImageFile.h
#ifndef IMAGEFILE_H
#define IMAGEFILE_H
#include <iostream>
#include <fstream>
#include <string>
#include <GL/freeglut.h>

/******************************************************************************
Class RGB
---------
Base class for a 2-dimentional array of RGB values.
int width() const -- width of image
int height() const -- height of image
const GLubyte * const data() -- pointer to pixel data
******************************************************************************/

class RGB
{
  public:
    RGB() {}
    ~RGB() { delete [] data_; }
    GLuint width() const { return width_; }
    GLuint height() const { return height_; }
    void allocate(unsigned int s) { data_ = new GLubyte[s]; }
    const GLubyte * const data() const { return data_; }
  protected:
    GLubyte * data_;
    GLuint width_;
    GLuint height_;
};

/******************************************************************************
Class BMPFile
-------------
BMPFile(const std::string &) -- constructor
int width() const -- width of image (inherited from RGB)
int height() const -- height of image (inherited from RGB)
const GLubyte * const data() -- pointer to pixel data (inherited from RGB)
USAGE:
BMPFile image("grass.bmp")
std::cout << image.width() << std::endl;
std::cout << image.height() << std::endl;
std::cout << image.data()[0] << std::endl;
******************************************************************************/

class BMPFile: public RGB
{
  public:
    BMPFile(const std::string & path="")
    {
        std::ifstream f(path.c_str(), std::ios::binary);
        if (!f.is_open())
        {
            throw (std::string("File not found:") + path);
        }
        unsigned int offset, headerSize;
        f.seekg(10); // Get starting point of image data.
        f.read((char *) &offset, 4);
        f.read((char *) &headerSize,4); // Get the header size of the bitmap.
        f.seekg(18); // Get width and height
        f.read((char *) &width_, 4);
        f.read((char *) &height_, 4);
        unsigned int size = width_ * height_ * 3;
        allocate(size);
        f.seekg(offset); // Read bitmap data
        f.read((char *) data_, size);
// Reverse color from bgr to rgb
        for (unsigned int i = 0; i < size; i += 3)
        {
            unsigned char temp = data_[i];
            data_[i] = data_[i + 2];
            data_[i + 2] = temp;
        }
    }
    
  private:
};

#endif
