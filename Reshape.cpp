// File  : Reshape.cpp
// Author: smaug

#include "View.h"
#include "SingletonView.h"
#include "Reshape.h"

void mygllib::Reshape::reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    mygllib::View * pview = mygllib::SingletonView::getInstance();
    pview->aspect() = double(w) / h;
    pview->set_projection();
    pview->lookat();
}
