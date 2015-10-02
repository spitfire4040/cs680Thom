***This is a readme file for PA4***

This version of the program will load an .obj file from a directory specified by a full path name. The game board I created in Blender and it's .mtl file are in the bin directory, and so can be run without a full path name. The colors specified in the .mtl file are used to render the model. There are problems with the way the colors are assigned to the surfaces, couldn't quite get it straigtened out...but close. Once running, the right mouse click will bring up a menu to initiate a rotation, giving a better look at the model. 'r' will reverse rotation. '1' through '6' will adjust the speed of rotation.


**************************************************************************************************************************

To compile the program, 'cd' to build folder and type 'make'. An executable called Matrix will be placed in the bin folder. Input './Matrix' followed by 'path-to-file' to run the desired .obj file. The .obj and the .mtl need to be in the same directory to find one another.

When the executable is run, the specified OBJ file will be rendered.
