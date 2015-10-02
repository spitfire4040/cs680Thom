// include header
#include "loaderClass.h"

//Include GL Library
#include <GL/glew.h>
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GL/glut.h>

//GLM Library
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Include assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Include iostream
#include <iostream>
#include <vector>

// constructor
Loader::Loader()
   {
    // set faces to 0
    faces = 0;
   }

// copy constructor
Loader::Loader(Loader &src)
   {
    if(src.faces == 0)
	   {
	    faces = 0;
	   }
	   
    else
	   {
	    faces = src.faces;
		data = src.data;
	   }
   }

// destructor
Loader::~Loader()
   {
	data.clear();
	faces = 0;
   }
   
// read file to data
bool Loader::readFile(char *fileLoc)
   {
    // set up assimp scene
	const aiScene* scene = loader.ReadFile(fileLoc, aiProcess_Triangulate);

	// check for file open
	if( !scene )
	   {
        return false;
	   }

	// read file
	for(unsigned int outIndex = 0; outIndex < scene->mNumMeshes; outIndex++)
	   { 
   		// create mesh
		const aiMesh* objMesh = scene->mMeshes[outIndex];
		const aiMaterial* mat;
		aiColor4D objColor;
		
		// checks for material
		if(outIndex < scene->mNumMaterials)
		   {
		    mat = scene->mMaterials[outIndex];
		    if(AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_DIFFUSE , objColor))
		       {
	            objColor.r = 1.0;
			    objColor.g = 1.0;
			    objColor.b = 1.0;
		       }	       
		   }
	    else
		   {
			objColor.r = 1.0;
			objColor.g = 1.0;
			objColor.b = 1.0;
		   }

		//loop to get verticies and color
		for(unsigned int indexMesh = 0; indexMesh < objMesh->mNumFaces; indexMesh++)
		   {
			// create vertex temp
			Vertex temp;

			// update faces
			faces++;

			// get face with proper vertex coordinate
			const aiFace& face = objMesh->mFaces[indexMesh];

			// loop through all three faces
			for(int index = 0; index < 3; index++)
			   {
				//Get the next face vertex data
				const aiVector3D* position = &(objMesh->mVertices[face.mIndices[index]]);

				//Get the 3 verticies of the current face
				temp.position[0] = position->x;
				temp.position[1] = position->y;
				temp.position[2] = position->z;
				temp.color[0] = objColor.r;
				temp.color[1] = objColor.g;
				temp.color[2] = objColor.b;

				// push to vertex
				data.push_back(temp);
			   }
		   }
	   }

    return true;
   }
   
// set size of vertex and color arrays
int Loader::bufferSize()
   {
    // return size: *6 for num values, *3 for 3 verticies/face, *4 for GLfloat
    return faces * 6 * 3 * 4;
   }
   
// return number of faces
int Loader::numFaces()
   {
    // return number of faces *3 for 3 verticies/face
	return faces * 3;
   }
   
// get size of Vertex
int Loader::getSizeOf()
   {
    return sizeof(Vertex);
   }

// get offset of color to vertex
void* Loader::getOffSetColor()
   {
    return (void*)offsetof(Vertex,color);
   }

//Return data
Vertex* Loader::getData()
   {
    return &data[0];
   }
