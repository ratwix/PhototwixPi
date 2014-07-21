#ifndef __UTIL__
#define __UTIL__

#include <iostream>
#include <string>

#define	CONFIG_FILE	"config.xml"

using namespace std;

struct s_config {
	int 	screen_width;
	int 	screen_height;
	int 	screen_touch_width;
	int 	screen_touch_height;
	string	screen_touch_event;
	int 	camera_width;
	int 	camera_height;
};

extern s_config g_config;

void load_config();

void logSDLError(ostream &os, const string &msg);

std::string get_file_contents(const char *filename);

#endif