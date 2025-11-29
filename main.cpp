#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <sstream>
#include "gl3d.h"
#include "View.h"
#include "SingletonView.h"
#include "Keyboard.h"
#include "Reshape.h"
#include "Material.h"
#include "ImageFile.h"
#include "Light.h"
#include "myglm.h"

mygllib::Light light;

std::vector< std::vector< int >> maze;
GLuint stoneTexture = 0;
GLuint hedgeTexture = 0;

void rotate(float & x, float & z, double t, float xpos, float zpos)
{
    // make a copy of the result x, so we don't change the calc for
    // the z value.
    float tx = x - xpos;
    float tz = z - zpos;
    float new_x = tx * std::cos(t) - tz * std::sin(t);
    z = zpos + tx * std::sin(t) + tz * std::cos(t);
    x = xpos + new_x;
}

void load_external_texture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    BMPFile stoneimage("stone.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stoneimage.width(),
                 stoneimage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 stoneimage.data());
}

void loadobj(std::vector< glm::vec3 > & vertices,
             std::vector< std::vector< GLuint >> & faces,
             std::vector< glm::vec3 > & normals)
{
    vertices.push_back(glm::vec3(0, 0, 0));
    normals.push_back(glm::vec3(0, 0, 0));
    std::ifstream inputFile("Male.OBJ");
    std::string line;
    while(std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        std::vector< std::string > words;
        std::string word;
        while(iss >> word)
            words.push_back(word);
        switch(line[0])
        {
            case 'v':
                vertices.push_back(glm::vec3(std::stof(words[1]),
                                             std::stof(words[2]),
                                             std::stof(words[3])));
                normals.push_back(glm::vec3(0, 0, 0));
                break;
            case 'f':
                std::vector< GLuint > inds;
                for(unsigned int i = 1; i < words.size(); i++)
                {
                    GLuint currindex = std::stoul(words[i], NULL, 10);
                    inds.push_back(currindex);
                }
                faces.push_back(inds);
                for(unsigned int i = 0; i < inds.size() - 2; i++)
                {
                    glm::vec3 ground = vertices[inds[i]];
                    glm::vec3 vec1 = vertices[inds[i+1]] - ground;
                    glm::vec3 vec2 = vertices[inds[i+2]] - ground;
                    glm::vec3 trinorm = glm::normalize(glm::cross(vec1,
                                                                  vec2));
                    for(int j = 0; j < 3; j++)
                        normals[inds[i+j]] += trinorm;
                }
                break;
        }
    }
    inputFile.close();
    for(unsigned int i = 0; i < normals.size(); i++)
        normals[i] = glm::normalize(normals[i]);
}

void init()
{
    mygllib::View & view = *(mygllib::SingletonView::getInstance());
    view.eyex() = 1.0f;
    view.eyey() = 1.0f;
    view.eyez() = 1.0f;
    view.refx() = 1.0f;
    view.refy() = 1.0f;
    view.refz() = 1.1f;
    view.set_projection();
    view.lookat();

    light.on();
    light.set();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glGenTextures(1, &stoneTexture);
    load_external_texture();
}

