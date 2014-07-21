# include <iostream>
# include <fstream>

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
	res_tex = new Gfx_Texture();
	res_tex->CreateRGBA(base_tex->GetWidth(), base_tex->GetHeight());
	
	//Load all camera information
	xml_node<> *cameras = node->first_node("cameras");
	for (xml_node<> * camera = cameras->first_node("camera"); camera; camera = camera->next_sibling())
	{
		//TODO : create a new Template_camera & put it on the vector
		Template_Camera tc(	atof(camera->first_attribute("x")->value()),
							atof(camera->first_attribute("y")->value()),
							atof(camera->first_attribute("w")->value()),
							atof(camera->first_attribute("h")->value()),
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
	if (base_tex != NULL) {
		delete base_tex;
		base_tex = NULL;
	}

	if (res_tex != NULL) {
		delete res_tex;
		res_tex = NULL;
	}

}

void Template::draw() {
	//TODO: update the res texture
	
	//TODO: draw the res texture
	
	graphic_util.drawRect(base_tex, x, y, dx, dy, NULL);
	
}

void Template::active_control() {
	addControl(BUTTON_TEMPLATE, x, y, dx, dy);
}

Template_Camera::Template_Camera(GLfloat px, GLfloat py, GLfloat pwidth, GLfloat pheight, GLfloat prot):
	x(px),
	y(py),
	width(pwidth),
	height(pheight),
	rot(prot),
	update(true)
{
	tex.CreateRGBA(g_config.camera_width, g_config.camera_height);
	std::cout << "New camera : x:" << x << " y:" << y << " h:" << height << " w:" << width << " rot:" << rot << std::endl; 
}