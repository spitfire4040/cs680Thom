***This is a readme file for PA2***

PA2 seemed relatively straight forward, most of the code being readily available in chapter two of the text. I was concerned about the use of global variables (rotVar and rotFlag) to make things work, but after speaking with one of the TA's from cs680, realized that these were necessary, but should be used with care. Functions keyboard(), myMouse(), and menu() were added as callbacks, as well as commands in main() to initialize menus. Beyond that, nothing of significance was changes. Each function contained a flag to reverse rotation from left to right, and a variable that is used in update() to control rotation is altered.

To compile the program, <cd> to build folder and type <make>. An executable called Matrix will be placed in the bin folder.

When the executable is run, an orbiting cube will appear. Initially, the cube will be orbiting, but not rotating on it's Y-axis. Rotation is initiated on one of three ways; a left mouse-click on the screen, pressing <l> or <L> on the keyboard for left rotation, pressing <r> or <R> on the keyboard, or right-clicking mouse and choosing a rotation from the drop-down menu. Any left mouse-click on the screen will reverse rotation. Keys <p> or <P> on the keyboard will pause rotation. <esc> on the keyboard will terminate program. Choices on the right-click/dropdown menu are self explanitory.
