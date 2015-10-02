#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaderClass.h"
#include <string.h>

using namespace std;

//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

//--Evil Global variables
//Just for this example!
int w = 640, h = 480;   // Window size
GLuint program;         // The GLSL program handle
GLuint vbo_geometry1;    // VBO handle for our geometry
GLuint vbo_geometry2;    // VBO handle for our geometry

// rotation flags
bool rotateForward = true;
bool rotateReverse = false;
bool orbitForward = true;
bool orbitReverse = false;

// text flags
bool rotateForwardText = true;
bool rotateReverseText = false;
bool orbitForwardText = true;
bool orbitReverseText = false;

// message pointers
char* textString1;
char* textString2;

// variable for message string (dynamic)
int len;

// message strings
char message1[20] = "Orbit: CW";
char message2[20] = "Orbit: CCW";
char message3[20] = "Orbit: Pause";
char message4[20] = "Rotation: CW";
char message5[20] = "Rotation: CCW";
char message6[20] = "Rotation: Pause";

//uniform locations
GLint loc_mvpmat1;// Location of the modelviewprojection matrix in the shader
GLint loc_mvpmat2;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position1;
GLint loc_position2;
GLint loc_color1;
GLint loc_color2;

//transform matrices
glm::mat4 model1;        //obj->world each object should have its own model matrix
glm::mat4 model2;        //obj->world each object should have its own model matrix
glm::mat4 view;         //world->eye
glm::mat4 projection;   //eye->clip
glm::mat4 mvp1;          //premultiplied modelviewprojection
glm::mat4 mvp2;          //premultiplied modelviewprojection

//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void mouse(int button, int state, int x, int y);
void arrows(int key, int x, int y);
void renderBitmapString(float x, float y, void *font, char *string);
void initializeText();

// Menu Function
void menu(int selection);

//--Resource management
bool initialize();
bool initialize_shaders();
void cleanUp();

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