void drawRoom(unsigned int d)
{
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glBegin(GL_QUADS);
    if(d & 1)
    {
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(2, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3f(2, 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, 2, 0);
    }
    if(d & 2)
    {
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(2, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(2, 0, 2);
        glTexCoord2f(1, 1);
        glVertex3f(2, 2, 2);
        glTexCoord2f(0, 1);
        glVertex3f(2, 2, 0);
    }
    if(d & 4)
    {
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(2, 0, 2);
        glTexCoord2f(1, 0);
        glVertex3f(0, 0, 2);
        glTexCoord2f(1, 1);
        glVertex3f(0, 2, 2);
        glTexCoord2f(0, 1);
        glVertex3f(2, 2, 2);
    }
    if(d & 8)
    {
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 2);
        glTexCoord2f(1, 0);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3f(0, 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, 2, 2);
    }
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);
    glTexCoord2f(1, 0);
    glVertex3f(2, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3f(2, 0, 2);
    glTexCoord2f(0, 1);
    glVertex3f(0, 0, 2);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mygllib::Light::all_off();
    mygllib::draw_axes();
    mygllib::Light::all_on();

    glEnable(GL_TEXTURE_2D);
    for(unsigned int i = 0; i < maze.size(); i++)
    {
        for(unsigned int j = 0; j < maze.size(); j++)
        {
            glPushMatrix();
            glTranslatef(2*j, 0, 2*i);
            drawRoom(maze[i][j]);
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int w, int h)
{
    mygllib::View & view = *(mygllib::SingletonView::getInstance());

    float temp = 0;
    switch (key)
    {
        case 'w':
            view.refy() += 0.01f;
            break;
        case 's':
            view.refy() -= 0.01f;
            break;
        case 'a':
            rotate(view.refx(), view.refz(), -M_PI/20, view.eyex(),
                   view.eyez());
            break;
        case 'd':
            rotate(view.refx(), view.refz(), M_PI/20, view.eyex(),
                   view.eyez());
            break;
    }
    
    view.set_projection();
    view.lookat();
    light.set();
    glutPostRedisplay();
}

void specialkeyboard(int key, int w, int h)
{
    mygllib::View & view = *(mygllib::SingletonView::getInstance());

    float c = 0;
    float t = 0;
    switch (key)
    {
        case GLUT_KEY_UP:
            c = view.refx() - view.eyex();
            view.eyex() = view.refx();
            view.refx() += c;
            c = view.refz() - view.eyez();
            view.eyez() = view.refz();
            view.refz() += c;
            break;
        case GLUT_KEY_DOWN:
            c = view.refx() - view.eyex();
            view.refx() = view.eyex();
            view.eyex() -= c;
            c = view.refz() - view.eyez();
            view.refz() = view.eyez();
            view.eyez() -= c;
            break;
        case GLUT_KEY_LEFT:
            c = view.refx() - view.eyex();
            t = view.refz() - view.eyez();
            rotate(c, t, -M_PI/2, 0, 0);
            view.refx() += c;
            view.eyex() += c;
            view.refz() += t;
            view.eyez() += t;
            break;
        case GLUT_KEY_RIGHT:
            c = view.refx() - view.eyex();
            t = view.refz() - view.eyez();
            rotate(c, t, M_PI/2, 0, 0);
            view.refx() += c;
            view.eyex() += c;
            view.refz() += t;
            view.eyez() += t;
            break;
    }
    
    view.set_projection();
    view.lookat();
    light.set();
    glutPostRedisplay();
}

int main(int argc, char ** argv)
{
    // Maze init
    int n;
    std::cin >> n;
    int rooms = 15;
    for(int i = 0; i < n; i++)
    {
        std::vector< int > t;
        t.push_back(rooms);
        rooms -= 8;
        for(int j = 1; j < n; j++)
            t.push_back(rooms);
        maze.push_back(t);
        rooms = 14;
    }
    srand(time(NULL));
    int currx = rand() % n;
    int curry = rand() % n;
    std::stack< int > visited;
    visited.push(n * curry + currx);
    bool done[n][n] = {{0}};
    done[curry][currx] = 1;
    while(!visited.empty())
    {
        currx = visited.top() % n;
        curry = visited.top() / n;
        std::vector< int > availneighbors;
        if(currx > 0 && !done[curry][currx-1])
            availneighbors.push_back(0);
        if(currx < n - 1 && !done[curry][currx+1])
            availneighbors.push_back(1);
        if(curry > 0 && !done[curry-1][currx])
            availneighbors.push_back(2);
        if(curry < n - 1 && !done[curry+1][currx])
            availneighbors.push_back(3);
        if(availneighbors.empty())
            visited.pop();
        else
        {
            int i = rand() % availneighbors.size();
            switch(availneighbors[i])
            {
                case 0: // W
                    if(maze[curry][currx] & 8)
                        maze[curry][currx] -= 8;
                    currx--;
                    if(maze[curry][currx] & 2)
                        maze[curry][currx] -= 2;
                    break;
                case 1: // E
                    if(maze[curry][currx] & 2)
                        maze[curry][currx] -= 2;
                    currx++;
                    if(maze[curry][currx] & 8)
                        maze[curry][currx] -= 8;
                    break;
                case 2: // N
                    if(maze[curry][currx] & 1)
                        maze[curry][currx] -= 1;
                    curry--;
                    if(maze[curry][currx] & 4)
                        maze[curry][currx] -= 4;
                    break;
                case 3: // S
                    if(maze[curry][currx] & 4)
                        maze[curry][currx] -= 4;
                    curry++;
                    if(maze[curry][currx] & 1)
                        maze[curry][currx] -= 1;
                    break;
            }
            visited.push(curry * n + currx);
            done[curry][currx] = 1;
        }
    }
        
    mygllib::WIN_W = 600;
    mygllib::WIN_H = 600;
    mygllib::init3d();
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(mygllib::Reshape::reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkeyboard);
    glutMainLoop();
  
    return 0;
}
