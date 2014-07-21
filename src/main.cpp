#include <GLES2/gl2.h>

#include <iostream>
#include <string>

#include "main_phototwix.h"
#include "util.h"
#include "control.h"

using namespace std;

int main(int argc, char** argv)
{
	//Init controls
	init_control();
	load_config();
	
	std::cout << "CAMERA TEST2:" << g_config.camera_width << "," << g_config.camera_height << std::endl;
	
	Main_Phototwix main;
	std::cout << "Main_Phototwix CREATED" << std::endl;
	main.start_phototwix();
	
    return 0;
}