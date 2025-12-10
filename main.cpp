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

struct Player
{
    float x, z, angle;
};

struct Enemy
{
    glm::vec3 pos, mov;
};

struct Bullet
{
    glm::vec3 pos, vel;
};

mygllib::Light light;
std::vector< std::vector< int >> maze;
Player player = {1, 1, 0};
std::vector< Enemy > enemies;
std::vector< Bullet > bullets;
GLuint stoneTexture = 0;
GLuint hedgeTexture = 0;
GLuint selfdraw = 0;
GLuint spheredraw = 0;
GLuint cyldraw = 0;
bool birdseye = 0;

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

bool validxmove(float x, float z, float dx, bool p = 1)
{
    if(p)
    {
        if(dx > 0)
            dx += .25;
        else
            dx -= .25;
    }
    float nx = x + dx;
    if((nx >= ((int)x / 2) * 2 + 2) || (nx <= ((int)x / 2) * 2))
    {
        if(dx < 0)
            return (maze[(int)z/2][(int)x/2] & 8) == 0;
        else
            return (maze[(int)z/2][(int)x/2] & 2) == 0;
    }
    return 1;
}

bool validzmove(float x, float z, float dz, bool p = 1)
{
    if(p)
    {
        if(dz > 0)
            dz += .25;
        else
            dz -= .25;
    }
    float nz = z + dz;
    if((nz >= ((int)z / 2) * 2 + 2) || (nz <= ((int)z / 2) * 2))
    {
        if(dz < 0)
            return (maze[(int)z/2][(int)x/2] & 1) == 0;
        else
            return (maze[(int)z/2][(int)x/2] & 4) == 0;
    }
    return 1;
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
    std::ifstream inputFile("3DModel.obj");
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

std::vector< std::vector< glm::vec3 >>
surface_of_revolution(const std::vector< glm::vec3 > & vertices,
                      unsigned int num_segments=20, bool closed=1,
                      GLfloat angle=0)
{
    if(closed)
        angle = 2 * M_PI;
    std::vector< std::vector< glm::vec3 >> res;
    float t = 0;
    for(unsigned int i = 0; i < num_segments - 1; i++)
    {
        std::vector< glm::vec3 > temp;
        for(auto p = vertices.begin(); p != vertices.end(); p++)
        {
            float x = (*p)[0];
            float z = (*p)[2];
            rotate(x, z, t, 0, 0);
            temp.push_back(glm::vec3(x, (*p)[1], z));
        }
        res.push_back(temp);
        t += angle / (num_segments - 1);
    }
    res.push_back(res[0]);
    return res;
}

std::vector< std::vector< glm::vec3 >>
get_normals(std::vector< std::vector< glm::vec3 >> & vertices)
{
    std::vector< std::vector< glm::vec3 >> res;
    res.resize(vertices.size());
    for(unsigned int i = 0; i < vertices.size(); i++)
        res[i].resize(vertices[0].size());
    for(int i = 0; i < vertices.size() - 1; i++)
    {
        res[i][0] = glm::vec3(0, -1, 0);
        int j = 1;
        for(; j < vertices[i].size() - 1; j++)
        {
            int li = (i - 1 >= 0? i - 1 : vertices.size() - 2);
            int bi = (i + 1 < vertices.size()? i + 1 : 1);
            int lj = (j - 1 >= 0? j - 1 : -1);
            int bj = (j + 1 < vertices[i].size()? j + 1 : -1);
            glm::vec3 north;
            glm::vec3 northeast;
            glm::vec3 east = vertices[bi][j];
            glm::vec3 south;
            glm::vec3 southwest;
            glm::vec3 west = vertices[li][j];
            if(lj == -1)
            {
                unsigned int newi = (i + vertices.size() / 2) % vertices.size();
                north = vertices[newi][1];
                newi = (newi + 1) % vertices.size();
                northeast = vertices[newi][1];
            }
            else
            {
                north = vertices[i][lj];
                northeast = vertices[bi][lj];
            }
            if(bj == -1)
            {
                unsigned int newi = (i + vertices.size() / 2) % vertices.size();
                south = vertices[newi][j - 1];
                newi = (newi - 1) % vertices.size();
                if(newi <= 0)
                    newi += vertices.size();
                southwest = vertices[newi][j-1];
            }
            else
            {
                south = vertices[i][bj];
                southwest = vertices[li][bj];
            }
            res[i][j] = glm::normalize(glm::cross(northeast, north) +
                                       glm::cross(east, northeast) +
                                       glm::cross(south, east) +
                                       glm::cross(southwest, south) +
                                       glm::cross(west, southwest) +
                                       glm::cross(north, west));
        }
        res[i][j] = glm::vec3(0, 1, 0);
    }
    res[res.size() - 1] = res[0];
    return res;
}

void drawSORshape(const std::vector< std::vector< glm::vec3 >> & points,
                  const std::vector< std::vector< glm::vec3 >> & normals)
{
    glBegin(GL_TRIANGLE_STRIP);
    bool mid = 0;
    unsigned int i = 0;
    for(; i < points.size() - 1; i++)
    {
        for(unsigned int j = 0; j < points[i].size(); j++)
        {
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
            glNormal3f(normals[i+1][j][0], normals[i+1][j][1],
                       normals[i+1][j][2]);
            glVertex3f(points[i+1][j][0], points[i+1][j][1], points[i+1][j][2]);
        }
        i++;
        if(i == points.size() - 1)
        {
            mid = 1;
            break;
        }
        for(unsigned int j = points[i].size() - 1; j > 0; j--)
        {
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
            glNormal3f(normals[i+1][j][0], normals[i+1][j][1],
                       normals[i+1][j][2]);
            glVertex3f(points[i+1][j][0], points[i+1][j][1], points[i+1][j][2]);
            glNormal3f(normals[i+1][j-1][0], normals[i+1][j-1][1],
                       normals[i+1][j-1][2]);
            glVertex3f(points[i+1][j-1][0], points[i+1][j-1][1],
                       points[i+1][j-1][2]);
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
        }
        glNormal3f(normals[i][0][0], normals[i][0][1], normals[i][0][2]);
        glVertex3f(points[i][0][0], points[i][0][1], points[i][0][2]);
        glNormal3f(normals[i+1][0][0], normals[i+1][0][1], normals[i+1][0][2]);
        glVertex3f(points[i+1][0][0], points[i+1][0][1], points[i+1][0][2]);
    }
    if(mid)
    {
        for(unsigned int j = points[0].size() - 1; j > 0; j--)
        {
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
            glNormal3f(normals[0][j][0], normals[0][j][1], normals[0][j][2]);
            glVertex3f(points[0][j][0], points[0][j][1], points[0][j][2]);
            glNormal3f(normals[0][j-1][0], normals[0][j-1][1],
                       normals[0][j-1][2]);
            glVertex3f(points[0][j-1][0], points[0][j-1][1], points[0][j-1][2]);
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
        }
        glNormal3f(normals[i][0][0], normals[i][0][1], normals[i][0][2]);
        glVertex3f(points[i][0][0], points[i][0][1], points[i][0][2]);
        glNormal3f(normals[0][0][0], normals[0][0][1], normals[0][0][2]);
        glVertex3f(points[0][0][0], points[0][0][1], points[0][0][2]);
    }
    else
    {
        for(unsigned int j = 0; j < points[i].size(); j++)
        {
            glNormal3f(normals[i][j][0], normals[i][j][1], normals[i][j][2]);
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
            glNormal3f(normals[0][j][0], normals[0][j][1], normals[0][j][2]);
            glVertex3f(points[0][j][0], points[0][j][1], points[0][j][2]);
        }
    }   
    glEnd();
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

    light.x() = 5;
    light.z() = 5;
    light.y() = 10;
    light.on();
    light.set();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    glGenTextures(1, &stoneTexture);
    load_external_texture();

    // spheres
    std::vector< glm::vec3 > line;
    for(float i = -M_PI/2; i <= M_PI/2; i += M_PI / 10)
    {
        float x = 1;
        float y = 0;
        rotate(x, y, i, 0, 0);
        line.push_back(glm::vec3(x, y, 0));
    }
    std::vector< std::vector< glm::vec3 >> points = surface_of_revolution(line,
                                                                          20,
                                                                          0,
                                                                          2*M_PI);
    std::vector< std::vector< glm::vec3 >> normals = get_normals(points);
    spheredraw = glGenLists(1);
    glNewList(spheredraw, GL_COMPILE);
    drawSORshape(points, normals);
    glEndList();

    //cylinders
    line.clear();
    points.clear();
    normals.clear();
    line.push_back({0, -0.5, 0});
    line.push_back({0.5, -0.5, 0});
    line.push_back({1, -0.5, 0});
    line.push_back({1, 0, 0});
    line.push_back({1, 0.5, 0});
    line.push_back({0.5, 0.5, 0});
    line.push_back({0, 0.5, 0});
    points = surface_of_revolution(line, 20, 0, 2*M_PI);
    normals = get_normals(points);
    cyldraw = glGenLists(1);
    glNewList(cyldraw, GL_COMPILE);
    drawSORshape(points, normals);
    glEndList();

    line.clear();
    std::vector< glm::vec3 > objnorms;
    std::vector< std::vector< GLuint >> faces;
    loadobj(line, faces, objnorms);
    selfdraw = glGenLists(1);
    glNewList(selfdraw, GL_COMPILE);
    for(auto v : faces)
    {
        switch(v.size())
        {
            case 3:
                glBegin(GL_TRIANGLES);
                break;
            case 4:
                glBegin(GL_QUADS);
                break;
            default:
                glBegin(GL_POLYGON);
        }
        for(unsigned int i = 0; i < v.size(); i++)
        {
            glNormal3f(objnorms[v[i]][0], objnorms[v[i]][1],
                       objnorms[v[i]][2]);
            glVertex3f(line[v[i]][0], line[v[i]][1], line[v[i]][2]);
        }
        glEnd();
    }
    glEndList();
}

std::vector< std::vector< bool >> visiblerooms()
{
    std::vector< std::vector< bool >> res;
    res.resize(maze.size());
    for(unsigned int i = 0; i < res.size(); i++)
        res[i].resize(res.size());
    
    glm::vec3 temppos = {player.x, 0, player.z};
    res[((int)temppos[2])/2][((int)temppos[0])/2] = 1;
    glm::vec3 poschange = {0, 0, .01};
    rotate(poschange[0], poschange[2], player.angle + 4*M_PI/10, 0, 0);
    for(unsigned int i = 0; i < 19; i++)
    {
        while(validxmove(temppos[0], temppos[2], poschange[0], 0) &&
              validzmove(temppos[0], temppos[2], poschange[2], 0))
        {
            glm::vec3 aftermov = temppos + poschange;
            if(((int)temppos[0]) / 2 != ((int)aftermov[0]) / 2 &&
               ((int)temppos[2]) / 2 != ((int)aftermov[2]) / 2)
                break;
            temppos = aftermov;
            res[((int)temppos[2])/2][((int)temppos[0])/2] = 1;
        }
        int mazex = (int)temppos[0] / 2;
        int mazez = (int)temppos[2] / 2;
        if(!(maze[mazez][mazex] & 1))
            res[mazez-1][mazex] = 1;
        if(!(maze[mazez][mazex] & 2))
            res[mazez][mazex+1] = 1;
        if(!(maze[mazez][mazex] & 4))
            res[mazez+1][mazex] = 1;
        if(!(maze[mazez][mazex] & 8))
            res[mazez][mazex-1] = 1;
        temppos = {player.x, 0, player.z};
        float tchangex = poschange[0];
        float tchangez = poschange[2];
        rotate(tchangex, tchangez, -M_PI/20, 0, 0);
        poschange = {tchangex, 0, tchangez};
    }
    return res;
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
    mygllib::Light::all_on();

    std::vector< std::vector< bool >> visible = visiblerooms();

    glEnable(GL_TEXTURE_2D);
    if(visible[0][0])
        drawRoom(maze[0][0]);
    else
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        if(!(maze[0][0] & 1))
        {
            glVertex3f(1, 0, 1);
            glVertex3f(1, 0, 0);
        }
        if(!(maze[0][0] & 2))
        {
            glVertex3f(1, 0, 1);
            glVertex3f(2, 0, 1);
        }
        if(!(maze[0][0] & 4))
        {
            glVertex3f(1, 0, 1);
            glVertex3f(1, 0, 2);
        }
        if(!(maze[0][0] & 8))
        {
            glVertex3f(1, 0, 1);
            glVertex3f(0, 0, 1);
        }
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
    int drawwall;
    for(unsigned int j = 1; j < maze.size(); j++)
    {
        if(visible[0][j])
        {
            glPushMatrix();
            glTranslatef(2*j, 0, 0);
            drawwall = 15;
            if(visible[0][j - 1])
                drawwall -= 8;
            drawRoom(maze[0][j] & drawwall);
            glPopMatrix();
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
            glPushMatrix();
            glTranslatef(2*j, 0, 0);
            glBegin(GL_LINES);
            if(!(maze[0][j] & 1))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(1, 0, 0);
            }
            if(!(maze[0][j] & 2))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(2, 0, 1);
            }
            if(!(maze[0][j] & 4))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(1, 0, 2);
            }
            if(!(maze[0][j] & 8))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(0, 0, 1);
            }
            glEnd();
            glPopMatrix();
            glEnable(GL_TEXTURE_2D);
        }
    }
    for(unsigned int i = 1; i < maze.size(); i++)
    {
        if(visible[i][0])
        {
            glPushMatrix();
            glTranslatef(0, 0, 2*i);
            drawwall = 15;
            if(visible[i-1][0])
                drawwall -= 1;
            drawRoom(maze[i][0] & drawwall);
            glPopMatrix();
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
            glPushMatrix();
            glTranslatef(0, 0, 2*i);
            glBegin(GL_LINES);
            if(!(maze[i][0] & 1))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(1, 0, 0);
            }
            if(!(maze[i][0] & 2))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(2, 0, 1);
            }
            if(!(maze[i][0] & 4))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(1, 0, 2);
            }
            if(!(maze[i][0] & 8))
            {
                glVertex3f(1, 0, 1);
                glVertex3f(0, 0, 1);
            }
            glEnd();
            glPopMatrix();
            glEnable(GL_TEXTURE_2D);
        }
        for(unsigned int j = 1; j < maze.size(); j++)
        {
            if(visible[i][j])
            {
                glPushMatrix();
                glTranslatef(2*j, 0, 2*i);
                drawwall = 15;
                if(visible[i-1][j])
                    drawwall -= 1;
                if(visible[i][j-1])
                    drawwall -= 8;
                drawRoom(maze[i][j] & drawwall);
                glPopMatrix();
            }
            else
            {
                glDisable(GL_TEXTURE_2D);
                glPushMatrix();
                glTranslatef(2*j, 0, 2*i);
                glBegin(GL_LINES);
                if(!(maze[i][j] & 1))
                {
                    glVertex3f(1, 0, 1);
                    glVertex3f(1, 0, 0);
                }
                if(!(maze[i][j] & 2))
                {
                    glVertex3f(1, 0, 1);
                    glVertex3f(2, 0, 1);
                }
                if(!(maze[i][j] & 4))
                {
                    glVertex3f(1, 0, 1);
                    glVertex3f(1, 0, 2);
                }
                if(!(maze[i][j] & 8))
                {
                    glVertex3f(1, 0, 1);
                    glVertex3f(0, 0, 1);
                }
                glEnd();
                glPopMatrix();
                glEnable(GL_TEXTURE_2D);
            }
        }
    }
    glDisable(GL_TEXTURE_2D);
    for(const auto & e : enemies)
    {
        if(visible[((int)e.pos[2])/2][((int)e.pos[0])/2])
        {
            glPushMatrix();
            glTranslatef(e.pos[0], .75, e.pos[2]);
            glScalef(.5, 1.5, .5);
            glCallList(cyldraw);
            glPopMatrix();
        }
        else if(birdseye)
        {
            glPushMatrix();
            glTranslatef(e.pos[0], .75, e.pos[2]);
            glScalef(.25, .25, .25);
            glCallList(spheredraw);
            glPopMatrix();
        }
    }
    for(const auto & b : bullets)
    {
        glPushMatrix();
        glTranslatef(b.pos[0], b.pos[1], b.pos[2]);
        glScalef(.1, .1, .1);
        glCallList(spheredraw);
        glPopMatrix();
    }
    if(birdseye)
    {
        glPushMatrix();
        glTranslatef(player.x, 0.75, player.z);
        glRotatef(-player.angle * (180 / M_PI), 0, 1, 0);
        glCallList(selfdraw);
        glPopMatrix();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int w, int h)
{
    mygllib::View & view = *(mygllib::SingletonView::getInstance());

    switch (key)
    {
        case 'w':
            if(!birdseye)
                view.refy() += 0.01f;
            break;
        case 's':
            if(!birdseye)
                view.refy() -= 0.01f;
            break;
        case 'a':
            if(!birdseye)
                rotate(view.refx(), view.refz(), -M_PI/20, view.eyex(),
                       view.eyez());
            player.angle -= M_PI / 20;
            break;
        case 'd':
            if(!birdseye)
                rotate(view.refx(), view.refz(), M_PI/20, view.eyex(),
                       view.eyez());
            player.angle += M_PI / 20;
            break;
        case 'v':
            birdseye = !birdseye;
            if(birdseye)
            {
                view.eyey() = 5;
                view.refy() = 0;
                view.refx() = view.eyex();
                view.refz() = view.eyez() + .1;
            }
            else
            {
                view.eyey() = 1;
                view.refy() = 1;
                float x = 0;
                float z = .1;
                rotate(x, z, player.angle, 0, 0);
                view.eyex() = player.x;
                view.refx() = player.x + x;
                view.eyez() = player.z;
                view.refz() = player.z + z;
            }
            break;
        case 'i':
            if(birdseye)
            {
                view.eyez() += .1;
                view.refz() += .1;
            }
            break;
        case 'j':
            if(birdseye)
            {
                view.eyex() += .1;
                view.refx() += .1;
            }
            break;
        case 'l':
            if(birdseye)
            {
                view.eyex() -= .1;
                view.refx() -= .1;
            }
            break;
        case 'k':
            if(birdseye)
            {
                view.eyez() -= .1;
                view.refz() -= .1;
            }
            break;
        case 'u':
            if(birdseye && view.eyey() > 2)
                view.eyey() -= .1;
            break;
        case 'o':
            if(birdseye)
                view.eyey() += .1;
            break;
        case 'f':
            glm::vec3 vel = {0, 0, 0.2};
            if(!birdseye)
                vel[1] = view.refy() - view.eyey();
            rotate(vel[0], vel[2], player.angle, 0, 0);
            glm::vec3 pos = {player.x + vel[0], 1, player.z + vel[2]};
            pos += vel;
            bullets.push_back({pos, vel});
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
    if(birdseye)
    {
        switch (key)
        {
            case GLUT_KEY_UP:
                t = .1;
                rotate(c, t, player.angle, 0, 0);
                if(validxmove(player.x, player.z, c))
                    player.x += c;
                if(validzmove(player.x, player.z, t))
                    player.z += t;
                break;
            case GLUT_KEY_DOWN:
                t = .1;
                rotate(c, t, player.angle + M_PI, 0, 0);
                if(validxmove(player.x, player.z, c))
                    player.x += c;
                if(validzmove(player.x, player.z, t))
                    player.z += t;
                break;
            case GLUT_KEY_LEFT:
                t = .1;
                rotate(c, t, player.angle - (M_PI / 2), 0, 0);
                if(validxmove(player.x, player.z, c))
                    player.x += c;
                if(validzmove(player.x, player.z, t))
                    player.z += t;
                break;
            case GLUT_KEY_RIGHT:
                t = .1;
                rotate(c, t, player.angle + (M_PI / 2), 0, 0);
                if(validxmove(player.x, player.z, c))
                    player.x += c;
                if(validzmove(player.x, player.z, t))
                    player.z += t;
                break;
        }
    }
    else
    {
        switch (key)
        {
            case GLUT_KEY_UP:
                c = view.refx() - view.eyex();
                t = view.refz() - view.eyez();
                if(validxmove(view.eyex(), view.eyez(), c))
                {
                    view.eyex() = view.refx();
                    view.refx() += c;
                }
                if(validzmove(view.eyex(), view.eyez(), t))
                {
                    view.eyez() = view.refz();
                    view.refz() += t;
                }
                break;
            case GLUT_KEY_DOWN:
                c = -(view.refx() - view.eyex());
                t = -(view.refz() - view.eyez());
                if(validxmove(view.eyex(), view.eyez(), c))
                {
                    view.refx() = view.eyex();
                    view.eyex() += c;
                }
                if(validzmove(view.eyex(), view.eyez(), t))
                {
                    view.refz() = view.eyez();
                    view.eyez() += t;
                }
                break;
            case GLUT_KEY_LEFT:
                c = view.refx() - view.eyex();
                t = view.refz() - view.eyez();
                rotate(c, t, -M_PI/2, 0, 0);
                if(validxmove(view.eyex(), view.eyez(), c))
                {
                    view.eyex() += c;
                    view.refx() += c;
                }
                if(validzmove(view.eyex(), view.eyez(), t))
                {
                    view.eyez() += t;
                    view.refz() += t;
                }
                break;
            case GLUT_KEY_RIGHT:
                c = view.refx() - view.eyex();
                t = view.refz() - view.eyez();
                rotate(c, t, M_PI/2, 0, 0);
                if(validxmove(view.eyex(), view.eyez(), c))
                {
                    view.eyex() += c;
                    view.refx() += c;
                }
                if(validzmove(view.eyex(), view.eyez(), t))
                {
                    view.eyez() += t;
                    view.refz() += t;
                }
                break;
        }
        player.x = view.eyex();
        player.z = view.eyez();
    }
    
    view.set_projection();
    view.lookat();
    light.set();
    glutPostRedisplay();
}

bool enemyfire(unsigned int i)
{
    glm::vec3 travel = {player.x - enemies[i].pos[0], 0,
        player.z - enemies[i].pos[2]};
    travel = glm::normalize(travel);
    travel *= 0.2;
    glm::vec3 temppos = enemies[i].pos + travel;
    while(1)
    {
        if((temppos[0] < player.x + .01 && temppos[0] > player.x - .01) &&
           (temppos[2] < player.z + .01 && temppos[2] > player.z - .01))
            break;
        if(validxmove(temppos[0], temppos[2], travel[0], 0))
            temppos[0] += travel[0];
        else
            return 0;
        if(validzmove(temppos[0], temppos[2], travel[2], 0))
            temppos[2] += travel[2];
        else
            return 0;
    }
    glm::vec3 newbullet = {enemies[i].pos[0], 1, enemies[i].pos[2]};
    bullets.push_back({newbullet + travel, travel});
    return 1;
}

void updates(int fire)
{
    if(fire > 0)
        fire--;
    for(unsigned int i = 0; i < bullets.size(); i++)
    {
        auto & [bpos, bvel] = bullets[i];
        if(validxmove(bpos[0], bpos[2], bvel[0]) &&
           validzmove(bpos[0], bpos[2], bvel[2]) && bpos[1] > .2)
        {
            // Can const and &s
            // auto [fname1, fname2, ...] = v
            bpos += bvel;
            if(bpos[0] >= player.x - .25 &&
               bpos[0] <= player.x + .25 &&
               bpos[2] >= player.z - .25 &&
               bpos[2] <= player.z + .25)
            {
                std::cout << "PLAYER IS HIT\n";
                bullets.erase(bullets.begin() + i);
                i--;
            }
            else
            {
                for(unsigned int j = 0; j < enemies.size(); j++)
                {
                    auto [epos, emov] = enemies[j];
                    if(bpos[0] >= epos[0] - .25 &&
                       bpos[0] <= epos[0] + .25 &&
                       bpos[2] >= epos[2] - .25 &&
                       bpos[2] <= epos[2] + .25)
                    {
                        std::cout << "ENEMY HIT\n";
                        enemies.erase(enemies.begin() + j);
                        bullets.erase(bullets.begin() + i);
                        std::cout << "ENEMIES LEFT: " << enemies.size()
                                  << std::endl;
                        break;
                    }
                }
            }
        }
        else
        {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }
    bool fired = 0;
    for(unsigned int i = 0; i < enemies.size(); i++)
    {
        if(!fire && enemyfire(i))
        {
            fired = 1;
            continue;
        }
        if(enemies[i].mov[1] == 40)
        {
            enemies[i].mov[0] = 0;
            enemies[i].mov[1] = 0;
            enemies[i].mov[2] = 0;
            int dir = rand() % 4;
            auto & [epos, emov] = enemies[i];
            switch(dir)
            {
                case 0:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 1) &&
                       epos[2] > 2)
                        emov[2] = -.05;
                    break;
                case 1:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 2) &&
                       epos[0] < (maze.size() - 1) * 2)
                        emov[0] = .05;
                    break;
                case 2:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 4) &&
                       epos[2] < (maze.size() - 1) * 2)
                        emov[2] = .05;
                    break;
                case 3:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 8) &&
                       epos[0] > 2)
                        emov[0] = -.05;
                    break;
            }
        }
        enemies[i].pos += enemies[i].mov;
        enemies[i].mov[1] += 1;
        enemies[i].pos[1] = 0;
    }
    if(fired)
        fire = 20;
    glutPostRedisplay();
    glutTimerFunc(50, updates, fire);
}

int main(int argc, char ** argv)
{
    // Maze init
    int n;
    std::cin >> n;
    // Enemies
    for(int i = 0; i < n; i++)
        enemies.push_back({glm::vec3((rand() % n) * 2 + 1, 0,
                                     (rand() % n) * 2 + 1),
                glm::vec3(0, 20, 0)});
    for(int i = 0; i < n; i++)
    {
        std::vector< int > t;
        for(int j = 0; j < n; j++)
            t.push_back(15);
        maze.push_back(t);
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
    glutTimerFunc(100, updates, 0);
    glutMainLoop();
  
    return 0;
}
