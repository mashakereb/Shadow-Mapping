#include <stdio.h>
#include <GL/glut.h>
#include "Maths/Maths.h"
#include "scene.h"

void DrawScene(float angle)
{


	//Draw objects

    //draw base

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 0.05f, 1.0f);
    glutSolidCube(4.0f);

    glPopMatrix();

    glColor3f(1.0f, 0.3f, 0.0f);
    glPushMatrix();

    glTranslatef(0.0f, 1.0f, 0.0f);
    glRotatef(angle, 0.0f, 1.0f, 1.0f);
   //
    glPushMatrix();

    glutSolidTorus(0.1, 0.3, 50, 50);

    glPopMatrix();

   glRotatef(angle + 60, 1.0f, 0.0f, 0.0f);
   glColor3f(1.0f, 0.0f, 1.0f);

   glutSolidTorus(0.05, 0.13, 50, 50);

   glPopMatrix();

    glPopMatrix();
}



