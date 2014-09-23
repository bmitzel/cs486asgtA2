/* 
 * Programmer: Brian Mitzel
 * Email: bmitzel@csu.fullerton.edu
 * Course: CPSC 486
 * Assignment: A - Points, Planes, Rendering - Part 2
 * Due Date: 9/2/2014
 *
 * Filename: ply_viewer_glut.cpp
 *
 * This program loads and renders a PLY model using OpenGL.
 * The model rotates while the program is running.
 * 
 * The program was adapted from Professor Michael Shafae's
 * ply_viewer.cpp source code which used GLFW instead of
 * GLUT.
 *
 * This assignment is not graded.
 */

#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "PlyModel.h"

#define msglError( ) _msglError( stderr, __FILE__, __LINE__ )

FaceList* gModel;

bool _msglError( FILE *out, const char *filename, int line ){
	bool ret = false;
	GLenum err = glGetError( );
	while( err != GL_NO_ERROR ) {
		ret = true;
		fprintf( out, "GL ERROR:%s:%d: %s\n",
		filename, line, (char*)gluErrorString( err ) );
		err = glGetError( );
	}
	return( ret );
}

bool fileExists(const char *f){
	FILE *fh;
	bool rv = true;
	if( (fh = fopen(f, "r")) == NULL ){
		fprintf( stderr, "Opening file %s encountered an error.\n", f );
		rv = false;
	}else{
		fclose( fh );
	}
	return( rv );
}

void msglVersion(void){  
	fprintf(stderr, "Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(stderr, "Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stderr, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

double getElapsedTime( ){
	static struct timeval start;
	static int firstTime = 1;
	struct timeval now;
	struct timeval elapsed;
	if( firstTime ){
		firstTime = 0;
		gettimeofday(&start, NULL);
	}
	gettimeofday(&now, NULL);
	elapsed.tv_sec = now.tv_sec - start.tv_sec;
	elapsed.tv_usec = now.tv_usec - start.tv_usec;
	if (elapsed.tv_usec < 0){
		elapsed.tv_sec--;
		elapsed.tv_usec += 1000000;
	}
	double n = ((uint64_t) elapsed.tv_sec * (uint64_t) 1000000 + (uint64_t) elapsed.tv_usec) / 1000000.0;
	return n;
}

void updateCallback( int x ){
	glutPostRedisplay( );
	glutTimerFunc( 16, updateCallback, 0 );
}

void keyboardCallback( unsigned char key, int x, int y ){
	switch( key ){
	case 27: // The 'esc' key
	case 'q':
		glutLeaveMainLoop( );
		break;
	default:
		fprintf( stderr, "You pushed '%c' (%d).\n", key, key );
		break;
	}
}

void reshapeCallback( int width, int height ){
	if (height == 0){
		height = 1;
	}
	glViewport( 0, 0, (GLsizei)width, (GLsizei)height );
	double ratio = double(width) / double(height);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glFrustum(-ratio, ratio, -ratio, ratio, 1.0f, 40.0f);
}

void displayCallback()
{
	static int oldTime = 0;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt ( 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glRotatef( static_cast< float >( getElapsedTime() ) * 50.f, 0.577f, 0.577f, 0.577f );

	double scaleFactor = 1.0 / gModel->radius;
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	GLfloat ambient[] = { 0.5, 0.5, 0.5 };
	GLfloat diffuse[] = { 0.07568, 0.61424, 0.07568 };
	GLfloat specular[] = { 0.633, 0.727811, 0.633 };
	GLfloat shine = 0.6;
      
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
      
	glBegin( GL_TRIANGLES );
	for ( int i = 0; i < gModel->fc; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			glColor3dv(gModel->colors[gModel->faces[i][j]]);
			glNormal3dv(gModel->v_normals[gModel->faces[i][j]]);
			glVertex3dv(gModel->vertices[gModel->faces[i][j]]);
		}
	}
	glEnd();

	glutSwapBuffers();
}

int main( int argc, char* argv[] )
{
	// Initialize GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "PLY Viewer" );
	
	// Initialize GLEW
	glewExperimental = true;
	if( glewInit() != GLEW_OK )
	{
		fprintf( stderr, "GLEW init failed.\n" );
		exit( 1 );
	}
	
	// Set OpenGL state
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	glFrontFace( GL_CCW );
	glEnable(GL_NORMALIZE);
  
	// Register callbacks with GLUT
	glutKeyboardFunc( keyboardCallback );
	glutDisplayFunc( displayCallback );
	glutReshapeFunc( reshapeCallback );
	glutTimerFunc( 16, updateCallback, 0 );
	
	// Print version info
	msglVersion();
	
	// Load PLY object
	if ( fileExists( argv[1] ) )
	{
		gModel = readPlyModel( argv[1] );
	}
	else
	{
		exit( 1 );
	}
	
	// Set up a light
	glShadeModel( GL_SMOOTH );
	GLfloat l_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat l_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat l_position[] = { 0.0, 1.5, -1.5, 0.0 };

	glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, l_position );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
  
	glutMainLoop();

	return 0;
}
