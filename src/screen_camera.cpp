#include <fstream>
#include <cerrno>
#include <iostream>
#include <vector>
#include "lib/rapidxml/rapidxml.hpp"
#include "screen_camera.h"
#include "control.h"

using namespace rapidxml;

ScreenCamera::ScreenCamera(Graphic_Util &graphic):
	Screen(graphic),
	camera(graphic),
	current_template(0),
	nb_template(0)
{
	//init Screen Camera textures
	init();
}

bool ScreenCamera::init() {
	load_templates();

	return true;
}


void ScreenCamera::draw() {
	/* Write Background */
	graphic_util.drawRect(background, -1, -1, 1, 1, NULL);
	
	/* Update Camera */

	camera.read_frame();
	camera.draw_camera();
	
	/* Update & Draw template */
	draw_current_template();
	
	/* Draw effects */
}

ScreenCamera::~ScreenCamera() {
	for (int i = 0; i < nb_template; i++) {
		delete templates[i];
	}
	delete[] templates;
}

//Load all templates
void ScreenCamera::load_templates() {
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
	rapidxml::xml_node<> *templates_node = design_node->first_node("templates");
	
	//For all template node, create a new template
	int tmp = 0;
	for (xml_node<> * template_node = templates_node->first_node("template"); template_node; template_node = template_node->next_sibling())
	{
		tmp++;
	}

	templates = new Template*[tmp];
	
	for (xml_node<> * template_node = templates_node->first_node("template"); template_node; template_node = template_node->next_sibling())
	{
		Template *t = new Template(template_node, graphic_util);
		nb_template++;
		templates[nb_template - 1] = t;
	}
	templates[current_template]->active_control();
}

void ScreenCamera::draw_current_template() {
	Template *c = templates[current_template];
	
	c->draw();
}

void ScreenCamera::next_template() {
	current_template++;
	
	if (current_template >= nb_template)
		current_template = 0;
		
	Template *c = templates[current_template];
	if (c != 0)
		templates[current_template]->active_control();
}

void ScreenCamera::control(SDL_Event e) {
	if (e.type == SDL_FINGERDOWN) {
		int b = check_control(e.tfinger.dx, e.tfinger.dy);
		if (b == BUTTON_TEMPLATE) {
			next_template();
		}
	}
}