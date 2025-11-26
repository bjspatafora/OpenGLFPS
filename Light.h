#ifndef LIGHT_H
#define LIGHT_H

namespace mygllib
{
    class Light
    {
    public:
        Light(GLenum id=GL_LIGHT0,
              float ambR=0, float ambG=0, float ambB=0, float ambA=1,
              float diffR=1, float diffG=1, float diffB=1, float diffA=1,
              float specR=1, float specG=1, float specB=1, float specA=1,
              float x=5, float y=10, float z=5, float w=0,
              float spot_dir_x=0, float spot_dir_y=-1, float spot_dir_z=0,
              float spot_cutoff=45)
            : id_(id), spot_cutoff_(spot_cutoff)
        {
            ambient_[0] = ambR;   ambient_[1] = ambG;
            ambient_[2] = ambB;   ambient_[3] = ambA;
            diffuse_[0] = diffR;  diffuse_[1] = diffG;
            diffuse_[2] = diffB;  diffuse_[3] = diffA;
            specular_[0] = specR; specular_[1] = specG;
            specular_[2] = specB; specular_[3] = specA;
            position_[0] = x;     position_[1] = y;
            position_[2] = z;     position_[3] = w;

            spot_dir_[0] = spot_dir_x;
            spot_dir_[1] = spot_dir_y;
            spot_dir_[2] = spot_dir_z;
        }

        float & x()       { return position_[0]; }
        float   x() const { return position_[0]; }
        float & y()       { return position_[1]; }
        float   y() const { return position_[1]; }
        float & z()       { return position_[2]; }
        float   z() const { return position_[2]; }
        float & w()       { return position_[3]; }
        float   w() const { return position_[3]; }

        float & spot_dir_x()       { return spot_dir_[0]; }
        float   spot_dir_x() const { return spot_dir_[0]; }
        float & spot_dir_y()       { return spot_dir_[1]; }
        float   spot_dir_y() const { return spot_dir_[1]; }
        float & spot_dir_z()       { return spot_dir_[2]; }
        float   spot_dir_z() const { return spot_dir_[2]; }
        
        float & spot_cutoff()       { return spot_cutoff_; }
        float   spot_cutoff() const { return spot_cutoff_; }

        void on() const             { glEnable(id_); }
        void off() const            { glDisable(id_); }
        void set_ambient() const    { glLightfv(id_, GL_AMBIENT, ambient_); }
        void set_diffuse() const    { glLightfv(id_, GL_DIFFUSE, diffuse_); }
        void set_specular() const   { glLightfv(id_, GL_SPECULAR, specular_); }
        void set_position() const   { glLightfv(id_, GL_POSITION, position_); }
        void set()                  { glLightfv(id_, GL_AMBIENT, ambient_);
                                      glLightfv(id_, GL_DIFFUSE, diffuse_);
                                      glLightfv(id_, GL_SPECULAR, specular_);
                                      glLightfv(id_, GL_POSITION, position_);
                                    }
        static void all_on()        { glEnable(GL_LIGHTING); }
        static void all_off()       { glDisable(GL_LIGHTING); }
    
    private:
        GLenum id_;
        GLfloat ambient_[4];
        GLfloat diffuse_[4];
        GLfloat specular_[4];
        GLfloat position_[4];
        GLfloat spot_dir_[3];
        GLfloat spot_cutoff_;
    };


    class LightModel
    {
    };
}

#endif
