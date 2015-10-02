#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <chrono>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "shaderClass.h"
#include <unistd.h>
#include <cstdlib> 

using namespace std;

// data types (vertex object)
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

// global variables
int w = 640, h = 480;
GLuint program;
GLuint vbo_geometry;
int numVertices;
char* fname;
unsigned int numOfVert = 0;
float amount = 0;
int colorIndex = 0;
char mtlTemp[30];
float c1, c2, c3;
float c4, c5, c6;
float c7, c8, c9;

// rotation flags
float dir = 1;
float speed = .01;
bool canSpin = false;

// uniform locations
GLint loc_mvpmat;

// attribute locations
GLint loc_position;
GLint loc_color;

// transform matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 mvp;

// GLUT callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void mouse(int button, int state, int x, int y);
void getColor(char* mtl);

// menu function
void menu(int id);

// resource management
bool initialize();
bool initialize_shaders();
Vertex* loadOBJ(char* obj);
float colorConv(float i);
void cleanUp();

// random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

//--Main
int main(int argc, char **argv)
{
  // check for args
  if( argc != 2)
  {
    //cout << "Command line arguments required!" << endl;
    system("clear");
    fprintf(stderr,"Command line arguments required!\n");
    exit(1);
  }
  // initialize glut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(w, h);

  // name and create the window
  glutCreateWindow("Matrix Example");

  // manage command line args
  int size = std::strlen(argv[1]);
  fname = new char[size];
  std::strcpy(fname, argv[1]);

  // initialize GLEW
  GLenum status = glewInit();

  if( status != GLEW_OK)
  {
    fprintf(stderr,"GLEW not initialized...\n");
    fprintf(stderr,"glewGetErrorString(status)\n");
      return -1;
  }

  // Set callbacks to GLUT
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutIdleFunc(update);

  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutCreateMenu(menu);

  glutAddMenuEntry("quit", 1);
  glutAddMenuEntry("start rotation", 2);
  glutAddMenuEntry("stop rotation", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // initialize resources
  bool init = initialize();
  if(init)
  {
    t1 = std::chrono::high_resolution_clock::now();
    glutMainLoop();
  }

  // Enable mouse menu
  glutCreateMenu(menu);
   glutMouseFunc(mouse);

  glutAddMenuEntry("quit", 1);
  glutAddMenuEntry("start rotation", 2);
  glutAddMenuEntry("stop rotation", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // Clean up after ourselves
  cleanUp();
  return 0;
}

//--Implementations
void render()
{
  //clear screen
  glClearColor(0.0, 0.0, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //premultiply matrix
  mvp = projection * view * model;

  //enable shader program
  glUseProgram(program);

  //upload matrix to shader
  glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

  //set up the Vertex buffer object
  glEnableVertexAttribArray(loc_position);
  glEnableVertexAttribArray(loc_color);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);

  //set pointers into the vbo
  glVertexAttribPointer( loc_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        0);

  glVertexAttribPointer( loc_color,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex,color));

  glDrawArrays(GL_TRIANGLES, 0, numOfVert);//mode, starting index, count

  //clean up
  glDisableVertexAttribArray(loc_position);
  glDisableVertexAttribArray(loc_color);
                       
  //swap the buffers
  glutSwapBuffers();
}

void update()
{
  //total time
  static float angle = 0.0;
  static float spin = 0.0;
  float dt = getDT();

  angle += dt * M_PI/2;

  model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)) );

  if(canSpin)
  {    
    //converts rad to angles then multiply by direction and speed of spin
    spin += speed * dir * dt * 90;   
  }
  model = model * glm::rotate(glm::mat4(1.0f), spin, glm::vec3(1.0f,1.0f, 1.0f));  
 
  // Update state of scene
  glutPostRedisplay();
}

void reshape(int n_w, int n_h)
{
  w = n_w;
  h = n_h;

  // set viewport
  glViewport( 0, 0, w, h);

  // update projection matrix
  projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);
}

