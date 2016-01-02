
#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <GL/glut.h>

void DrawScene(float angle)
{


    //Draw objects

    //draw base
    glPushMatrix();
    glColor3f(0.4f, 0.3f, 0.3f);
    glTranslatef(0.0f, 0.0f, 1.0f);
    glScalef(0.95f, 0.05f, 0.95f);
    glutSolidCube(5.0f);
    glPopMatrix();
    glPushMatrix();

    //draw red torus
    glColor3f(1.0f, 0.3f, 0.3f);
    glTranslatef(0.0f, 1.0f, 0.0f);
    glRotatef(angle, 0.0f, 1.0f, 1.0f);
    glPushMatrix();
    glutSolidTorus(0.1, 0.3, 50, 50);
    glPopMatrix();

     //draw yellow torus
    glRotatef(angle + 60, 1.0f, 0.0f, 0.0f);
    glColor3f(1.2f, 0.8f, 0.4f);
    glutSolidTorus(0.05, 0.13, 50, 50);

    glPopMatrix();
    glPopMatrix();
}





#endif	//SCENE_H
