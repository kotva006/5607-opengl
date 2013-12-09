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

#define PI 3.14159265

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 400
#define ANG 1 * PI / 180

typedef struct {
	float x, y, z, w;
} FloatType4D;

typedef struct {
	float r, g, b;
} FloatType3D;

void mul(float *R1, float *R2, float *ret) {

	ret[0] = R1[0] * R2[0] + R1[4] * R2[1] + R1[8] * R2[2];
	ret[4] = R1[0] * R2[4] + R1[4] * R2[5] + R1[8] * R2[6];
	ret[8] = R1[0] * R2[8] + R1[4] * R2[9] + R1[8] * R2[10];

	ret[1] = R1[1] * R2[0] + R1[5] * R2[1] + R1[9] * R2[2];
	ret[5] = R1[1] * R2[4] + R1[5] * R2[5] + R1[9] * R2[6];
	ret[9] = R1[1] * R2[8] + R1[5] * R2[9] + R1[9] * R2[10];

	ret[3] = R1[2] * R2[0] + R1[6] * R2[1] + R1[10] * R2[2];
	ret[6] = R1[2] * R2[4] + R1[6] * R2[5] + R1[10] * R2[6];
	ret[10] = R1[2] * R2[8] + R1[6] * R2[9] + R1[10] * R2[10];

}

const int nvertices = 8;
GLint m_location;
float M[16] = { 1, 0, 0, 0,  // 1, 0, 0, 0
                0, 1, 0, 0,  // 0, 1, 0, 0
                0, 0, 1, 0,  // 0, 0, 1, 0
                0, 0, 0, 1 };// 0, 0, 0, 1

float Rx[16] = { 1, 0,         0,        0,
                 0, cos(ANG),  sin(ANG), 0,
                 0, -sin(ANG), cos(ANG), 0,
                 0, 0,         0,        1 };

float nRx[16] = { 1, 0,          0,         0,
                  0, cos(-ANG),  sin(-ANG), 0,
                  0, -sin(-ANG), cos(-ANG), 0,
                  0, 0,          0,         1 };

float Ry[16] = { cos(ANG), 0, -sin(ANG), 0,
                 0,        1, 0,         0,
                 sin(ANG), 0, cos(ANG),  0,
                 0,        0, 0,         1 };

float nRy[16] = { cos(-ANG), 0, -sin(-ANG), 0,
                  0,         1, 0,          0,
                  sin(-ANG), 0, cos(-ANG),  0,
                  0,         0, 0,          1 };

float Rz[16] = { cos(ANG),  sin(ANG), 0, 0,
                 -sin(ANG), cos(ANG), 0, 0,
                 0,         0,        1, 0,
                 0,         0,        0, 1 };

float nRz[16] = { cos(-ANG), sin(-ANG), 0, 0,
                  -sin(-ANG),  cos(-ANG), 0, 0,
                  0,         0,         1, 0,
                  0,         0,         0, 1 };

float T[16] = { 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1 };

float R[16] = { 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1 };


float x_scale = 1, y_scale = 1;
float scale = 0.1;

bool l_state = false, r_state = false;
int s_key = 0;


float angle = 1.0;
float trans = 0.012;
float translation_x = 0, translation_y = 0;

float m_s_p_x = 0; //mouse starting position x
float m_s_p_y = 0; //mouse starting position y

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
	FloatType4D vertices[nvertices];
	FloatType3D colors[nvertices];
	GLuint vao[1], buffer, location, colorBuffer, color, program;
	
	// A hard-coded, simple object to look at
	vertices[0].x = -0.5;  vertices[0].y = -0.5;  vertices[0].z = -0.5;  vertices[0].w = 1;
	vertices[1].x = -0.5;  vertices[1].y = -0.5;  vertices[1].z = 0.5;   vertices[1].w = 1;
	vertices[2].x = 0.5;   vertices[2].y = -0.5;  vertices[2].z = 0.5;   vertices[2].w = 1;
	vertices[3].x = 0.5;   vertices[3].y = -0.5;  vertices[3].z = -0.5;  vertices[3].w = 1;
	vertices[4].x = 0.5;   vertices[4].y = 0.5;   vertices[4].z = -0.5;  vertices[4].w = 1;
	vertices[5].x = 0.5;   vertices[5].y = 0.5;   vertices[5].z = 0.5;   vertices[5].w = 1;
	vertices[6].x = -0.5;  vertices[6].y = 0.5;   vertices[6].z = 0.5;   vertices[6].w = 1;
	vertices[7].x = -0.5;  vertices[7].y = 0.5;   vertices[7].z = -0.5;  vertices[7].w = 1;

	colors[0].r = 0;     colors[0].g = 0;    colors[0].b = 0;
	colors[1].r = 0;     colors[1].g = 0;    colors[1].b = 1;
	colors[2].r = 1;     colors[2].g = 0;    colors[2].b = 1;
    colors[3].r = 1;     colors[3].g = 0;    colors[3].b = 0;
	colors[4].r = 1;     colors[4].g = 1;    colors[4].b = 0;
	colors[5].r = 1;     colors[5].g = 1;    colors[5].b = 1;
	colors[6].r = 0;     colors[6].g = 1;    colors[6].b = 1;
	colors[7].r = 0;     colors[7].g = 1;    colors[7].b = 0;

	glEnable(GL_DEPTH_TEST);
	
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
    glVertexAttribPointer( location, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	color = glGetAttribLocation(program, "vertex_color");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices) ));

	m_location = glGetUniformLocation(program, "M");

    glClearColor( 0.3, 0.3, 0.3, 1 );
	
	
}

