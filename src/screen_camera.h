#ifndef __SCREEN_CAMERA__
# define __SCREEN_CAMERA__

# include <list>
# include <vector>
# include "screen.h"
# include "graphic_util.h"
# include "template.h"


class ScreenCamera : public Screen {
	private:
		Gfx_Camera camera;
		Template **templates;
		int	nb_template;
		int	current_template;
		
		void load_templates();
		void draw_current_template();
	public:
		ScreenCamera(Graphic_Util &graphic);
		~ScreenCamera();
		void next_template();
		
		bool init();
		void draw();
		void control(SDL_Event e);
};

#endif