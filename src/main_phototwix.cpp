#include <time.h>

#include "main_phototwix.h"
#include "util.h"
#include "screen.h"
#include "screen_camera.h"

Main_Phototwix::Main_Phototwix() {
	return;
} 

Main_Phototwix::~Main_Phototwix() {
	//TODO: free all the screen
	delete screen_camera;
}

void Main_Phototwix::start_phototwix() {
	//Initialize screen
	screen_camera = new ScreenCamera(graphic_util);
	
	//start game loop
	game_loop();
	SDL_Delay(3000); /* Attendre trois secondes, que l'utilisateur voie la fenêtre//TODO a supprimer */
}

void Main_Phototwix::game_loop() {
	//clear screen
	//TODO
	
	//set the master screen to camera screen
	screen_main = screen_camera;
	
	//draw content of current screen
	for (int i = 0; i < 300; i++) { //TODO a remplacer par un while 1
		//cout << i << endl;
		graphic_util.clear_screen();
		
		if (screen_main != NULL) {
			screen_main->draw();
		}
		
		//Update the screen
		graphic_util.refresh_screen();
	}
}