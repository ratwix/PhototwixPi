#include <iostream>
#include <fstream>
#include <vector>

#include "graphic_util.h"
#include "control.h"
#include "SDL_mixer.h"
#include <vector>
#include <iostream>
#include <fstream> 
#include <chrono>
#include <ctime>
#include <png.h>
#include <setjmp.h>
#include <png++/png.hpp>

Graphic_Util::Graphic_Util() {
	std::cout << "GRAPHIC UTIL" << std::endl;
	//Init SDL Opengl COntext at startup
	
	init_sdl();
	build_shader();
}

bool Graphic_Util::init_sdl() {
	// Initialisation de la librairie SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        logSDLError(cout, "SDL_Init");
        return false;
    }

	//Init OpenGL windows
	window = SDL_CreateWindow("Photobooth",SDL_WINDOWPOS_CENTERED,
														  SDL_WINDOWPOS_CENTERED,
														  g_config.screen_width,
														  g_config.screen_height,
														  SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	check();
	
	if( window )
    {
		glContext = SDL_GL_CreateContext(window);
		check();
		glEnable(GL_BLEND);
		check();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		check();
		SDL_GL_SetSwapInterval(1); //This makes our buffer swap syncronized with the monitor's vertical refresh
		check();
		SDL_GetWindowSize(window, &w_width, &w_height);
		cout << "Windows size : " << w_width << ":" << w_height << endl;
    }
    else
    {
       logSDLError(cout, "Erreur de création de la fenêtre");
	   return false;
	}
	
	//Init SDL_Image
	int flags = IMG_INIT_JPG|IMG_INIT_PNG;
	int initted = IMG_Init(flags);
		
	if((initted & flags) != flags) {
		cerr << "IMG_Init: Failed to init required jpg and png support!\n" << "IMG_Init: " << IMG_GetError() << endl;
		return false;
	}
	
	SDL_ShowCursor(0);
	
	 //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;    
    }
	
	return true;
}

Gfx_Texture * Graphic_Util::load_image( const string filename ) 
{
        SDL_Surface *surface;
        GLuint textureid;
        int mode;
        surface = IMG_Load(filename.c_str());
		
        // could not load filename
        if (!surface) {
				cerr << "Could not open image " << filename << endl;
                return 0;
        }

		//Create a RGB Texture
		Gfx_Texture	*texture = new Gfx_Texture();
		
        if (surface->format->BytesPerPixel == 3) { // RGB 24bit
                texture->mode = GL_RGB;
        } else if (surface->format->BytesPerPixel == 4) { // RGBA 32bit
                texture->mode = GL_RGBA;
        } else {
                SDL_FreeSurface(surface);
                return 0;
        }		
		
		texture->CreateRGBA(surface->w, surface->h, surface->pixels, texture->mode);
		cout << "Texture generee : " << filename << endl;
        // clean up
		
        SDL_FreeSurface(surface);
		
        return texture;
}

void Graphic_Util::build_shader() {
	GSimpleVS.LoadVertexShader("./shaders/simplevertshader.glsl");
	GSimpleFS.LoadFragmentShader("./shaders/simplefragshader.glsl");
	GYUVFS.LoadFragmentShader("./shaders/yuvfragshader.glsl");
	
	GSepiaFS.LoadFragmentShader("./shaders/sepiashader.glsl");
	
	// GBlurFS.LoadFragmentShader("./shaders/blurfragshader.glsl");
	// GSobelFS.LoadFragmentShader("./shaders/sobelfragshader.glsl");
	// GMedianFS.LoadFragmentShader("./shaders/medianfragshader.glsl");
	// GMultFS.LoadFragmentShader("./shaders/multfragshader.glsl");
	// GThreshFS.LoadFragmentShader("./shaders/threshfragshader.glsl");
	// GDilateFS.LoadFragmentShader("./shaders/dilatefragshader.glsl");
	// GErodeFS.LoadFragmentShader("./shaders/erodefragshader.glsl");
	// 
    GSimpleProg.Create(&GSimpleVS,&GSimpleFS);
	GYUVProg.Create(&GSimpleVS,&GYUVFS);
	
	GSepiaProg.Create(&GSimpleVS,&GSepiaFS);
	
	// GBlurProg.Create(&GSimpleVS,&GBlurFS);
	// GSobelProg.Create(&GSimpleVS,&GSobelFS);
	// GMedianProg.Create(&GSimpleVS,&GMedianFS);
	// GMultProg.Create(&GSimpleVS,&GMultFS);
	// GThreshProg.Create(&GSimpleVS,&GThreshFS);
	// GDilateProg.Create(&GSimpleVS,&GDilateFS);
	// GErodeProg.Create(&GSimpleVS,&GErodeFS);

	check();
	

	//create an ickle vertex buffer
	static const GLfloat quad_vertex_positions[] = {
		0.0f, 0.0f,	1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenBuffers(1, &GQuadVertexBuffer);
	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_positions), quad_vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check();
}

