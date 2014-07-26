#ifndef __SCREEN_CAMERA__
# define __SCREEN_CAMERA__

# include <list>
# include <vector>
# include <chrono>
# include "screen.h"
# include "graphic_util.h"
# include "template.h"
# include "control.h"
# include "SDL_mixer.h"
# define	MAX_LIGHT	4


using namespace std::chrono;

class ScreenCamera : public Screen {
	private:
		Graphic_Util &graphic_util;
		Gfx_Camera camera;
		Template **templates;
		int	nb_template;
		int	current_template;
		int current_effect;
		int	current_light;
		
		Button			button_gallerie;
		Button			button_effect_normal;
		Button			button_effect_bw;
		Button			button_effect_sepia;
		
		Gfx_Texture		*light_off;
		Gfx_Texture		*light_green;
		Gfx_Texture		*light_red;
		
		Mix_Chunk 		*sound_bip;
		Mix_Chunk 		*sound_photo;
		
		system_clock::time_point	time;
		
		bool			photo_in_process;
		bool			single_photo;
		int				current_photo;
		
		void load_templates();
		void draw_current_template();
		void load_lights();
		void draw_lights(GLfloat total, GLfloat current);
		void select_button(int button);
	public:
		ScreenCamera(Graphic_Util &graphic);
		~ScreenCamera();
		void next_template();
		
		bool init();
		void draw();
		void control(SDL_Event e);
		
		void start_take_photo();
};

#endif