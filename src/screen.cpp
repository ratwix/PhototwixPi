#include <fstream>
#include <cerrno>
#include <iostream>
#include <vector>
#include "lib/rapidxml/rapidxml.hpp"
#include "screen.h"

bool Screen::main_initialized = false;

Screen::Screen(Graphic_Util &graphic):
	graphic_util(graphic)
{
	//Init SDL screen 
	init();
}

bool Screen::init() {
	if (!main_initialized) {
		//First initialization of the screen
		
		//Load design.xml to load background img
		std::cout << "Parsing config XML" << std::endl;
		std::ifstream design_file (DESIGN_FILE);
		assert(design_file);
		vector<char> buffer((istreambuf_iterator<char>(design_file)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		rapidxml::xml_document<> doc;
		doc.parse<0>(&buffer[0]);
		std::cout << "End parsing config XML" << std::endl;
		design_file.close();
		rapidxml::xml_node<> *design_node = doc.first_node("design");
		rapidxml::xml_node<> *background_node = design_node->first_node("background");
		
		
		//load textures
		background = graphic_util.load_image(background_node->first_attribute("file")->value());
		assert(background != NULL);
		cout << "Background texture loaded" << endl;
		main_initialized = true;
	}
	return true;
}

//Free texture
Screen::~Screen() {
	if (background != NULL) {
		delete background;
		background = NULL;
	}
}




