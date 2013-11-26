// Basic OpenGL program
// Based on example code from Ed Angel's 6th edition textbook
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <gl/glew.h>
#include <GL/freeglut.h>

#define DEBUG_ON 0
#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))


typedef struct {
	float x, y;
} FloatType2D;

typedef struct {
	float r, g, b;
} FloatType3D;

const int nvertices = 4;
GLint m_location;
float M[16] = { 1, 0, 0, 0,  // 1, 0, 0, 0
                0, 1, 0, 0,  // 0, 1, 0, 0
                0, 0, 1, 0,  // 0, 0, 1, 0
                0, 0, 0, 1 };// 0, 0, 0, 1
//const int starvertices = 10;

float x_scale = 1, y_scale = 1;
float scale = 0.1;


// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
    FILE *fp;
	long length;
	char *buffer;
	
	// open the file containing the text of the shader code
	//fopen_s(&fp, shaderFile, "r");
	fp = fopen(shaderFile, "r");
	
	// check for errors in opening the file
    if ( fp == NULL ) { 
		printf("can't open shader source file %s\n", shaderFile); 
		return NULL; 
	}
	
	// determine the file size
    fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
    length = ftell(fp);  // return the value of the current position
	
	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];
	
	// read the appropriate number of bytes from the file
    fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
    fread(buffer, 1, length, fp); // read all of the bytes
	
	// append a NULL character to indicate the end of the string
    buffer[length] = '\0';
	
	// close the file
    fclose(fp);
	
	// return the string
    return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFileName, const char* fShaderFileName)
{	
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;
	
	// check GLSL version
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);
	
	// error check
	if ( vs_text == NULL ) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit( 1 );
	} else if (DEBUG_ON) {
		printf("read shader code:\n%s\n", vs_text);
	}
	if ( fs_text == NULL ) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit( 1 );
	} else if (DEBUG_ON) {
		printf("read shader code:\n%s\n", fs_text);
	}
	
	// Set shader source
	const char *vv = vs_text;
	const char *ff = fs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);
	glShaderSource(fragment_shader, 1, &ff, NULL);
	
	// Compile shaders
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);
	
	// error check
	GLint  compiled;
	glGetShaderiv( vertex_shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
		printf("vertex_shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv( vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize );
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog( vertex_shader, logMaxSize, &logLength, logMsg );
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	glGetShaderiv( fragment_shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
		printf("fragment_shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv( fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize );
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog( fragment_shader, logMaxSize, &logLength, logMsg );
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}

		exit(1);
	}
	
	// Create the program
	program = glCreateProgram();
	
	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
    // Link and set program to use
	glLinkProgram(program);
	glUseProgram(program);
	
    return program;
}


//----------------------------------------------------------------------------

void
init( void )
{
	FloatType2D vertices[nvertices];
	FloatType3D colors[4];
	GLuint vao[1], buffer, location, colorBuffer, color, program;
	

	// A hard-coded, simple object to look at
	vertices[0].x = -0.5;  vertices[0].y = -0.5;
	vertices[1].x =  0.5;  vertices[1].y = -0.5;
	vertices[2].x =  0.5;  vertices[2].y =  0.5;
	vertices[3].x = -0.5;  vertices[3].y =  0.5;



	colors[0].r = 1.0; colors[0].g = 0; colors[0].b = 0;
	colors[1].r = 1; colors[1].g = 0; colors[1].b = 0;
	colors[2].r = 0; colors[2].g = 0; colors[2].b = 1;
    colors[3].r = 0;   colors[3].g = 0; colors[3].b = 1;

	// Create a vertex array object
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    
    // Create and initialize a buffer object to hold the vertex data
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), vertices, GL_STATIC_DRAW );
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	
    // Initialize the vertex position attribute from the vertex shader
	location = glGetAttribLocation( program, "vertex_position" );
    glEnableVertexAttribArray( location );
    glVertexAttribPointer( location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	color = glGetAttribLocation(program, "vertex_color");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices) ));

	m_location = glGetUniformLocation(program, "M");

    glClearColor( 1, 1, 1, 1 );
	
}

//----------------------------------------------------------------------------

void
display_callback( void )
{
	glClear( GL_COLOR_BUFFER_BIT );     // fill the window with the background color

	M[0] = x_scale;
	M[5] = y_scale;

	glUniformMatrix4fv(m_location, 1, GL_FALSE, M);
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glFlush();					// ensure that all commands are pushed through the pipeline
}

//----------------------------------------------------------------------------

void
keyboard_callback( unsigned char key, int x, int y )
{
	fprintf(stdout, "Keypressed %i", key);
    switch ( key ) {

		case 033:  // octal ascii code for ESC
		case 'q':
		case 'Q':
			exit( 0 );
			break;
		case GLUT_KEY_LEFT:
			x_scale += scale;
			break;
		case GLUT_KEY_RIGHT:
			x_scale -= scale;
			
			break;
    }

	glutPostRedisplay();
}

void mouse_callback(int button, int state, int x, int y) {

}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
	glutInitWindowSize(200, 200);
    glutCreateWindow( "Basic Test 2" );
	glewExperimental = true;
	glewInit();
	
    init();
	
    glutDisplayFunc( display_callback );
    glutKeyboardFunc( keyboard_callback );
	glutMouseFunc(mouse_callback);

	
	
    glutMainLoop();

	
    return 0;
}