bool initialize()
{
  // load model
  Vertex* geometry = loadOBJ(fname);

  // Create VBO to store vertex info on GPU
  glGenBuffers(1, &vbo_geometry);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
  glBufferData(GL_ARRAY_BUFFER, numOfVert * sizeof(Vertex), geometry, GL_STATIC_DRAW);

  // initialize shaders
  initialize_shaders();

  // set locations of attributes and uniforms
  loc_position = glGetAttribLocation(program,
  const_cast<const char*>("v_position"));

  if(loc_position == -1)
  {
    fprintf(stderr,"[F] Position not found\n");
    return false;
  }

  loc_color = glGetAttribLocation(program,
  const_cast<const char*>("v_color"));

  if(loc_color == -1)
  {
    fprintf(stderr,"[F] V_COLOR NOT FOUND\n");
    return false;
  }

  loc_mvpmat = glGetUniformLocation(program,
  const_cast<const char*>("mvpMatrix"));
  if(loc_mvpmat == -1)
  {
    fprintf(stderr,"[F] MVP MATRIX NOT FOUND\n");

    return false;
  }

  //--Init the view and projection matrices
  view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
  projection = glm::perspective( 45.0f, float(w) / float(h), 0.01f, 100.0f);

  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

bool initialize_shaders()
{
  // create object
  shader_loader loader;

  // create program
  program = glCreateProgram();

  //compile the shaders
  const char* vs = "shader.vs";
  const char* fs = "shader.fs";
  loader.add_shader(program, vs, GL_VERTEX_SHADER);
  loader.add_shader(program, fs, GL_FRAGMENT_SHADER);

  // link shader objects into program on GPU
  glLinkProgram(program);

  //check if linked properly
  GLint shader_status;
  glGetProgramiv(program, GL_LINK_STATUS, &shader_status);

  if(!shader_status)
  {
    fprintf(stderr,"[F] THE SHADER PROGRAM FAILED TO LINK\n");

    return false;
  }
  return true;
}

void cleanUp()
{
  // clean up
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_geometry);
}

// get time delta
float getDT()
{
  float ret;
  t2 = std::chrono::high_resolution_clock::now();
  ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
  t1 = std::chrono::high_resolution_clock::now();
  return ret;
}

// menu function for callback
void menu(int id)
{
  switch(id){
    case 1:
      exit(0);
      break;
    case 2:
      canSpin = true;
      break;
    case 3:
      canSpin = false;
      break;
}

glutPostRedisplay();
}

// keyboard function for callback 
void keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 'q':
      exit(0);  
      break;
    case 'Q':
      exit(0);
      break;  
    case 27:
      exit(0);
      break;
    case 'r':
      dir = dir * -1.0f;
      break;

    // rotation speed control
    case '1':
      speed = .01;
      break;
    case '2':
      speed = .02;
      break;
    case '3':
      speed = .03;
      break;
    case '4':
      speed = .04;
      break;
    case '5':
      speed = .05;
      break;
    case '6':
      speed = .06;
      break;
  }
}

//mouse function CB
void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    dir = dir * -1.0f;
  }
}

