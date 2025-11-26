#ifndef TEXT_H
#define TEXT_H

namespace mygllib
{
    class Text
    {
    public:
        Text(int x=0, int y=0,
             const std::string & s="",
             void * font=GLUT_STROKE_ROMAN)
            : x_(x), y_(y), s_(s), font_(font)
        {}
        
        void draw() const
        {
            glPushMatrix();
            glTranslatef(x_, y_, 0);
            glScalef(0.2, 0.2, 1);
            for (unsigned int i = 0; i < s_.length(); i++)
            {
                glutStrokeCharacter(font_, s_[i]);
            }
            glPopMatrix();
        }
        
        static void draw(int x, int y,
                         const std::string & s,
                         void * font=GLUT_STROKE_ROMAN)
        {
            Text(x, y, s, font).draw();        
        }
        
    private:
        int x_, y_;
        std::string s_;
        void * font_;
    };
}
#endif
