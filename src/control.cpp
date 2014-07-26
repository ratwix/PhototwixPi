#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <ctime>
#include <SDL2/SDL.h>
#include <fstream>
#include <vector>

#include "control.h"
#include "util.h"
#include "screen.h"
#include "lib/rapidxml/rapidxml.hpp"

using namespace rapidxml;

//Global touch variable
mouse_position g_mouse;

bool g_finish_mouse_thread;

std::list<button_position> g_button_position_list;

/**
	read /dev/input/event0 for touch event
	*/

void task_mouse() {
	int fd = open(g_config.screen_touch_event.c_str(), O_RDONLY);
    struct input_event ev;
	
	while (1)
    {
        if (g_finish_mouse_thread) {
			break;
		}
		
		read(fd, &ev, sizeof(struct input_event));

        if (ev.type == 3) {
			SDL_Event e;
			
			e.type = 0;
		
			if (ev.code == 53) {				//Move X
				g_mouse.cur_x = ev.value;
				if (g_mouse.start_x == -1) {
					g_mouse.start_x = ev.value;
					if (g_mouse.start_y != -1) {
						e.type = SDL_FINGERDOWN;
					}
				} else {
					if (g_mouse.start_y != -1) {
						e.type = SDL_FINGERMOTION;
					}
				}
			}
			
			if (ev.code == 54) {				//Move Y
				g_mouse.cur_y = ev.value;
				if (g_mouse.start_y == -1) {
					g_mouse.start_y = ev.value;
					if (g_mouse.start_x != -1) {
						e.type = SDL_FINGERDOWN;
					}
				} else {
					if (g_mouse.start_x != -1) {
						e.type = SDL_FINGERMOTION;
					}
				}
			}
			
			if (ev.code == 57 && ev.value > 0) { //Press		
				g_mouse.press = true;
				g_mouse.start_x = -1;
				g_mouse.start_y = -1;
				//printf("Press\n");
			}
			
			if (ev.code == 57 && ev.value == -1) { //Release
				g_mouse.press = false;
				e.type = SDL_FINGERUP;
				//printf("Release\n");
			}
			
			if (e.type != 0) {
				e.tfinger.type = e.type;
				e.tfinger.timestamp = std::time(0);
				e.tfinger.touchId = 0;
				e.tfinger.fingerId = 1;
				e.tfinger.pressure = 1;
				e.tfinger.x = g_mouse.start_x * 2.0 / g_config.screen_touch_width - 1;
				e.tfinger.y = -(g_mouse.start_y * 2.0 / g_config.screen_touch_height - 1);
				e.tfinger.dx = g_mouse.cur_x * 2.0 / g_config.screen_touch_width - 1;
				e.tfinger.dy = -(g_mouse.cur_y * 2.0 / g_config.screen_touch_width - 1);
				
				SDL_PushEvent(&e);
			}
		}
    }
}

void init_control() {
	//Init a mouse in a different thread
	g_finish_mouse_thread = false;

	std::thread t1(task_mouse);
	t1.detach();
}

void addControl(int control, float x, float y, float dx, float dy) {
	//Remove all current item
	std::list<button_position>::iterator it;
	
	for (it = g_button_position_list.begin(); it != g_button_position_list.end(); it++) {
		if ((*it).control == control) {
			g_button_position_list.erase(it);
			break;
		}
	}
	
	button_position bp;
	
	bp.control = control;
	bp.x = x;
	bp.y = y;
	bp.dx = dx;
	bp.dy = dy;
	g_button_position_list.push_back(bp);
	
	//std::cout << "Control " << control << std::endl;
}

int check_control(float x, float y) {
	std::list<button_position>::iterator it;

	for (it = g_button_position_list.begin(); it != g_button_position_list.end(); it++) {
		button_position bp = (*it);
		
		if (x > bp.x && x < bp.dx && y < bp.y && y > bp.dy) {
			//std:cout << "Touch " << bp.control << std::endl;
			return bp.control;
		}
	}
	
	return 0;
}

void clearControl() {
	g_button_position_list.clear();
}

Button::Button(Graphic_Util &g, std::string name):
	graphic_util(g),
	selected(false)
{
	std::cout << "Parsing config XML for button " << name << std::endl;
	std::ifstream design_file (DESIGN_FILE);
	assert(design_file);
	vector<char> buffer((istreambuf_iterator<char>(design_file)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	rapidxml::xml_document<> doc;
	doc.parse<0>(&buffer[0]);
	std::cout << "End parsing config XML" << std::endl;
	design_file.close();
	
	rapidxml::xml_node<> *design_node = doc.first_node("design");
	rapidxml::xml_node<> *design_buttons = design_node->first_node("buttons");
	
	//For all button, look for the good one
	int tmp = 0;
	for (xml_node<> * design_button = design_buttons->first_node("button"); design_button; design_button = design_button->next_sibling())
	{
		std::string n = design_button->first_attribute("name")->value();
		if (n == name) { //Match a button name
			x = atof(design_button->first_attribute("x")->value());
			y = atof(design_button->first_attribute("y")->value());
			dx = atof(design_button->first_attribute("dx")->value());
			dy = atof(design_button->first_attribute("dy")->value());
			button_name = name;
			
			std::cout << "Load button " << name << " texture" << std::endl;
			texture = graphic_util.load_image(design_button->first_attribute("file")->value());
			
			if ((design_button->first_attribute("select") != NULL) && (design_button->first_attribute("select")->value() != "")) {
				select_texture = graphic_util.load_image(design_button->first_attribute("select")->value());
			}
			
			if (name == "BUTTON_GALLERY") {
				code = BUTTON_GALLERY;
			} else if (name == "BUTTON_EFFECT_NORMAL") {
				code = BUTTON_EFFECT_NORMAL;
			} else if (name == "BUTTON_EFFECT_BW") {
				code = BUTTON_EFFECT_BW;
			} else if (name == "BUTTON_EFFECT_SEPIA") {
				code = BUTTON_EFFECT_SEPIA;
			}
			
			addControl(code, x, y, dx, dy);

			break;
		}
	}
	
}

void Button::draw() {
	graphic_util.drawRect(texture, x, y, dx, dy, NULL);
	
	if (selected && (select_texture != NULL)) {
		graphic_util.drawRect(select_texture, x, y, dx, dy, NULL);
	}
}

Button::~Button() {
	if (texture != 0) {
		delete texture;
	}
	if (select_texture != 0) {
		delete select_texture;
	}
}