//--Main
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);

    // Name and create the Window
    glutCreateWindow("Matrix Example");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }
    // initialize text to screen
    initializeText();

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);    // Called when its time to display
    glutReshapeFunc(reshape);   // Called if the window is resized
    glutIdleFunc(update);       // Called if there is nothing else to do
    glutKeyboardFunc(keyboard); // Called if there is keyboard input
    glutSpecialFunc(arrows);    // Called if there is an arrow key input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvp1 = projection * view * model1;
    mvp2 = projection * view * model2;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat1, 1, GL_FALSE, glm::value_ptr(mvp1));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position1);

    //glEnableVertexAttribArray(loc_position2);
    glEnableVertexAttribArray(loc_color1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry1);

    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position1,    //location of attribute
                           3,               //number of elements
                           GL_FLOAT,        //type
                           GL_FALSE,        //normalized?
                           sizeof(Vertex),  //stride
                           0);              //offset

    glVertexAttribPointer( loc_color1,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, 36);      //mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position1);
    glDisableVertexAttribArray(loc_color1);


    //upload the next matrix to the shader
    glUniformMatrix4fv(loc_mvpmat2, 1, GL_FALSE, glm::value_ptr(mvp2));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position2);

    //glEnableVertexAttribArray(loc_position2);
    glEnableVertexAttribArray(loc_color2);

    //glBufferData(GL_ARRAY_BUFFER, sizeof(geometry2), geometry2, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry2);

    //set pointers into the vbo for each of the attributes(position and color)

    glVertexAttribPointer( loc_position2,    //location of attribute
                           3,               //number of elements
                           GL_FLOAT,        //type
                           GL_FALSE,        //normalized?
                           sizeof(Vertex),  //stride
                           0);              //offset

    glVertexAttribPointer( loc_color2,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));


    glDrawArrays(GL_TRIANGLES, 0, 36);      //mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position2);
    glDisableVertexAttribArray(loc_color2);

    // set text on screen
    renderBitmapString(-0.2, 0.8, GLUT_BITMAP_TIMES_ROMAN_24, textString1);
    renderBitmapString(-0.25, 0.7, GLUT_BITMAP_TIMES_ROMAN_24,textString2);

    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float angle = 0.0;
    static float moonAngle = 0.0;
    static float rotationAngle = 0.0;
    float dt = getDT();// if you have anything moving, use dt.

    moonAngle += dt * M_PI/2;

    // Set orbit movement
    if(orbitForward && !orbitReverse)
    {
      angle += dt * M_PI/2; //move through 90 degrees a second
      if(textString1 != NULL)
      {
          delete[] textString1;
          textString1 = NULL;
      }
      len = strlen(message2);
      textString1 = new char[len];
      strcpy(textString1, message2);
    }

    else if(orbitReverse)
    {
      angle -= dt * M_PI/2; //move through -90 degrees a second
      if(textString1 != NULL)
      {
          delete[] textString1;
          textString1 = NULL;
      }
      len = strlen(message1);
      textString1 = new char[len];
      strcpy(textString1, message1);
    }

    else
    {
      // hold position
      if(textString1 != NULL)
      {
          delete[] textString1;
          textString1 = NULL;
      }
      len = strlen(message3);
      textString1 = new char[len];
      strcpy(textString1, message3);
    }

    // Set rotation movement
    if(rotateForward == true && !rotateReverse)
    {
      rotationAngle += dt * M_PI/4; // Rotate 45 degrees per second
      if(textString2 != NULL)
      {
          delete[] textString2;
          textString2 = NULL;
      }
      len = strlen(message5);
      textString2 = new char[len];
      strcpy(textString2, message5);
    }
      
    else if(rotateReverse)
    {
      rotationAngle -= dt * M_PI/4; // Rotate -45 degrees per second
      if(textString2 != NULL)
      {
          delete[] textString2;
          textString2 = NULL;
      }
      len = strlen(message4);
      textString2 = new char[len];
      strcpy(textString2, message4);
    }

    else
    {
      // hold position
      if(textString2 != NULL)
      {
          delete[] textString2;
          textString2 = NULL;
      }
      len = strlen(message6);
      textString2 = new char[len];
      strcpy(textString2, message6);
    }

    // Set the model location to the origin of the cube
    model1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));

    // Create an orbit
    model1 = glm::translate( model1, glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)));
    
    // Create a rotation
    model1 = glm::rotate(model1, rotationAngle, glm::vec3(0.0, 1.0, 0.0));

    // Get the new position of the cube
    glm::vec3 planetPos = glm::vec3(model1[3][0], model1[3][1], model1[3][2]);

    // Set the moon model to the origin of the planet cube
    model2 = glm::translate(glm::mat4(1.0f), planetPos);

    // Create a moon orbit
    model2 = glm::translate( model2, glm::vec3(4.0 * sin(3*moonAngle), 0.0, 4.0 * cos(3*moonAngle)));

    // Update the state of the scene
    glutPostRedisplay();
}

void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;

    //Change the viewport to be correct
    glViewport( 0, 0, w, h);

    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }

    if (key == 'a') //Start ccw rotation with 'a' key
    {
      rotateForward = true;
      rotateReverse = false;
    }

    if (key == 'd') //Start cw rotation with 'd' key
    {
      rotateForward = true;
      rotateReverse = true;
    }

    if(key == 's') //Stop rotation with 's' key
    {
      rotateForward = false;
      rotateReverse = false;
    }

        if (key == GLUT_KEY_LEFT) //Start ccw rotation with 'a' key
    {
      rotateForward = true;
      rotateReverse = false;
    }

    if (key == GLUT_KEY_RIGHT) //Start cw rotation with 'd' key
    {
      rotateForward = true;
      rotateReverse = true;
    }

    if(key == GLUT_KEY_UP) //Stop rotation with 's' key
    {
      rotateForward = false;
      rotateReverse = false;
    }

    if (key == 'A') //Start ccw orbit with 'A' key
    {
      orbitForward = true;
      orbitReverse = false;
    }

    if (key == 'D') //Start cw orbit with 'D' key
    {
      orbitForward = true;
      orbitReverse = true;
    }

    if(key == 'S') //Stop orbit with 'S' key
    {
      orbitForward = false;
      orbitReverse = false;
    }
}