void Graphic_Util::drawRect(Gfx_Texture* texture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target) {
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GSimpleProg.GetId());	check();
	
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex"), 0);
	check();
	
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	assert(texture != NULL);

	glBindTexture(GL_TEXTURE_2D, texture->GetId());	check();
	
	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport ( 0, 0, w_width, w_height );
	}
}

void Graphic_Util::drawBlack(float x0, float y0, float x1, float y1, Gfx_Texture* render_target) {
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	
	
	glUseProgram(GSimpleProg.GetId());	check();
	
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex"), 0);
	check();
	
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	
	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport ( 0, 0, w_width, w_height );
	}
}

void Graphic_Util::drawRectSepia(Gfx_Texture* texture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target) {
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GSepiaProg.GetId());	check();
	
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex"), 0);
	check();
	
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	assert(texture != NULL);

	glBindTexture(GL_TEXTURE_2D, texture->GetId());	check();
	
	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport ( 0, 0, w_width, w_height );
	}
}

void Graphic_Util::drawYUVTextureRect(Gfx_Texture* ytexture, Gfx_Texture* utexture, Gfx_Texture* vtexture, float x0, float y0, float x1, float y1, Gfx_Texture* render_target) {
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GYUVProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GYUVProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GYUVProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex0"), 0);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex1"), 1);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex2"), 2);
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,ytexture->GetId());	check();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,utexture->GetId());	check();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,vtexture->GetId());	check();
	glActiveTexture(GL_TEXTURE0);

	GLuint loc = glGetAttribLocation(GYUVProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, w_width, w_height );
	}
}

void Graphic_Util::save_texture(Gfx_Texture* tex, string name) {
	std::cout << "Save file " << name << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, tex->GetFramebufferId());
	check();
	glViewport (0, 0, tex->GetWidth(), tex->GetHeight());
	check();
	int size = (tex->GetWidth() + 1) * (tex->GetHeight() + 1) * 4;
	GLubyte res[size];
	glReadPixels(0, 0, tex->GetWidth(), tex->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &res);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport ( 0, 0, w_width, w_height );
	check();
	
	save_png(name.c_str(), res, tex->GetWidth(), tex->GetHeight());
	std::cout << "End save file " << name << std::endl;
}

void Graphic_Util::save_texture_thumb(Gfx_Texture* tex, string name) {
	std::cout << "Save thumb " << name << std::endl;
	
	Gfx_Texture thumb;
	
	thumb.CreateRGBA(tex->GetWidth() / 3, tex->GetHeight() / 3);
	thumb.GenerateFrameBuffer();
	drawRect(tex, -1, -1, 1, 1, &thumb);
	
	save_texture(&thumb, name);
	
	std::cout << "End save thumb " << name << std::endl;
}


//Clear the screen before a new draw
void Graphic_Util::clear_screen() {
	glViewport ( 0, 0, w_width, w_height );
	check();
	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	check();
}

//Refresh a screen after drawing
void Graphic_Util::refresh_screen() {
	SDL_GL_SwapWindow(window);
	check();
}

Graphic_Util::~Graphic_Util() {
	SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
	SDL_Quit();
}

/******************************
	****	GFX Texture
	***************************/

