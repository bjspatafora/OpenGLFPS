// File  : SingletonView.cpp
// Author: smaug

#include "SingletonView.h"

mygllib::View * mygllib::SingletonView::instance_(new mygllib::View());


mygllib::View * mygllib::SingletonView::getInstance()
{
    return instance_;
}
