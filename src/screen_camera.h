#ifndef __SCREEN_CAMERA__
# define __SCREEN_CAMERA__

# include "screen.h"
# include "graphic_util.h"
# include "template.h"


class ScreenCamera : public Screen {
	private:
		Gfx_Camera camera;
		
	public:
		ScreenCamera(Graphic_Util &graphic);
		~ScreenCamera();
		
		bool init();
		void draw();
};

#endif