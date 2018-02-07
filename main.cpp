#include <GL/glut.h>
#include "Maths/Maths.h"
#include "scene.h"

//Timer used for frame rate independent movement
int timer = 0;

//Camera & light positions
VECTOR3D cameraPosition(0.0f, 1.8f,-3.5f);
VECTOR3D lightPosition(2.0f, 2.6f,-2.0f);

const int shadowMapSize = 512;

GLuint shadowMapTexture;

int windowWidth, windowHeight;

MATRIX4X4 lightProjectionMatrix, lightViewMatrix;
MATRIX4X4 cameraProjectionMatrix, cameraViewMatrix;

bool Init(){

	// Load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Shading states
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


	// Depth states
	glClearDepth(1.0f);

	/*
	glDepthFunc specifies the function used to compare each incoming pixel depth value
    with the depth value present in the depth buffer.
    The comparison is performed only if depth testing is enabled.
    */
	// Passes if the incoming depth value is less than or equal to the stored depth value.

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);


    glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	// Create the shadow map texture
    glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
					GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Use the color as the ambient and diffuse material
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// White specular material color, shininess 16
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);

	// Calculate & save matrices
	glPushMatrix();

	glLoadIdentity();
	gluPerspective(45.0f, (float)windowWidth/windowHeight, 1.0f, 100.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjectionMatrix);

	glLoadIdentity();
	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);

	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 2.0f, 8.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightProjectionMatrix);

	glLoadIdentity();
	gluLookAt(	lightPosition.x, lightPosition.y, lightPosition.z,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);

	glPopMatrix();

	// Reset timer
    timer = 0;

	return true;
}


void display(){

    /* Called to draw the scene */

	// angle of spheres in scene. Calculate from time
    timer += 2;
    float angle = timer;

	// First pass - from light's point of view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(lightProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lightViewMatrix);

	// Use viewport the same size as the shadow map
	glViewport(0, 0, shadowMapSize, shadowMapSize);

	// Draw back faces into the shadow map
	glCullFace(GL_FRONT);

	// Disable color writes, and use flat shading for speed
	glShadeModel(GL_FLAT);
	glColorMask(0, 0, 0, 0);

	// Draw the scene
	DrawScene(angle);

	// Read the depth buffer into the shadow map texture
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

	// restore states
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);

		/*
		In the second pass, we draw the scene from the camera's point of view,
		with the light set to the brightness of the shadowed areas. Firstly,
		clear the depth buffer. There is no need to clear the color buffer since it has not
		yet been written to. Then, set up the matrices to draw from the camera's point of view,
		and use a viewport which covers the whole window.
		*/

	// 2nd pass - Draw from camera's point of view
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cameraViewMatrix);

	glViewport(0, 0, windowWidth, windowHeight);

	// Use dim light to represent shadowed areas
	glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(lightPosition));
	glLightfv(GL_LIGHT1, GL_AMBIENT, white*0.2f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white*0.2f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, black);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);


    // draw the model
	DrawScene(angle);


	// 3rd pass
	// Draw with bright light

	/*
	The third pass is where the actual shadow calculations take place.
	If a fragment passes the shadow test (i.e. is unshadowed) then we want it to be lit brightly,
	overwriting the dim pixel from the previous pass. So, enable a bright light, with full specular brightness.
	*/
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	// Calculate texture matrix for projection
	// This matrix takes us from eye space to the light's clip space
	// It is postmultiplied by the inverse of the current view matrix when specifying texgen

	/*
	when you transform vertices inside the frustum with a standard modelview/projection matrix,
	the result you get is a vertex that, once w-divide is done, is in the [-1:1]x[-1:1]x[-1:1] cube
	. you want your texture coordinates to be in the [0:1]x[0:1] range, hence the remapping for x
	and y. It's the same kind of thing for Z, assuming your DepthRange is [0:1],

	*/
	static MATRIX4X4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f);	//bias from [-1, 1] to [0, 1]
	MATRIX4X4 textureMatrix=biasMatrix*lightProjectionMatrix*lightViewMatrix;

	// Set up texture coordinate generation.
	// selects a texture-coordinate generation function or supplies coefficients for one
	// of the functions. coord names one of the (s, t, r, q)
	// If the texture generation function is GL_EYE_LINEAR, the function  = g = p1' xe + p2' ye + p3' ze + p4' we
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(0));
	glEnable(GL_TEXTURE_GEN_S);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrix.GetRow(1));
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrix.GetRow(2));
	glEnable(GL_TEXTURE_GEN_R);

	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrix.GetRow(3));
	glEnable(GL_TEXTURE_GEN_Q);

	/*
    Now we bind and enable the shadow map texture, and set up the automatic shadow comparison. First we enable
	the comparison, then tell the GL to generate a "true" result if r is less than or equal to the value stored in
	the texture. The shadow comparison produces either a 0 or 1 per fragment for a result. We instruct the GL to
	replicate this to all 4 color channels, i.e. to generate an intensity result.
    */


	// Bind & enable shadow map texture
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glEnable(GL_TEXTURE_2D);

	// Enable shadow comparison
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

	// Shadow comparison should be true (ie not in shadow) if r<=texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	// Shadow comparison should generate an INTENSITY result
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

	// Set alpha test to discard false comparisons

    /*
    If the shadow comparison passes, an alpha value of 1 will be generated. So, we use the alpha test to
    discard all fragments with alpha less than 0.99. This way, fragments which fail the shadow test will not be
    displayed, so allowing the darker result from the previous step to show.

    */

	glAlphaFunc(GL_GEQUAL, 0.99f);
	glEnable(GL_ALPHA_TEST);
    // The scene is then drawn for the third and final time, then any changed states are reset.

	// We draw the shadow
	DrawScene(angle);

	// Disable textures and texgen
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	// Restore other states
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);


	/*
      glFinish does not return until the effects of all previously
            called GL commands are complete.
	*/

	glFinish();
	glutSwapBuffers();
	glutPostRedisplay();
}


void reshape(int w, int h) {

    /* Called on window resize*/

	// Save new window size into the global variables
	windowWidth = w, windowHeight = h;

	// Update the camera's projection matrix
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45.0f, (float)windowWidth/windowHeight, 1.0f, 100.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjectionMatrix);
	glPopMatrix();
}

void keypress(unsigned char key, int x, int y) {
	// If escape is pressed, exit
	if(key==27)
		exit(0);

}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 512);
	glutCreateWindow("Shadow Mapping");
    Init();

    glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypress);
	glutMainLoop();
	return 0;
}
