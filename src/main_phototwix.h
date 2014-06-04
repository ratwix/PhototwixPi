#ifndef __MAIN_PHOTOTWIX__
# define __MAIN_PHOTOTWIX__

# include "util.h"
# include "screen.h"
# include "graphic_util.h"

class Main_Phototwix {
	private :	
		Graphic_Util	graphic_util;
		
		Screen *screen_main;
		Screen *screen_camera;
		//Screen *gallery_screen;
		
		bool init_sdl();
		bool init_sdl_screen();
		
		void game_loop();
	public :	
		void start_phototwix();
		Main_Phototwix();
		~Main_Phototwix();
};

#endif