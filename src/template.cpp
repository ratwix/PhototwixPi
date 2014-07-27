# include <iostream>
# include <fstream>
# include <chrono>

# include "template.h"
# include "util.h"
# include "control.h"

using namespace rapidxml;

Template::Template(xml_node<> * node, Graphic_Util &g):
	graphic_util(g)
{
	std:cout << "New template " << node->first_attribute("file")->value() << std::endl;
	//Load image template to a texture
	base_tex = graphic_util.load_image(node->first_attribute("file")->value());
	//Create a texture of the same size
	res_tex.CreateRGBA(base_tex->GetWidth(), base_tex->GetHeight());
	res_tex.GenerateFrameBuffer();
	check();
	//Load all camera information
	xml_node<> *cameras = node->first_node("cameras");
	for (xml_node<> * camera = cameras->first_node("camera"); camera; camera = camera->next_sibling())
	{
		//TODO : create a new Template_camera & put it on the vector
		Template_Camera *tc = new Template_Camera(	atof(camera->first_attribute("x")->value()),
							atof(camera->first_attribute("y")->value()),
							atof(camera->first_attribute("dx")->value()),
							atof(camera->first_attribute("dy")->value()),
							atof(camera->first_attribute("rot")->value()));
		cams.push_back(tc);
	}
	
	GLfloat bx = base_tex->GetHeight() * 0.3 / base_tex->GetWidth() / 2 * g_config.screen_width / g_config.screen_height;
	x = -0.95;
	y = bx;
	dx = -0.65;
	dy = -bx;
}

Template::~Template() 
{
	std::list<Template_Camera*>::iterator i = cams.begin();
	while (i != cams.end()) {
		delete (*i);
		i++;
	}

	if (base_tex != NULL) {
		delete base_tex;
		base_tex = NULL;
	}


}

void Template::draw() {
	//Draw the res texture
	std::list<Template_Camera*>::iterator i = cams.begin();
	while (i != cams.end()) {
		if (!(*i)->active) { //Texture non active : draw black rectangle
			graphic_util.drawBlack((*i)->x, (*i)->y, (*i)->dx, (*i)->dy, &res_tex);
		} else {
			graphic_util.drawRect(&((*i)->tex), (*i)->x, (*i)->y, (*i)->dx, (*i)->dy, &res_tex);
		}
		i++;
	}
	
	graphic_util.drawRect(base_tex, -1, -1, 1, 1, &res_tex);
	graphic_util.drawRect(&res_tex, x, y, dx, dy, NULL);
	
}

void Template::active_control() {
	addControl(BUTTON_TEMPLATE, x, y, dx, dy);
}

int Template::get_nb_photo() {
	return cams.size();
}

void Template::clear_template() {
	std::list<Template_Camera*>::iterator i = cams.begin() ;

	while (i != cams.end()) {
		(*i)->active = false;
		i++;
	}
}

void Template::save() {
	std::cout << "Sauvegarde template " << std::endl;
	
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"photo_%Y-%m-%d_%I:%M:%S.png",timeinfo);
	std::string str(buffer); 
	
	graphic_util.save_texture(&res_tex, g_config.result_dir + "/" + str);
	graphic_util.save_texture_thumb(&res_tex, g_config.result_thumb_dir + "/" + str);
}

void Template::set_photo(Gfx_Texture *t, int nb) {
	std::cout << "Set photo " << nb << std::endl;
	std::list<Template_Camera*>::iterator i = cams.begin() ;
	
	while (i != cams.end()) {
		nb--;
		if (nb > 0) {
			i++;
		} else {
			break;
		}
	}
	
	graphic_util.drawRect(t, -1, -1, 1, 1, &((*i)->tex));
	(*i)->active = true;
}

Template_Camera::Template_Camera(GLfloat px, GLfloat py, GLfloat pdx, GLfloat pdy, GLfloat prot):
	x(px),
	y(py),
	dx(pdx),
	dy(pdy),
	rot(prot),
	active(false)
{
	tex.CreateRGBA(g_config.camera_width, g_config.camera_height);
	tex.GenerateFrameBuffer();
	std::cout << "New camera : x:" << x << " y:" << y << " dx:" << dx << " dy:" << dy << " rot:" << rot << std::endl; 
}

