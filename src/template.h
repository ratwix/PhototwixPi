#ifndef __TEMPLATE__
# define __TEMPLATE__

# include <list>
# include "util.h"
# include "graphic_util.h"
# include "lib/rapidxml/rapidxml.hpp"

class Template_Camera {
	public:
		Template_Camera(GLfloat x, GLfloat y, GLfloat dx, GLfloat dy, GLfloat rot);
		Gfx_Texture	tex;
		bool		active;	
		GLfloat 	x, y, dx, dy, rot;	
};

class Template {
	public:
		Template(rapidxml::xml_node<> *node, Graphic_Util &g);
		~Template();
		
		void draw();
		void active_control();
		int	 get_nb_photo();
		
		void clear_template();
		void set_photo(Gfx_Texture *t, int nb);
	private:
		GLfloat x, y, dx, dy;
		Graphic_Util &graphic_util;
		std::string filename;
		std::list<Template_Camera*> cams; 
		Gfx_Texture	*base_tex;
		Gfx_Texture	res_tex;
};



#endif