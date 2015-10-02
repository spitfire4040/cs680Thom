***This is a readme file for PA3***

This is program PA3, moon orbiting planet. I was penalized a point on PA2 for a jumpy cube when direction was changed, and this has been fixed. I added a second model for a moon, added functionality to reverse the orbit of the main cube, and added text to indicate what was happening in the program (not extra credit for 680). The arrow keys were also incorporated to cause a change in rotation direction or to initiate a pause. Every part of this program is working perfectly on my machine running Ubuntu 14.04 and OpenGL 3.0. I have no trouble with seg-faults when I terminate the program in any combination.

In response to the email sent by graders, I came to the ECC to run my program, as it seemed impossible to fix a problem that doesn't exist on my machine, and to my suprise my program is a complete mess on the virtual machine running there wiht OpenGL 1.2 and XUbuntu. It is now too late for me to make changes, and to be honest I don't even know where to start. There must be libraries in OpenGL 3.0 that are different than those on the ECC machine. I'm bummed that I may be graded down on this project, as I spent a substantial amount of time getting it to work right only to find there is an outdated version that my program will be run on for evaluation. Just saying....

At any rate, here is what I have. Since I don't know what version/what releases I should have, or what the differences are, I am submitting this program that runs perfectly on my machine.


***********************************************************************************************************************

To compile the program, <cd> to build folder and type <make>. An executable called Matrix will be placed in the bin folder. ./Matrix to run the program.

When the executable is run, an orbiting cube will appear. Initially, the cube will be orbiting and rotating on it's Y-axis in a CCW direction (this is indicated in a text message at the top of the screen). A smaller black cube (the moon) will be orbiting the main cube in a CCW direction. Rotation and orbit of the main cube can be initiated in one of three ways; a left mouse-click on the screen, pressing <l> or <L> on the keyboard for left rotation, pressing <r> or <R> on the keyboard, or right-clicking mouse and choosing a rotation from the drop-down menu. The <left-arrow> and <right-arrow> keys will also change the rotation direction, and the <up-arrow> key will pause rotation. Any left mouse-click on the screen will reverse rotation. Keys <p> or <P> on the keyboard will pause rotation. <esc> on the keyboard will terminate program. Choices on the right click/dropdown menu are self explanitory...
