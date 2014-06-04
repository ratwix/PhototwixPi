#ifndef __TEMPLATE__
# define __TEMPLATE__

# include <vector>
# include "util.h"
# include "graphic_util.h"
# include "lib/rapidxml/rapidxml.hpp"

class Template_Camera {
	public:
		Template_Camera(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat rot);
	private:
		GLfloat 	x, y, width, height, rot;
		bool		update;
		Gfx_Texture	tex;
};

class Template {
	public:
		Template(const std::string f, Graphic_Util &g);
		~Template();
	private:
		Graphic_Util &graphic_util;
		std::string filename;
		std::vector<Template_Camera> cams; 
		Gfx_Texture	*base_tex;
		Gfx_Texture	*res_tex;
};



#endif