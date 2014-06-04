#ifndef __UTIL__
#define __UTIL__

#include <iostream>
#include <string>

using namespace std;

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 748;

static const int  CAMERA_WIDTH = 1280;
static const int  CAMERA_HEIGHT = 720;


void logSDLError(ostream &os, const string &msg);

std::string get_file_contents(const char *filename);

#endif