#ifndef __GRAPHIC_UTIL__
# define __GRAPHIC_UTIL__

# include <GLES2/gl2.h>
# include <EGL/egl.h>
# include <EGL/eglext.h>
# include <SDL_image.h>
# include <assert.h>

# include "util.h"

# include "lib/camera/camera.h"

# define check() assert(glGetError() == 0)
//# define check() {}

class Gfx_Shader
{
	GLchar* Src;
	GLuint Id;
	GLuint GlShaderType;

public:
	Gfx_Shader() : Src(NULL), Id(0), GlShaderType(0) {}
	~Gfx_Shader() { if(Src) delete[] Src; }
	
	void printShaderInfoLog();
	bool LoadVertexShader(const char* filename);
	bool LoadFragmentShader(const char* filename);
	GLuint GetId() { return Id; }
};

class Gfx_Program
{
	Gfx_Shader* VertexShader;
	Gfx_Shader* FragmentShader;
	GLuint Id;

public:
	Gfx_Program() {}
	~Gfx_Program() {}

	bool Create(Gfx_Shader* vertex_shader, Gfx_Shader* fragment_shader);
	GLuint GetId() { return Id; }
};

class Gfx_Texture
{
	int Width;
	int Height;
	GLuint Id;

	
	GLuint FramebufferId;
public:

	Gfx_Texture() : Width(0), Height(0), Id(0), FramebufferId(0) {}
	~Gfx_Texture();

	GLint mode; //GL_RGB GL_RGBA GL_LUMINANCE
	
	bool CreateRGBA(int width, int height, const void* data = NULL, GLint mode = GL_RGBA);
	bool CreateGreyScale(int width, int height, const void* data = NULL);
	bool GenerateFrameBuffer();
	void SetPixels(const void* data);
	GLuint GetId() { return Id; }
	GLuint GetFramebufferId() { return FramebufferId; }
	int GetWidth() {return Width;}
	int GetHeight() {return Height;}
	
	
	
};

class Graphic_Util;

class Gfx_Camera
{
	private:
		CCamera* cam;
		
		Graphic_Util	&graphic_util;
		
		Gfx_Texture ytexture,utexture,vtexture;
		Gfx_Texture yreadtexture,ureadtexture,vreadtexture;
		Gfx_Texture rgbtextures;
		
		Gfx_Texture	resulttexture;
		
	public:
		Gfx_Camera(Graphic_Util &g);
		~Gfx_Camera();
		void read_frame();
		void draw_camera(int effect);
		Gfx_Texture *get_result_photo() {return &resulttexture;}
};

class Graphic_Util {
	private :	
		//Context
		SDL_Window* window;
		SDL_GLContext glContext;

		//Shaders
		Gfx_Shader GSimpleVS;
		Gfx_Shader GSimpleFS;
		Gfx_Shader GYUVFS;
		
		Gfx_Shader GSepiaFS;
		
		//Program
		Gfx_Program GSimpleProg;
		Gfx_Program GYUVProg;
		Gfx_Program GSepiaProg;
		
		GLuint GQuadVertexBuffer;
		
		int	w_height;
		int w_width;
		
		bool init_sdl();
		void build_shader();
	public :
		void	clear_screen();
		void	refresh_screen();
		
		
		Gfx_Texture	*load_image( std::string filename );
		//Draw a texture
		void drawRect(Gfx_Texture* texture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target);
		void drawRectSepia(Gfx_Texture* texture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target);
		//Combine YUV texture in a RGB Texture
		void drawYUVTextureRect(Gfx_Texture* ytexture, Gfx_Texture* utexture, Gfx_Texture* vtexture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target);
		void drawBlack(float x0, float y0, float x1, float y1, Gfx_Texture* render_target);
		
		SDL_Window* getWindow(){return window;};
		SDL_GLContext getGlContext(){return glContext;};
		Graphic_Util();
		~Graphic_Util();
};



#endif