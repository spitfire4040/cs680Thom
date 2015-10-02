// include GL Library
#include <GL/glew.h>
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GL/glut.h>

// include assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// other libs
#include <iostream>
#include <vector>

struct Vertex
   {
    GLfloat position[3];
    GLfloat color[3];
   };

class Loader
{
   public:
      // constructor/desructor
      Loader();
      Loader(Loader &src);
      ~Loader();
	  
	  // read file
	  bool readFile(char *fileLoc);
      bool readFile_Alternate(char *fileLoc);
	  
	  //get buffer size
	  int bufferSize();
	  int numFaces();
	  int getSizeOf();
	  void* getOffSetColor();
	  Vertex* getData();
	  
   private:
	  int faces;
	  std::vector<Vertex> data;
	  Assimp::Importer loader;
};
