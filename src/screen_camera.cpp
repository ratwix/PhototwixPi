#include "screen_camera.h"

ScreenCamera::ScreenCamera(Graphic_Util &graphic):
	Screen(graphic),
	camera(graphic)
{
	//init Screen Camera textures
	init();
}

bool ScreenCamera::init() {
	//TODO : load all selected template
	Template t = Template("./templates/template_img/retro_config1.png", graphic_util);

	return true;
}


void ScreenCamera::draw() {
	/* Write Background */
	graphic_util.drawRect(background, -1, -1, 1, 1, NULL);
	
	/* Update & draw Camera */
	camera.read_frame();
	camera.draw_camera(-1, -1, 0, 0);
	
	/* Draw template */
	
	/* Draw effects */
}

ScreenCamera::~ScreenCamera() {

}