bool initialize()
{
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    Vertex geometry1[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };

    Vertex geometry2[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry1), geometry1, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_geometry2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry2);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry2), geometry2, GL_STATIC_DRAW);

    //--Geometry done

    initialize_shaders();

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position1 = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position1 == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color1 = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(loc_color1 == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat1 = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat1 == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0),     //Eye Position
                        glm::vec3(0.0, 0.0, 0.0),       //Focus point
                        glm::vec3(0.0, 1.0, 0.0));      //Positive Y is up

    projection = glm::perspective( 45.0f,               //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h),   //Aspect Ratio, so Circles stay Circular
                                   0.01f,               //Distance to the near plane, normally a small value like this
                                   100.0f);             //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable mouse menu
    glutCreateMenu(menu);
    glutAddMenuEntry("Rotate Left", 1);
    glutAddMenuEntry("Pause Rotation", 2);
    glutAddMenuEntry("Rotate Right", 3);
    glutAddMenuEntry("Orbit Left", 4);
    glutAddMenuEntry("Pause Orbit", 5);
    glutAddMenuEntry("Orbit Right", 6);
    glutAddMenuEntry("Quit", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // call mouse function
    glutMouseFunc(mouse);

    //and its done
    return true;
}

bool initialize_shaders()
{
    shader_loader loader;
    program = glCreateProgram();

    //compile the shaders
    const char* vs = "shader.vs";
    const char* fs = "shader.fs";
    loader.add_shader(program, vs, GL_VERTEX_SHADER);
    loader.add_shader(program, fs, GL_FRAGMENT_SHADER);

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    glLinkProgram(program);

    //check if everything linked ok
    GLint shader_status;
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry1);
    glDeleteBuffers(1, &vbo_geometry2);
}

//returns the time delta
float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

void menu(int selection)
{
  switch(selection)
    {
      // ccw cube rotatation
      case 1:
        rotateForward = true;
        rotateReverse = false;
        break;

      // pause cube rotation
      case 2:
        rotateForward = false;
        rotateReverse = false;
        break;

      // cw cube rotation
      case 3:
        rotateForward = true;
        rotateReverse = true;
        break;

      // ccw cube orbit
      case 4:
        orbitForward = true;
        orbitReverse = false;
        break;

      // pause cube orbit
      case 5:
        orbitForward = false;
        orbitReverse = false;
        break;

      // cw cube orbit
      case 6:
        orbitForward = false;
        orbitReverse = true;
        break;

      // quit program
      case 7:
        exit(0);
        break;

      default:
        break;
    }
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
 if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && rotateReverse == true)
    {
      rotateForward = true;
      rotateReverse = false;
    }
 else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && rotateReverse == false)
    {
      rotateForward = true;
      rotateReverse = true;
    }
}

void arrows(int key, int x, int y)
{

  switch(key)
  {
    case GLUT_KEY_LEFT :          // ccw rotation
        rotateForward = true;
        rotateReverse = false;
        break;
    case GLUT_KEY_RIGHT :         // cw rotation
        rotateForward = true;
        rotateReverse = true;
        break;
    case GLUT_KEY_UP :            // pause rotation
        rotateForward = false;
        rotateReverse = false;
        break;
  }
}


void renderBitmapString(float x, float y, void *font, char *string)
{
  char *c;
  glRasterPos2f(x, y);
  for (c=string; *c != '\0'; c++)
  {
    glutBitmapCharacter(font, *c);
  }
}


void initializeText()
{
  // initialize text
  len = strlen(message2);
  textString1 = new char[len];
  strcpy(textString1, message2);
  len = strlen(message5);
  textString2 = new char[len];
  strcpy(textString2, message5);
}