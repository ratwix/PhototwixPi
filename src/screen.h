#ifndef __SCREEN_H__
# define __SCREEN_H__


# include "util.h"
# include "graphic_util.h"

# define DESIGN_FILE "./design/design.xml"

class Screen {
	public:
		Screen(Graphic_Util &graphic);
		~Screen();
		bool init();
		
		virtual void draw() = 0;
		virtual void control(SDL_Event e) = 0;
		
	protected:
		//Screen
		Graphic_Util &graphic_util;

		//Textures
		Gfx_Texture		*background;	//background opengl texture
		
		static bool 	main_initialized;
};

#endif