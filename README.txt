 * Programmer: Brian Mitzel
 * Email: bmitzel@csu.fullerton.edu
 * Course: CPSC 486
 * Assignment: A - Points, Planes, Rendering - Part 2
 * Due Date: 9/2/2014
 
ply_viewer_glut.cpp is a C++ program that loads a PLY model
and renders it in a window using OpenGL. The model rotates
as long as the program is running.

Building:
Using the FreeBSD VM distributed by Professor Michael
Shafae, open a terminal window to the source directory and
enter the following command at the shell prompt:

	gmake

Executing:
From the same directory where you built the executable,
enter the command:

	./ply_viewer <filename>
	
Where filename is the name of the PLY model to be rendered.
For example:

	./ply_viewer box.ply
	
box.ply is included with this program as a demonstration.

Features:
Q or Esc will exit the program.

Completed:
This program renders a PLY model as required by the
assignment.

Bugs:
There are no known bugs at this time.
