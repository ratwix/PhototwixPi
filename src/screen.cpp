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
		//init SDL8Image library
		
		//load textures
		background = graphic_util.load_image(IMG_BACKGROUND);
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




