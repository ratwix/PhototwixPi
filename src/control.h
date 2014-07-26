#ifndef __CONTROL_H__
# define __CONTROL_H__

# include <list>
# include "graphic_util.h"

# define BUTTON_TEMPLATE		0x01
# define BUTTON_GALLERY			0x02

# define BUTTON_EFFECT_NORMAL	0x10
# define BUTTON_EFFECT_BW		0x11
# define BUTTON_EFFECT_SEPIA	0x12

struct mouse_position {
	mouse_position() : cur_x(-1), cur_y(-1), start_x(-1), start_y(-1), press(false) {}
	int 	cur_x;
	int 	cur_y;
	int		start_x;
	int		start_y;
	bool	press;
};

struct button_position {
	int control;
	float x;
	float y;
	float dx;
	float dy;
};

extern bool g_finish_mouse_thread;

extern std::list<button_position> g_button_position_list;

void init_control();
void addControl(int control, float x, float y, float dx, float dy);
void clearControl();
int check_control(float x, float y);

class Button {
	public:
		Button(Graphic_Util &g, std::string name);
		void draw();
		~Button(); //Free texture
		bool	selected;
	private:
		Graphic_Util	&graphic_util;
		int				code;
		GLfloat 		x, y, dx, dy;
		std::string		button_name;
		Gfx_Texture		*texture;
		Gfx_Texture		*select_texture;
};

#endif