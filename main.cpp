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
    glPushMatrix();
    glTranslatef(0, 0, 0);
    drawRoom(maze[0][0]);
    glPopMatrix();
    for(unsigned int j = 1; j < maze.size(); j++)
    {
        glPushMatrix();
        glTranslatef(2*j, 0, 0);
        drawRoom(maze[0][j]&7);
        glPopMatrix();
    }
    for(unsigned int i = 1; i < maze.size(); i++)
    {
        glPushMatrix();
        glTranslatef(0, 0, 2*i);
        drawRoom(maze[i][0]&14);
        glPopMatrix();
        for(unsigned int j = 1; j < maze.size(); j++)
        {
            glPushMatrix();
            glTranslatef(2*j, 0, 2*i);
            drawRoom(maze[i][j]&6);
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);
    for(const auto & e : enemies)
    {
        glPushMatrix();
        glTranslatef(e.pos[0], .75, e.pos[2]);
        glScalef(.5, 1.5, .5);
        glutSolidCube(1);
        glPopMatrix();
    }
    for(const auto & b : bullets)
    {
        glPushMatrix();
        glTranslatef(b.pos[0], b.pos[1], b.pos[2]);
        glutSolidSphere(.1, 10, 10);
        glPopMatrix();
    }
    if(birdseye)
    {
        glPushMatrix();
        glTranslatef(player.x, 0.25, player.z);
        glRotatef(-player.angle * (180 / M_PI), 0, 1, 0);
        glutSolidCube(0.5);
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
    glm::vec3 travel;
    travel[0] = player.x - enemies[i].pos[0];
    travel[2] = player.z - enemies[i].pos[2];
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
        if(enemies[i].mov[1] == 20)
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
                        emov[2] = -.1;
                    break;
                case 1:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 2) &&
                       epos[0] < (maze.size() - 1) * 2)
                        emov[0] = .1;
                    break;
                case 2:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 4) &&
                       epos[2] < (maze.size() - 1) * 2)
                        emov[2] = .1;
                    break;
                case 3:
                    if(!(maze[(int)epos[2]/2][(int)epos[0]/2] & 8) &&
                       epos[0] > 2)
                        emov[0] = -.1;
                    break;
            }
        }
        enemies[i].pos += enemies[i].mov;
        enemies[i].mov[1] += 1;
        enemies[i].pos[1] = 0;
    }
    if(fired)
        fire = 10;
    glutPostRedisplay();
    glutTimerFunc(100, updates, fire);
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