bool Gfx_Texture::CreateRGBA(int width, int height, const void* data, GLint mode)
{
	Width = width;
	Height = height;
	glGenTextures(1, &Id);
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexImage2D(GL_TEXTURE_2D, 0, mode, Width, Height, 0, mode, GL_UNSIGNED_BYTE, data); //RGB || RGBA!!!!!
	check();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	check();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	check();
	return true;
}

bool Gfx_Texture::CreateGreyScale(int width, int height, const void* data)
{
	Width = width;
	Height = height;
	mode = GL_LUMINANCE;
	glGenTextures(1, &Id);
	check();
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Width, Height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	check();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool Gfx_Texture::GenerateFrameBuffer()
{
	//Create a frame buffer that points to this texture
	glGenFramebuffers(1, &FramebufferId);
	check();
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferId);
	check();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Id, 0);
	check();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	check();
	return true;
}

void Gfx_Texture::SetPixels(const void* data)
{
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, mode, GL_UNSIGNED_BYTE, data);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Gfx_Texture::~Gfx_Texture() {
	glDeleteTextures(1, &Id);
	if (FramebufferId != 0) {
		glDeleteFramebuffers(1, &FramebufferId);
	}
}



/******************************
	****	GFX Shader
	***************************/

bool Gfx_Shader::LoadVertexShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	check();
	//now create and compile the shader
	GlShaderType = GL_VERTEX_SHADER;
	Id = glCreateShader(GlShaderType);
	check();
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	check();
	glCompileShader(Id);
	//cout << glGetError() << endl;
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled == 0)
	{
		cout << "Failed to compile vertex shader " << filename << ":\n" << Src;
		printShaderInfoLog();
		glDeleteShader(Id);
		return false;
	}
	else
	{
		cout << "Compiled vertex shader " << filename << ":\n";
	}

	return true;
}

bool Gfx_Shader::LoadFragmentShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	//now create and compile the shader
	GlShaderType = GL_FRAGMENT_SHADER;
	Id = glCreateShader(GlShaderType);
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	glCompileShader(Id);
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled==0)
	{
		cout << "Failed to compile fragment shader " << filename << ":\n" << Src << endl;
		printShaderInfoLog();
		glDeleteShader(Id);
		return false;
	}
	else
	{
		cout << "Compiled fragment shader " << filename << ":\n" << endl;	
	}

	return true;
}

void Gfx_Shader::printShaderInfoLog()
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(Id, infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog : " << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}
}

bool Gfx_Program::Create(Gfx_Shader* vertex_shader, Gfx_Shader* fragment_shader)
{
	VertexShader = vertex_shader;
	FragmentShader = fragment_shader;
	Id = glCreateProgram();
	glAttachShader(Id, VertexShader->GetId());
	glAttachShader(Id, FragmentShader->GetId());
	glLinkProgram(Id);
	check();
	cout << "Created program id " << GetId() << " from vs " << VertexShader->GetId() << " and fs " << FragmentShader->GetId() << endl;

	// Prints the information log for a program object
	char log[1024];
	glGetProgramInfoLog(Id,sizeof log,NULL,log);
	//cout << Id << ":program:\n" << log << endl;

	return true;	
}

/******************************
	****	GFX Camera
	***************************/

Gfx_Camera::Gfx_Camera(Graphic_Util	&g):
	graphic_util(g)
{
	std::cout << "CAMERA:" << g_config.camera_width << "," << g_config.camera_height << std::endl;
	//Init the camera, 15fps, 1 resolution, no RGB conersion
	
	cam = StartCamera(g_config.camera_width, g_config.camera_height, 15, 1, false); 
	
	ytexture.CreateGreyScale(g_config.camera_width, g_config.camera_height);
	utexture.CreateGreyScale(g_config.camera_width / 2, g_config.camera_height / 2);
	vtexture.CreateGreyScale(g_config.camera_width / 2, g_config.camera_height / 2);

	yreadtexture.CreateRGBA(g_config.camera_width, g_config.camera_height);
	yreadtexture.GenerateFrameBuffer();
	
	ureadtexture.CreateRGBA(g_config.camera_width / 2, g_config.camera_height / 2);
	ureadtexture.GenerateFrameBuffer();
	
	vreadtexture.CreateRGBA(g_config.camera_width / 2, g_config.camera_height / 2);
	vreadtexture.GenerateFrameBuffer();
	
	rgbtextures.CreateRGBA(g_config.camera_width, g_config.camera_height);
	rgbtextures.GenerateFrameBuffer();
	
	resulttexture.CreateRGBA(g_config.camera_width, g_config.camera_height);
	resulttexture.GenerateFrameBuffer();
}