// model loader
Vertex* loadOBJ(char* obj)
{
  // initialize variables  
  int numRefVert = 0;

  // size of temporary vertice array
  int sizeOfTemp = 2;
  
  // flags
  bool hasVT = false;
  bool hasNorm = false;
  bool hasFourth = false; 
  int match = 0;
    
  // temporary Reference Vertices array
  Vertex* tempV = new Vertex[sizeOfTemp]; 
  std::vector< unsigned int > vertexIndices;
  FILE * file = fopen(obj, "r");

  // check for file open
  if(file == NULL)
  {
    system("clear");
    printf("Failed to open the file !\n");
    return NULL;
  }

  // loop until EOF
  while(1)
  {
    // initialize array
    char lineHeader[128];

    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);

    // check for EOF
    if(res == EOF)
    {
      break;
    }

    // get name of .mtl file
    if(strcmp(lineHeader, "mtllib") == 0)
    {
      fscanf(file, "%s\n", mtlTemp);
    }

    // check for vertices
    if(strcmp(lineHeader, "v") == 0)
    {
      GLfloat x, y, z;

      fscanf(file, "%f %f %f\n", &x, &y, &z);

      if(numRefVert >= sizeOfTemp)
      {
        //resize the vertex array
        Vertex* temp = new Vertex[2 * sizeOfTemp];

        for (int i = 0; i < sizeOfTemp; i++)
        {
          temp[i].position[0] = tempV[i].position[0];
          temp[i].position[1] = tempV[i].position[1];
          temp[i].position[2] = tempV[i].position[2];
          temp[i].color[0] = tempV[i].color[0];
          temp[i].color[1] = tempV[i].color[1];
          temp[i].color[2] = tempV[i].color[2];
        }

        sizeOfTemp = 2 * sizeOfTemp;
        delete[] tempV;
        tempV = temp; 
      }

      tempV[numRefVert].position[0] = x;
      tempV[numRefVert].position[1] = y;
      tempV[numRefVert].position[2] = z;
      numRefVert++;

    }
    else if(strcmp(lineHeader, "vt") == 0)
    {
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y);
      hasVT = true;
    }
    else if(strcmp(lineHeader, "vn") == 0)
    {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      hasNorm = true;
    }

    else if(strcmp( lineHeader, "f") == 0)
    {
      unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
      if(hasVT && hasNorm)
      {
        match = fscanf(file, "%i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3] );
        if(match == 12)
        {
          hasFourth = true;
        }
    
      }
      else if(!hasVT && hasNorm)
      {
        match = fscanf(file, "%i//%i %i//%i %i//%i %i//%i\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3]);
        if(match == 8)
        {
          hasFourth = true;
        }
        
      }
      else if(hasVT && !hasNorm)
      {
        match = fscanf(file, "%i/%i %i/%i %i/%i %i/%i\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2], &vertexIndex[3], &uvIndex[3]);
        if (match == 8)
        {
          hasFourth = true;
        }
      }
      else if(!hasVT && !hasNorm)
      { 
        match = fscanf(file, "%i %i %i %i\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2], &vertexIndex[3]);
        if(match == 4)
        {
          hasFourth = true;
        }
      }

      //saving face value for reference processing    
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);

      //quad conversion
      if(hasFourth)
      {
        vertexIndices.push_back(vertexIndex[0]);
        vertexIndices.push_back(vertexIndex[2]);
        vertexIndices.push_back(vertexIndex[3]);
      }   
    }
  }

  //exact sizing
  Vertex* temp = new Vertex[numRefVert];
  for(int i = 0; i < numRefVert; i++)
  {
    temp[i].position[0] = tempV[i].position[0];
    temp[i].position[1] = tempV[i].position[1];
    temp[i].position[2] = tempV[i].position[2];
    temp[i].color[0] = tempV[i].color[0];
    temp[i].color[1] = tempV[i].color[1];
    temp[i].color[2] = tempV[i].color[2];
  }

  delete[] tempV;
  tempV = temp; 

  // get color data from .mtl file
  getColor(mtlTemp);

  numOfVert = vertexIndices.size();
  
  //process data
  Vertex* out = new Vertex[numOfVert];
  
  for(unsigned int i=0; i< numOfVert; i++)
  {
    unsigned int vertexIndex = vertexIndices[i];
    out[i].position[0] = tempV[vertexIndex-1].position[0];
    out[i].position[1] = tempV[vertexIndex-1].position[1];
    out[i].position[2] = tempV[vertexIndex-1].position[2];

    if(i < (numOfVert/3))
    {
      out[i].color[0] = c1;
      out[i].color[1] = c2;
      out[i].color[2] = c3;
    }

    if((i >= numOfVert/3) && (1 < 2*(numOfVert/3)))
    {
      out[i].color[0] = c4;
      out[i].color[1] = c5;
      out[i].color[2] = c6;
    }

    if(i >= (2*(numOfVert/3)))
    {
      out[i].color[0] = c7;
      out[i].color[1] = c8;
      out[i].color[2] = c9;
    }
  }

  return out; 
}

void getColor(char* mtl)
{
  FILE * file = fopen(mtl, "r");

  // check for file open
  if(file == NULL)
  {
    system("clear");
    printf("Failed to open the .mtl file!\n");
  }

  else
  {
    while(1)
    {
      // initialize array
      char mtlHeader[128];

      // read the first word of the line
      int res = fscanf(file, "%s", mtlHeader);

      // check for EOF
      if(res == EOF)
      {
        break;
      }

      // get color values from .mtl
      if(strcmp(mtlHeader, "Kd") == 0)
      {
        if(colorIndex == 0)
        {
          fscanf(file, "%f %f %f\n", &c1,&c2,&c3);
        }
        if(colorIndex == 1)
        {
          fscanf(file, "%f %f %f\n", &c4,&c5,&c6);
        }
        if(colorIndex == 2)
        {
          fscanf(file, "%f %f %f\n", &c7,&c8,&c9);
        }
        colorIndex += 1;
      }
    }
  }
}

