#include <fstream>
#include <cerrno>
#include <iostream>
#include <vector>
#include <chrono>

#include "lib/rapidxml/rapidxml.hpp"
#include "screen_camera.h"
#include "control.h"

using namespace rapidxml;

ScreenCamera::ScreenCamera(Graphic_Util &graphic):
	graphic_util(graphic),
	Screen(graphic),
	camera(graphic),
	current_template(0),
	nb_template(0),
	button_gallerie(graphic, "BUTTON_GALLERY"),
	button_effect_normal(graphic, "BUTTON_EFFECT_NORMAL"),
	button_effect_bw(graphic, "BUTTON_EFFECT_BW"),
	button_effect_sepia(graphic, "BUTTON_EFFECT_SEPIA"),
	current_effect(BUTTON_EFFECT_NORMAL),
	current_light(0),
	photo_in_process(false)
{
	//init Screen Camera textures
	init();
}

bool ScreenCamera::init() {
	load_templates();
	load_lights();
	time = std::chrono::system_clock::now();

	//init sounds
	sound_bip = Mix_LoadWAV("./design/sound/bip.ogg");
	if (sound_bip == NULL) 
		std::cout << "tachatte" << std::endl;
	else
		std::cout << "pute" << std::endl;
	sound_photo = Mix_LoadWAV("./design/sound/photo.ogg");
	
	return true;
}


void ScreenCamera::draw() {
	/* Write Background */
	graphic_util.drawRect(background, -1, -1, 1, 1, NULL);
	
	/* Update Camera */

	camera.read_frame();
	camera.draw_camera(current_effect);
	
	/* Update & Draw template */
	draw_current_template();
	
	/* Draw buttons */
	button_gallerie.draw();
	button_effect_normal.draw();
	button_effect_bw.draw();
	button_effect_sepia.draw();
	
	if (photo_in_process) {	//We actually takes photos
		//If we still taking photos
		if (current_photo <= templates[current_template]->get_nb_photo()) {
			if (std::chrono::system_clock::now() > (time + std::chrono::seconds(1))) {	//Light a new light each second
				Mix_PlayChannel(-1, sound_bip, 0);
				current_light += 1;
				time = std::chrono::system_clock::now();
			}
			
			/* Draw lights */
			draw_lights(MAX_LIGHT, current_light);
			
			if (current_light > MAX_LIGHT) { //If all the lights on --> green light : take the photo
				//TODO : Flash
				if (single_photo) {
					Mix_PlayChannel(-1, sound_photo, 0);
					Gfx_Texture *photo = camera.get_result_photo();
					templates[current_template]->set_photo(photo, current_photo);
					single_photo = false;
				}
			}
			
			if (current_light > MAX_LIGHT + 1) { //If all the lights on --> green light : take the photo
				current_photo++;
				current_light = 0; //on recommence
				single_photo = true;
			}
		} else {
			photo_in_process = false;
			single_photo = true;
			//TODO : fin de la prise de photo
		}
	}
}

ScreenCamera::~ScreenCamera() {
	for (int i = 0; i < nb_template; i++) {
		delete templates[i];
	}
	delete[] templates;
	
	if (light_off)
		delete light_off;
	if (light_green)
		delete light_green;
	if (light_red)
		delete light_red;
	
	if (sound_bip)
		Mix_FreeChunk(sound_bip);
	if (sound_photo)
		Mix_FreeChunk(sound_photo);
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
	if (c != 0) {
		templates[current_template]->active_control();
		templates[current_template]->clear_template();
	}
}

void ScreenCamera::control(SDL_Event e) {
	if (e.type == SDL_FINGERDOWN) {
		int b = check_control(e.tfinger.dx, e.tfinger.dy);
		if (b == BUTTON_TEMPLATE) {
			next_template();
		} else if (b == BUTTON_GALLERY) {
			std::cout << "Press Button BUTTON_GALLERY" << std::endl;
			//TODO : REMOVE
			start_take_photo();
		} else if (b == BUTTON_EFFECT_NORMAL) {
			current_effect = BUTTON_EFFECT_NORMAL;
			select_button(BUTTON_EFFECT_NORMAL);
		} else if (b == BUTTON_EFFECT_BW) {
			current_effect = BUTTON_EFFECT_BW;
			select_button(BUTTON_EFFECT_BW);
		} else if (b == BUTTON_EFFECT_SEPIA) {
			current_effect = BUTTON_EFFECT_SEPIA;
			select_button(BUTTON_EFFECT_SEPIA);
		} 
	}
}

void ScreenCamera::select_button(int button) {
	button_effect_normal.selected = false;
	button_effect_bw.selected = false;
	button_effect_sepia.selected = false;
	
	if (button == BUTTON_EFFECT_NORMAL) {
		button_effect_normal.selected = true;
	} else if (button == BUTTON_EFFECT_BW) {
		button_effect_bw.selected = true;
	} else if (button == BUTTON_EFFECT_SEPIA) {
		button_effect_sepia.selected = true;
	} 
}

void ScreenCamera::load_lights() {
	std::cout << "Parsing config XML for lights " << std::endl;
	std::ifstream design_file (DESIGN_FILE);
	assert(design_file);
	vector<char> buffer((istreambuf_iterator<char>(design_file)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	rapidxml::xml_document<> doc;
	doc.parse<0>(&buffer[0]);
	std::cout << "End parsing config XML" << std::endl;
	design_file.close();
	
	rapidxml::xml_node<> *design_node = doc.first_node("design");
	rapidxml::xml_node<> *design_icons = design_node->first_node("icons");
	
	//For all button, look for the good one
	int tmp = 0;
	for (xml_node<> * design_icon = design_icons->first_node("icon"); design_icon; design_icon = design_icon->next_sibling())
	{
		std::string n = design_icon->first_attribute("name")->value();
		if (n == "LIGHT_OFF") { //Match a button name
			light_off = graphic_util.load_image(design_icon->first_attribute("file")->value());
		} else if (n == "LIGHT_RED") { //Match a button name
			light_red = graphic_util.load_image(design_icon->first_attribute("file")->value());
		} else if (n == "LIGHT_GREEN") {
			light_green = graphic_util.load_image(design_icon->first_attribute("file")->value());
		}
	}
}

void ScreenCamera::draw_lights(GLfloat total, GLfloat current) {
	GLfloat y = 0.9;
	GLfloat dy = 0.7;
	//GLfloat width = 1.0 * g_config.screen_touch_width * 0.2 / g_config.screen_touch_height; //calcul aspect ratio
	GLfloat width = 0.133;
	GLfloat inter = 0.02;
	
	for (GLfloat i = 1; i <= total; i = i + 1) {
		GLfloat x = (i - 1) * (width + inter) - (total / 2. * width + (total - 1) / 2. * inter);
		GLfloat dx = x + width;
		if (current > total) {
			graphic_util.drawRect(light_green, x, y, dx, dy, NULL);
		} else if (i <= current) {
			graphic_util.drawRect(light_red, x, y, dx, dy, NULL);
		} else {
			graphic_util.drawRect(light_off, x, y, dx, dy, NULL);
		}
	}
}

void ScreenCamera::start_take_photo() {
	photo_in_process = true;
	single_photo = true;
	current_photo = 1;
	current_light = 0;
	templates[current_template]->clear_template();
	time = std::chrono::system_clock::now();
}