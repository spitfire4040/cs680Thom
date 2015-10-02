#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "loaderClass.h"

// global variables
int w = 640, h = 480;
Loader newLoader;
float SCALE = 1.0;

GLuint program;
GLuint vertex_buffer;

// uniform locations
GLint mvp_location;
GLint position_location;
GLint color_location;

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

// resource management
bool initialize();
bool initialize_shaders();

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

  // create new loader
  newLoader.readFile( argv[1] );

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
 

  // initialize resources
  bool init = initialize();
  if(init)
  {
    t1 = std::chrono::high_resolution_clock::now();
    glutMainLoop();
  }

  //End program 
  glDeleteProgram(program);
  glDeleteBuffers(1, &vertex_buffer);
    return 0;
}

// implementations
void render()
{
    //Clear the buffer
  glClearColor(0.5, 0.6, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Multiply out the mvp
  mvp = projection * view * model;

  //Set the program
  glUseProgram(program);

  //upload the matrix to the shader
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

  //Set the buffer
  glEnableVertexAttribArray( position_location );
  glEnableVertexAttribArray( color_location );
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, newLoader.getSizeOf(), 0);
  glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, newLoader.getSizeOf(), newLoader.getOffSetColor());

  // render model
  glDrawArrays(GL_TRIANGLES, 0, newLoader.numFaces());

  //Clean pointers
  glDisableVertexAttribArray(position_location);
  glDisableVertexAttribArray(color_location);

  //Swap the bufer
  glutSwapBuffers();
}

void update()
{
  //Run a rotation to see all the models
  static float move = 0.0;
  float turn = (M_PI/2) * move;
  move += 0.01;
  model = glm::rotate(glm::mat4(1.0f), turn, glm::vec3(0.0, 0.3, 1.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(SCALE));

  //Redisplay the model
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
  // Create VBO to store vertex info on GPU
    //Initilize vertex buffer
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, newLoader.bufferSize(), newLoader.getData(), GL_STATIC_DRAW);

  // initialize shaders
  initialize_shaders();

  // set locations of attributes and uniforms
  position_location = glGetAttribLocation(program,
  const_cast<const char*>("v_position"));

  if(position_location == -1)
  {
    fprintf(stderr,"[F] Position not found\n");
    return false;
  }

  color_location = glGetAttribLocation(program,
  const_cast<const char*>("v_color"));

  if(color_location == -1)
  {
    fprintf(stderr,"[F] V_COLOR NOT FOUND\n");
    return false;
  }

  mvp_location = glGetUniformLocation(program,
  const_cast<const char*>("mvpMatrix"));
  if(mvp_location == -1)
  {
    fprintf(stderr,"[F] MVP MATRIX NOT FOUND\n");

    return false;
  }

  // init the view and projection matrices
  view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
  projection = glm::perspective( 45.0f, float(w) / float(h), 0.01f, 100.0f);

  // enable depth testing
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

  // compile shaders
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

// get time delta
float getDT()
{
  float ret;
  t2 = std::chrono::high_resolution_clock::now();
  ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
  t1 = std::chrono::high_resolution_clock::now();
  return ret;
}

// keyboard function for callback 
void keyboard(unsigned char key, int x_pos, int y_pos)
   {
    // check escape
  if(key == 27)
     {
      exit(0);
     }
     
    // if it is plus
  if(key == '=')
     {
      SCALE += 0.25;
     }
     
  // if it the minus
  if(key == '-')
     {
      SCALE -= 0.25;
     }
   }