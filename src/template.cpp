# include "template.h"
# include <iostream>
# include <fstream>

# define TEMPLATE_CONFIG_PATH "./templates/template_config/template_"

using namespace rapidxml;

Template::Template(const std::string f, Graphic_Util &g):
	filename(f),
	graphic_util(g)
{
	//Load image template to a texture
	base_tex = graphic_util.load_image(f);
	//Create a texture of the same size
	res_tex = new Gfx_Texture();
	res_tex->CreateRGBA(base_tex->GetWidth(), base_tex->GetHeight());
	
	//Identify config
	std::string xml_filename = TEMPLATE_CONFIG_PATH + filename.substr(filename.rfind("config"), filename.rfind(".") - filename.rfind("config")) + ".xml"; //get the config file --> foo_config3.png --> config3.xml
	cout << "Load config file : " << xml_filename << " for file " << filename << endl;
	
	//Read XML file
	ifstream theFile (xml_filename.c_str());
	assert(theFile);
	
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	xml_document<> doc;
	doc.parse<0>(&buffer[0]);
	cout << "End parsing XML" << endl;
	theFile.close();
	
	xml_node<> *template_config = doc.first_node("template_config");
	xml_node<> *cameras = template_config->first_node("cameras");
	for (xml_node<> * camera = cameras->first_node("camera"); camera; camera = camera->next_sibling())
	{
		//TODO : create a new Template_camera & put it on the vector
		
		
		std::cout << "New camera : x:" << camera->first_attribute("x")->value() << " y:" << camera->first_attribute("y")->value() << " h:" << camera->first_attribute("h")->value() << " w:" << camera->first_attribute("w")->value() << " rot:" << camera->first_attribute("rot")->value() << std::endl; 
	}
}

Template::~Template() 
{
	if (base_tex != NULL) {
		delete base_tex;
		base_tex = NULL;
	}

	if (res_tex != NULL) {
		delete res_tex;
		res_tex = NULL;
	}

}
