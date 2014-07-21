#include <SDL2/SDL.h>
#include <fstream>
#include <cerrno>
#include <iostream>
#include <vector>
#include "lib/rapidxml/rapidxml.hpp"

#include "util.h"

using namespace rapidxml;

//global variable g_config
s_config g_config;

void logSDLError(ostream &os, const string &msg) {
        os << msg << " error : " << SDL_GetError() << endl;
}

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  throw(errno);
}

void load_config() {
	//Read XML file
	ifstream config_file(CONFIG_FILE);
	assert(config_file);
	
	vector<char> buffer((istreambuf_iterator<char>(config_file)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	xml_document<> doc;
	doc.parse<0>(&buffer[0]);
	cout << "End parsing XML config file" << endl;
	config_file.close();
	
	xml_node<> *phototwix = doc.first_node("phototwix");
	xml_node<> *screen = phototwix->first_node("screen");
	

	g_config.screen_width = std::stoi(screen->first_attribute("w")->value());
	g_config.screen_height = std::stoi(screen->first_attribute("h")->value());
	g_config.screen_touch_width = std::stoi(screen->first_attribute("touch_w")->value());
	g_config.screen_touch_height = std::stoi(screen->first_attribute("touch_h")->value());
	g_config.screen_touch_event = screen->first_attribute("event")->value();
	
	xml_node<> *camera = phototwix->first_node("camera");
	g_config.camera_width = std::stoi(camera->first_attribute("w")->value());
	g_config.camera_height = std::stoi(camera->first_attribute("h")->value());
	
	std::cout << "CAMERA TEST:" << g_config.camera_width << "," << g_config.camera_height << std::endl;
	
}