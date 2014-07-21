#include <time.h>
#include <gpm.h>
#include <stdio.h>

#include "main_phototwix.h"
#include "util.h"
#include "screen.h"
#include "screen_camera.h"
#include "control.h"

Main_Phototwix::Main_Phototwix():
	screen_camera(0)
{
	std::cout << "Main_Phototwix" << std::endl;
	return;
} 

Main_Phototwix::~Main_Phototwix() {
	//TODO: free all the screen
	if (screen_camera) {
		delete screen_camera;
	}
}

void Main_Phototwix::start_phototwix() {
	//Initialize screen
	screen_camera = new ScreenCamera(graphic_util);
	
	//start game loop
	game_loop();
	g_finish_mouse_thread = true;
}

void Main_Phototwix::game_loop() {
	//clear screen
	SDL_Event event;
	
	//set the master screen to camera screen

	std::cout << "LOOP" << std::endl;
	screen_main = screen_camera;
	bool stop = false;
	
	int i = 0;
	
	//draw content of current screen
	while(!stop) { //TODO a remplacer par un while 1
		i++;
		//TODO: Test a effacer
		/*
		if (i == 100 || i == 200) {
			((ScreenCamera *) screen_main)->next_template();
		}
		*/
		
		while(SDL_PollEvent(&event)) {
			 switch(event.type) {
				case SDL_QUIT:
					stop = true;
					break;
				case SDL_FINGERDOWN:
					screen_main->control(event);
					//std::cout << "Press x:" << event.tfinger.dx << " y:" << event.tfinger.dy << std::endl;
					break;
				case SDL_FINGERMOTION:
					screen_main->control(event);
					//std::cout << "Move from x:" << event.tfinger.x << " y:" << event.tfinger.y << " to x:" << event.tfinger.dx << " y:" << event.tfinger.dy << std::endl;
					break;
				case SDL_FINGERUP:
					screen_main->control(event);
					//std::cout << "Up x:" << event.tfinger.dx << " y:" << event.tfinger.dy << std::endl;
					break;
			 }
		}
		
		graphic_util.clear_screen();

		if (screen_main != NULL) {
			screen_main->draw();
		}

		//Update the screen
	
		graphic_util.refresh_screen();
	}
}