void Gfx_Camera::read_frame() {
	const void* frame_data; int frame_sz;
	while(!cam->BeginReadFrame(0, frame_data, frame_sz)) {};
	
	//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
	{
		const uint8_t* data = (const uint8_t*) frame_data;
		int ypitch = g_config.camera_width;
		int ysize = ypitch * g_config.camera_height;
		int uvpitch = g_config.camera_width / 2;
		int uvsize = uvpitch * g_config.camera_height / 2;
		int upos = ysize;
		int vpos = upos + uvsize;
		//printf("Frame data len: 0x%x, ypitch: 0x%x ysize: 0x%x, uvpitch: 0x%x, uvsize: 0x%x, u at 0x%x, v at 0x%x, total 0x%x\n", frame_sz, ypitch, ysize, uvpitch, uvsize, upos, vpos, vpos+uvsize);
		ytexture.SetPixels(data);
		utexture.SetPixels(data + upos);
		vtexture.SetPixels(data + vpos);
		cam->EndReadFrame(0);
	}
	
	//Transform YUV texture in RGB texture
	
	graphic_util.drawYUVTextureRect(&ytexture, &utexture, &vtexture, -1.f, -1.f, 1.f, 1.f, &rgbtextures);
	
	//these are just here so we can access the yuv data cpu side - opengles doesn't let you read grey ones cos they can't be frame buffers!
	
	graphic_util.drawRect(&ytexture, -1, -1, 1, 1, &yreadtexture);
	/*
	graphic_util.drawRect(&utexture, -1, -1, 1, 1, &ureadtexture);
	graphic_util.drawRect(&vtexture, -1, -1, 1, 1, &vreadtexture);
	*/
}

void Gfx_Camera::draw_camera(int effect) {
	//graphic_util.drawRect(&rgbtextures, x0, y0, x1, y1, NULL);
	//draw camera at -0.5; 0.5; calculate height
	
	if (effect == BUTTON_EFFECT_NORMAL) {
		graphic_util.drawRect(&rgbtextures, -1, -1, 1, 1, &resulttexture);
	} else if (effect == BUTTON_EFFECT_BW) {
		graphic_util.drawRect(&yreadtexture, -1, -1, 1, 1, &resulttexture);
	} else if (effect == BUTTON_EFFECT_SEPIA) {
		graphic_util.drawRectSepia(&rgbtextures, -1, -1, 1, 1, &resulttexture);
	}
	
	GLfloat cam_h = g_config.camera_height * 1.1 / g_config.camera_width / 2.0 * g_config.screen_width / g_config.screen_height;
	
	graphic_util.drawRect(&resulttexture, -0.55, cam_h, 0.55, -cam_h, NULL);
}

Gfx_Camera::~Gfx_Camera() {
	StopCamera();
}

typedef struct {
    GLubyte red;
    GLubyte green;
    GLubyte blue;
} pixel_t;


bool Graphic_Util::save_png(const char *filename, GLubyte *pixels, int w, int h)
{
	 std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	
	
	png::image< png::rgb_pixel > image(w, h);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			GLubyte r = pixels[4 * sizeof(GLubyte) * (w * y + x)];
			GLubyte g = pixels[4 * sizeof(GLubyte) * (w * y + x) + 1];
			GLubyte b = pixels[4 * sizeof(GLubyte) * (w * y + x) + 2];
			GLubyte a = pixels[4 * sizeof(GLubyte) * (w * y + x) + 3];
			image[y][x] = png::rgb_pixel(r, g, b);
		}
	}
	image.write(filename);
	
	
	std::cout << "Write PNG in  " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() << "s" << std::endl;
}