//----------------------------------------------------------------------------

void
display_callback( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // fill the window with the background color

	//Set the scaleing
	//M[0] = x_scale;
	//M[5] = y_scale;

	//Set the rotation
	//M[0] = x_scale * Rx[0];
	//M[1] = x_scale * Rx[1];
	//M[4] = y_scale * Rx[4];
	//M[5] = y_scale * Rx[5];

	mul(T, R, M);

	glLineWidth(5.0);

	//Translation
	M[12] = translation_x;
	M[13] = translation_y;

	glUniformMatrix4fv(m_location, 1, GL_FALSE, M);
	glDrawArrays( GL_TRIANGLE_FAN, 0, nvertices );
	glFlush();					// ensure that all commands are pushed through the pipeline
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard_callback( unsigned char key, int x, int y )
{
	printf("Keypressed %i", key);
    switch ( key ) {

		case 033:  // octal ascii code for ESC
		case 'q':
		case 'Q':
			exit( 0 );
			break;
    }

	glutPostRedisplay();
}

void
key_callback(int key, int x, int y)
{
	//printf("Keypressed %i", key);
	switch (key) {

	case GLUT_KEY_LEFT:
		x_scale -= scale;
		break;
	case GLUT_KEY_RIGHT:
		x_scale += scale;
		break;
	case GLUT_KEY_UP:
		y_scale += scale;
		break;
	case GLUT_KEY_DOWN:
		y_scale -= scale;
		break;
	}

	glutPostRedisplay();
}

void mouse_callback(int button, int state, int x, int y) {

	switch (button) {

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			r_state = true;
		if (state == GLUT_UP)
			r_state = false;
		break;
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			l_state = true;
			m_s_p_x = x;
			m_s_p_y = y;
		}
		if (state == GLUT_UP)
			l_state = false;
		break;
	}

	s_key = glutGetModifiers();

	glutPostRedisplay();

}

void mouse_motion(int x, int y) {

	if (l_state && s_key == GLUT_ACTIVE_CTRL) {
		//printf("Motion %i %i\n", x, y);
		if (m_s_p_x < x) {
			mul(nRz, R, R);
		}
		else if (m_s_p_x > x) {
			mul(Rz, R, R);
		}
	}
	else if (l_state && s_key != GLUT_ACTIVE_ALT) {
		if (m_s_p_x < x) {
			mul(nRy, R, R);
		}
		else if (m_s_p_x > x) {
			mul(Ry, R, R);
		}
		if (m_s_p_y < y) {
			mul(nRx, R, R);
		}
		else if (m_s_p_y > y) {
			mul(Rx, R, R);
		}
	}
	else if (l_state && s_key == GLUT_ACTIVE_ALT) {
		
		if (m_s_p_x < x)
			translation_x += trans;
		else if (m_s_p_x > x)
			translation_x -= trans;

		if (m_s_p_y < y)
			translation_y -= trans;
		else if (m_s_p_y > y)
			translation_y += trans;

	}

	m_s_p_x = x;
	m_s_p_y = y;

	glutPostRedisplay();

}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow( "Image Transformer Beta" );
	glewExperimental = true;
	glewInit();
	
    init();
	
    glutDisplayFunc( display_callback );
    glutKeyboardFunc( keyboard_callback );
	glutSpecialFunc(key_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_motion);
	
    glutMainLoop();

    return 0;
}