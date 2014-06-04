#include <SDL2/SDL.h>
#include <fstream>
#include <cerrno>

#include "util.h"

void logSDLError(ostream &os, const string &msg) {
        os << msg << " error : " << SDL_GetError() << endl;
}

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  throw(errno);
}