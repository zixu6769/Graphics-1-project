#include "CSCIx229.h"

// CSCI 4229
// Professor Schreuder
// Final project
// Student name: Zijun Xu
// Used CSCIx229 library(modified)
// Used errcheck.c, fatal.c, loadtexbmp.c, object.c, and print.c functions from ex15

int th=0;            // Azimuth of view angle
int ph=0;            // Elevation of view angle
int mode=1;          //  Projection mode
int fov=65;          //  Field of view (for perspective)
double asp=5;        //  Aspect ratio
double dim=8;        //  Size of world
//  Eye position variables
float eyex = 1.0f;
float eyey = 1.0f;
float eyez = 0.0f;
//  Direction variables
float dirx = 0.0f;
float diry = 0.0f;
float dirz = 0.0f;
//  Angles for rotation
float viewangle = 0.0;
float updown = 0.0;
//  For moving sideway
float crossx = 0;
float crossz = 0;
//  For mouse movement
int last_x = 0;
int last_y = 0;
//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))
// Light values
int distance  =   7;       // Light distance
int inc       =   10;      // Ball increment
int smooth    =   1;       // Smooth/Flat shading
int local     =   0;       // Local Viewer Model
int emission  =   0;       // Emission intensity (%)
int ambient   =   0;       // Ambient intensity (%)
int diffuse   =   90;      // Diffuse intensity (%)
int specular  =   10;      // Specular intensity (%)
int shininess =   5;       // Shininess (power of two)
float shiny   =   10;      // Shininess (value)
int zh        =   90;      // Light azimuth
float ylight  =   0;       // Elevation of light
int moving = 1;            // Move the light

unsigned int texture[26];  // Store the textures
int skydistance = 200;     // size of the skybox
int groundsize = 40;       // size of the ground
int night = 0;             // Daytime/nighttime/torch

static void Project()
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective transformation
    gluPerspective(fov,asp,dim/1000,100*dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 *  The ball is for lighting demostration purpose therefore it is not textured
 */
static void ball(double x,double y,double z,double r)
{
    int th,ph;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  White ball
    glColor3f(1,1,1);
    glMaterialf(GL_FRONT,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    //  Bands of latitude
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}

/*
 *  Draw the sky.
 */
static void skybox(){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    //  Front
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[5]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[15]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(0,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez + skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex - skydistance, eyey + skydistance, eyez + skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex - skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(1,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez - skydistance);
    glEnd();

    //  Top
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[6]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[16]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(0,0);glVertex3f(eyex - skydistance, eyey + skydistance, eyez + skydistance);
    glTexCoord2f(1,0);glVertex3f(eyex - skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez + skydistance);
    glEnd();
    
    //  Back
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[7]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[17]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(1,0);glVertex3f(eyex + skydistance, eyey - skydistance, eyez + skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez + skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(0,0);glVertex3f(eyex + skydistance, eyey - skydistance, eyez - skydistance);
    glEnd();
    
    //  Left
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[9]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[19]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(1,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez + skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex - skydistance, eyey + skydistance, eyez + skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez + skydistance);
    glTexCoord2f(0,0);glVertex3f(eyex + skydistance, eyey - skydistance, eyez + skydistance);
    glEnd();
    
    //  Right
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[8]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[18]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(0,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez - skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex - skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex + skydistance, eyey + skydistance, eyez - skydistance);
    glTexCoord2f(1,0);glVertex3f(eyex + skydistance, eyey - skydistance, eyez - skydistance);
    glEnd();
    
    //  Bottom
    if (night == 0){ glBindTexture(GL_TEXTURE_2D,texture[10]);}
    else{ glBindTexture(GL_TEXTURE_2D,texture[20]);}
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(0,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez + skydistance);
    glTexCoord2f(1,0);glVertex3f(eyex - skydistance, eyey - skydistance, eyez - skydistance);
    glTexCoord2f(1,1);glVertex3f(eyex + skydistance, eyey - skydistance, eyez - skydistance);
    glTexCoord2f(0,1);glVertex3f(eyex + skydistance, eyey - skydistance, eyez + skydistance);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw the ground
 */
static void ground(){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);glVertex3f(groundsize,-0.4,groundsize);
    glTexCoord2f(20,0);glVertex3f(-groundsize,-0.4,groundsize);
    glTexCoord2f(20,20);glVertex3f(-groundsize,-0.4,-groundsize);
    glTexCoord2f(0,20);glVertex3f(groundsize,-0.4,-groundsize);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


/*
 *  Draw a road
 */
static void road(double x,double y,double z,
                 double dx,double dy, double dz,
                 double th){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    //  Top
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(1.3,-0.3,2);
    glTexCoord2f(1,0);glVertex3f(-1.3,-0.3,2);
    glTexCoord2f(1,1);glVertex3f(-1.3,-0.3,-2);
    glTexCoord2f(0,1);glVertex3f(1.3,-0.3,-2);
    //  Bottom
    glColor3f(0.5,0.5,0.5);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(1.3,-0.5,2);
    glTexCoord2f(1,0);glVertex3f(-1.3,-0.5,2);
    glTexCoord2f(1,1);glVertex3f(-1.3,-0.5,-2);
    glTexCoord2f(0,1);glVertex3f(1.3,-0.5,-2);
    //  Front
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0);glVertex3f(1.3,-0.3,2);
    glTexCoord2f(1,0);glVertex3f(1.3,-0.5,2);
    glTexCoord2f(1,1);glVertex3f(-1.3,-0.5,2);
    glTexCoord2f(0,1);glVertex3f(-1.3,-0.3,2);
    //  Back
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(0,0);glVertex3f(1.3,-0.3,-2);
    glTexCoord2f(1,0);glVertex3f(1.3,-0.5,-2);
    glTexCoord2f(1,1);glVertex3f(-1.3,-0.5,-2);
    glTexCoord2f(0,1);glVertex3f(-1.3,-0.3,-2);
    //  Left
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(1.3,-0.3,2);
    glTexCoord2f(1,0);glVertex3f(1.3,-0.5,2);
    glTexCoord2f(1,1);glVertex3f(1.3,-0.5,-2);
    glTexCoord2f(0,1);glVertex3f(1.3,-0.3,-2);
    //  Right
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-1.3,-0.3,2);
    glTexCoord2f(1,0);glVertex3f(-1.3,-0.5,2);
    glTexCoord2f(1,1);glVertex3f(-1.3,-0.5,-2);
    glTexCoord2f(0,1);glVertex3f(-1.3,-0.3,-2);
    //  Double Yellow lines
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(0.1,-0.29,2);
    glTexCoord2f(1,0);glVertex3f(0.05,-0.29,2);
    glTexCoord2f(1,1);glVertex3f(0.05,-0.29,-2);
    glTexCoord2f(0,1);glVertex3f(0.1,-0.29,-2);
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-0.1,-0.29,2);
    glTexCoord2f(1,0);glVertex3f(-0.05,-0.29,2);
    glTexCoord2f(1,1);glVertex3f(-0.05,-0.29,-2);
    glTexCoord2f(0,1);glVertex3f(-0.1,-0.29,-2);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//  Draw a tank
static void tank(double x,double y,double z,
                 double dx,double dy, double dz,
                 double th)
{
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    //  Upper Front
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 1);
    glTexCoord2f(0,0);glVertex3f(-2,0,4);
    glTexCoord2f(1,0);glVertex3f(+2,0,4);
    glTexCoord2f(1,1);glVertex3f(+2,1,3);
    glTexCoord2f(0,1);glVertex3f(-2,1,3);
    //  Lower Front
    glColor3f(1,1,1);
    glNormal3f(0, -1, 1);
    glTexCoord2f(0,0);glVertex3f(-2,0,4);
    glTexCoord2f(1,0);glVertex3f(+2,0,4);
    glTexCoord2f(1,1);glVertex3f(+2,-1,3);
    glTexCoord2f(0,1);glVertex3f(-2,-1,3);
    //  Upper Back
    glColor3f(1,1,1);
    glNormal3f( 0, 1, -1);
    glTexCoord2f(0,0);glVertex3f(-2,0,-4);
    glTexCoord2f(1,0);glVertex3f(+2,0,-4);
    glTexCoord2f(1,1);glVertex3f(+2,1,-3);
    glTexCoord2f(0,1);glVertex3f(-2,1,-3);
    //  Lower Back
    glColor3f(1,1,1);
    glNormal3f( 0, -1, -1);
    glTexCoord2f(0,0);glVertex3f(-2,0,-4);
    glTexCoord2f(1,0);glVertex3f(+2,0,-4);
    glTexCoord2f(1,1);glVertex3f(+2,-1,-3);
    glTexCoord2f(0,1);glVertex3f(-2,-1,-3);
    //  Bottom
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(-2,-1,3);
    glTexCoord2f(1,0);glVertex3f(+2,-1,3);
    glTexCoord2f(1,1);glVertex3f(+2,-1,-3);
    glTexCoord2f(0,1);glVertex3f(-2,-1,-3);
    //  Top
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-2,1,3);
    glTexCoord2f(1,0);glVertex3f(+2,1,3);
    glTexCoord2f(1,1);glVertex3f(+2,1,-3);
    glTexCoord2f(0,1);glVertex3f(-2,1,-3);
    glEnd();
    //  Left
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(2,1,3);
    glTexCoord2f(1,0);glVertex3f(2,0,4);
    glTexCoord2f(1,1);glVertex3f(2,-1,3);
    glTexCoord2f(0,1);glVertex3f(2,-1,-3);
    glTexCoord2f(0,0);glVertex3f(2,-1,-3);
    glTexCoord2f(1,0);glVertex3f(2,0,-4);
    glTexCoord2f(1,1);glVertex3f(2,1,-3);
    glTexCoord2f(0,1);glVertex3f(2,1,3);
    //  Right
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-2,1,3);
    glTexCoord2f(1,0);glVertex3f(-2,0,4);
    glTexCoord2f(1,1);glVertex3f(-2,-1,3);
    glTexCoord2f(0,1);glVertex3f(-2,-1,-3);
    glTexCoord2f(0,0);glVertex3f(-2,-1,-3);
    glTexCoord2f(1,0);glVertex3f(-2,0,-4);
    glTexCoord2f(1,1);glVertex3f(-2,1,-3);
    glTexCoord2f(0,1);glVertex3f(-2,1,3);
    glEnd();
    //  Turret front
    glBindTexture(GL_TEXTURE_2D,texture[4]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 1);
    glTexCoord2f(0,0);glVertex3f(-1.5,1,0.8);
    glTexCoord2f(1,0);glVertex3f(1.5,1,0.8);
    glTexCoord2f(1,1);glVertex3f(0.8,2,-0.2);
    glTexCoord2f(0,1);glVertex3f(-0.8,2,-0.2);
    //  Turret Left
    glColor3f(1,1,1);
    glNormal3f( 1, 1, 0);
    glTexCoord2f(0,0);glVertex3f(1.5,1,0.8);
    glTexCoord2f(1,0);glVertex3f(1.5,1,-2.2);
    glTexCoord2f(1,1);glVertex3f(0.8,2,-1.8);
    glTexCoord2f(0,1);glVertex3f(0.8,2,-0.2);
    //  Turret Right
    glColor3f(1,1,1);
    glNormal3f( -1, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-1.5,1,0.8);
    glTexCoord2f(1,0);glVertex3f(-1.5,1,-2.2);
    glTexCoord2f(1,1);glVertex3f(-0.8,2,-1.8);
    glTexCoord2f(0,1);glVertex3f(-0.8,2,-0.2);
    //  Turret Back
    glColor3f(1,1,1);
    glNormal3f( 0, 1, -1);
    glTexCoord2f(0,0);glVertex3f(1.5,1,-2.2);
    glTexCoord2f(1,0);glVertex3f(0.8,2,-1.8);
    glTexCoord2f(1,1);glVertex3f(-0.8,2,-1.8);
    glTexCoord2f(0,1);glVertex3f(-1.5,1,-2.2);
    //  Turret Top
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(0.8,2,-0.2);
    glTexCoord2f(1,0);glVertex3f(0.8,2,-1.8);
    glTexCoord2f(1,1);glVertex3f(-0.8,2,-1.8);
    glTexCoord2f(0,1);glVertex3f(-0.8,2,-0.2);
    glEnd();
    
    
    //  Left Wheels
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_POLYGON);
    float theta = 0;
    glColor3f(0.5,0.5,0.5);
    glNormal3f( 1, 0, 0);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)+2.5);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)+1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)-1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)-2.5);
    }
    glEnd();
    
    
    // Right wheels
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    glNormal3f( -1, 0, 0);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)+2.5);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)+1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)-1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    theta = 0;
    glColor3f(0.5,0.5,0.5);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-1.9, 0.8*sin(theta)-0.7, 0.8*cos(theta)-2.5);
    }
    glEnd();
    
    //  Gun Tube
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.5,0.5,0.5);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0.15*sin(phi), 0);
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.15*cos(phi), 0.15*sin(phi)+1.5, 0);
        glVertex3f(0.15*cos(phi), 0.15*sin(phi)+1.5, 5); }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/*
 *  Draw a big house
 */
static void bighouse(double x,double y,double z,
                 double dx,double dy, double dz,
                 double th){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    
    // base top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(8,0,10);
    glTexCoord2f(1,0);glVertex3f(-8,0,10);
    glTexCoord2f(1,1);glVertex3f(-8,0,-10);
    glTexCoord2f(0,1);glVertex3f(8,0,-10);
    
    // base base
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(8,-0.5,10);
    glTexCoord2f(1,0);glVertex3f(-8,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(-8,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(8,-0.5,-10);
    
    // base left
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(8,0,10);
    glTexCoord2f(1,0);glVertex3f(8,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(8,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(8,0,-10);
    
    // base right
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-8,0,10);
    glTexCoord2f(1,0);glVertex3f(-8,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(-8,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(-8,0,-10);
    
    // base front
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0);glVertex3f(8,0,10);
    glTexCoord2f(1,0);glVertex3f(-8,0,10);
    glTexCoord2f(1,1);glVertex3f(-8,-0.5,10);
    glTexCoord2f(0,1);glVertex3f(8,-0.5,10);
    
    // base back
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(0,0);glVertex3f(8,0,-10);
    glTexCoord2f(1,0);glVertex3f(-8,0,-10);
    glTexCoord2f(1,1);glVertex3f(-8,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(8,-0.5,-10);
    glEnd();
    
    // Left wall 1
    glBindTexture(GL_TEXTURE_2D,texture[12]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(8,0,5);
    glTexCoord2f(1,1);glVertex3f(8,5,5);
    glTexCoord2f(0,1);glVertex3f(8,6,1);
    glTexCoord2f(0,0);glVertex3f(8,0,1);
    
    // Left wall 2
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(8,0,1);
    glTexCoord2f(1,1);glVertex3f(8,6,1);
    glTexCoord2f(0,1);glVertex3f(8,6,-4);
    glTexCoord2f(0,0);glVertex3f(8,0,-4);
    
    // Left wall 3
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(8,0,-4);
    glTexCoord2f(1,1);glVertex3f(8,6,-4);
    glTexCoord2f(0,1);glVertex3f(8,6,-9);
    glTexCoord2f(0,0);glVertex3f(8,0,-9);
    glEnd();
    
    // Left wall 4 tri
    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(8,6,1);
    glTexCoord2f(0,1);glVertex3f(8,6,-4);
    glTexCoord2f(1,1);glVertex3f(8,8,-2);
    glEnd();
    
    // Right wall 1
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-8,0,5);
    glTexCoord2f(1,1);glVertex3f(-8,5,5);
    glTexCoord2f(0,1);glVertex3f(-8,6,1);
    glTexCoord2f(0,0);glVertex3f(-8,0,1);
    
    // Right wall 2
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-8,0,1);
    glTexCoord2f(1,1);glVertex3f(-8,6,1);
    glTexCoord2f(0,1);glVertex3f(-8,6,-4);
    glTexCoord2f(0,0);glVertex3f(-8,0,-4);
    
    // Right wall 3
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-8,0,-4);
    glTexCoord2f(1,1);glVertex3f(-8,6,-4);
    glTexCoord2f(0,1);glVertex3f(-8,6,-9);
    glTexCoord2f(0,0);glVertex3f(-8,0,-9);
    glEnd();
    
    // Right wall 4 tri
    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-8,6,1);
    glTexCoord2f(0,1);glVertex3f(-8,6,-4);
    glTexCoord2f(1,1);glVertex3f(-8,8,-2);
    glEnd();
    
    // Back side of the house
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(2,0);glVertex3f(8,6,-9);
    glTexCoord2f(2,2);glVertex3f(8,0,-9);
    glTexCoord2f(0,2);glVertex3f(-8,0,-9);
    glTexCoord2f(0,0);glVertex3f(-8,6,-9);
    
    // Front side of the house
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(2,0);glVertex3f(8,5,5);
    glTexCoord2f(2,2);glVertex3f(8,0,5);
    glTexCoord2f(0,2);glVertex3f(-8,0,5);
    glTexCoord2f(0,0);glVertex3f(-8,5,5);
    glEnd();
    
    // Roof 1
    glBindTexture(GL_TEXTURE_2D,texture[13]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0.5);
    glTexCoord2f(2,2);glVertex3f(-9,6,1);
    glTexCoord2f(0,2);glVertex3f(9,6,1);
    glTexCoord2f(0,0);glVertex3f(9,4,9);
    glTexCoord2f(2,0);glVertex3f(-9,4,9);
    
    // Roof 2
    glColor3f(1,1,1);
    glNormal3f( 0, 0.5, 1);
    glTexCoord2f(1,1);glVertex3f(9,8,-2);
    glTexCoord2f(0,1);glVertex3f(-9,8,-2);
    glTexCoord2f(0,0);glVertex3f(-9,6,1);
    glTexCoord2f(1,0);glVertex3f(9,6,1);
    
    // Roof 3
    glColor3f(1,1,1);
    glNormal3f( 0, 0.5, -1);
    glTexCoord2f(1,1);glVertex3f(9,6,-4);
    glTexCoord2f(0,1);glVertex3f(-9,6,-4);
    glTexCoord2f(0,0);glVertex3f(-9,8,-2);
    glTexCoord2f(1,0);glVertex3f(9,8,-2);
    
    // Roof 4
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(1,0);glVertex3f(9,6,-4);
    glTexCoord2f(1,1);glVertex3f(9,6,-10);
    glTexCoord2f(0,1);glVertex3f(-9,6,-10);
    glTexCoord2f(0,0);glVertex3f(-9,6,-4);
    glEnd();
    
    // Roof inside
    glBindTexture(GL_TEXTURE_2D,texture[14]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(5,0);glVertex3f(8,5,5);
    glTexCoord2f(5,5);glVertex3f(-8,5,5);
    glTexCoord2f(0,5);glVertex3f(-8,5,-9);
    glTexCoord2f(0,0);glVertex3f(8,5,-9);
    glEnd();
    
    // Left window front
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(8.2,5,0);
    glTexCoord2f(0,1);glVertex3f(8.2,5,-3);
    glTexCoord2f(0,0);glVertex3f(8.2,2,-3);
    glEnd();
    
    // Left window left
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(1,0);glVertex3f(8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(8.2,5,0);
    glTexCoord2f(0,1);glVertex3f(8,5,0);
    glTexCoord2f(0,0);glVertex3f(8,2,0);
    
    // Left window right
    glNormal3f( 0, 0, -1);
    glTexCoord2f(1,0);glVertex3f(8.2,2,-3);
    glTexCoord2f(1,1);glVertex3f(8.2,5,-3);
    glTexCoord2f(0,1);glVertex3f(8,5,-3);
    glTexCoord2f(0,0);glVertex3f(8,2,-3);
    
    // Left window top
    glNormal3f( 0, 1, 0);
    glTexCoord2f(1,0);glVertex3f(8.2,5,0);
    glTexCoord2f(1,1);glVertex3f(8.2,5,-3);
    glTexCoord2f(0,1);glVertex3f(8,5,-3);
    glTexCoord2f(0,0);glVertex3f(8,5,0);
    
    // Left window bottom
    glNormal3f( 0, -1, 0);
    glTexCoord2f(1,0);glVertex3f(8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(8.2,2,-3);
    glTexCoord2f(0,1);glVertex3f(8,2,-3);
    glTexCoord2f(0,0);glVertex3f(8,2,0);
    glEnd();
    
    // Right window front
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(-8.2,5,0);
    glTexCoord2f(0,1);glVertex3f(-8.2,5,-3);
    glTexCoord2f(0,0);glVertex3f(-8.2,2,-3);
    glDisable(GL_TEXTURE_2D);
    glEnd();
    
    // Right window left
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(1,0);glVertex3f(-8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(-8.2,5,0);
    glTexCoord2f(0,1);glVertex3f(-8,5,0);
    glTexCoord2f(0,0);glVertex3f(-8,2,0);
    
    // Right window right
    glNormal3f( 0, 0, 1);
    glTexCoord2f(1,0);glVertex3f(-8.2,2,-3);
    glTexCoord2f(1,1);glVertex3f(-8.2,5,-3);
    glTexCoord2f(0,1);glVertex3f(-8,5,-3);
    glTexCoord2f(0,0);glVertex3f(-8,2,-3);
    
    // Right window top
    glNormal3f( 0, 1, 0);
    glTexCoord2f(1,0);glVertex3f(-8.2,5,0);
    glTexCoord2f(1,1);glVertex3f(-8.2,5,-3);
    glTexCoord2f(0,1);glVertex3f(-8,5,-3);
    glTexCoord2f(0,0);glVertex3f(-8,5,0);
    
    // Right window bottom
    glNormal3f( 0, -1, 0);
    glTexCoord2f(1,0);glVertex3f(-8.2,2,0);
    glTexCoord2f(1,1);glVertex3f(-8.2,2,-3);
    glTexCoord2f(0,1);glVertex3f(-8,2,-3);
    glTexCoord2f(0,0);glVertex3f(-8,2,0);
    glEnd();
    
    // Door front
    glBindTexture(GL_TEXTURE_2D,texture[22]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(1,0);glVertex3f(-2,0,5.2);
    glTexCoord2f(1,1);glVertex3f(-2,4,5.2);
    glTexCoord2f(0,1);glVertex3f(2,4,5.2);
    glTexCoord2f(0,0);glVertex3f(2,0,5.2);
    glEnd();
    
    // Door left
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-2,0,5.2);
    glTexCoord2f(1,1);glVertex3f(-2,4,5.2);
    glTexCoord2f(0,1);glVertex3f(-2,4,5);
    glTexCoord2f(0,0);glVertex3f(-2,0,5);
                                 
    // Door right
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(2,0,5.2);
    glTexCoord2f(1,1);glVertex3f(2,4,5.2);
    glTexCoord2f(0,1);glVertex3f(2,4,5);
    glTexCoord2f(0,0);glVertex3f(2,0,5);
    
    // Door top
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(1,0);glVertex3f(-2,4,5.2);
    glTexCoord2f(1,1);glVertex3f(2,4,5.2);
    glTexCoord2f(0,1);glVertex3f(2,4,5);
    glTexCoord2f(0,0);glVertex3f(-2,4,5);
    
    // Door bottom
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(1,0);glVertex3f(-2,0,5.2);
    glTexCoord2f(1,1);glVertex3f(2,0,5.2);
    glTexCoord2f(0,1);glVertex3f(2,0,5);
    glTexCoord2f(0,0);glVertex3f(-2,0,5);
    glDisable(GL_TEXTURE_2D);
    glEnd();
    
    //  Pillar 1
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.15*cos(phi)-7, 0, 0.15*sin(phi)+8);
        glVertex3f(0.15*cos(phi)-7, 4.2, 0.15*sin(phi)+8); }
    glEnd();
    
    //  Pillar 2
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.15*cos(phi)-3, 0, 0.15*sin(phi)+8);
        glVertex3f(0.15*cos(phi)-3, 4.2, 0.15*sin(phi)+8); }
    glEnd();
    
    //  Pillar 3
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.15*cos(phi)+3, 0, 0.15*sin(phi)+8);
        glVertex3f(0.15*cos(phi)+3, 4.2, 0.15*sin(phi)+8); }
    glEnd();
    
    //  Pillar 4
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.15*cos(phi)+7, 0, 0.15*sin(phi)+8);
        glVertex3f(0.15*cos(phi)+7, 4.2, 0.15*sin(phi)+8); }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
}

/*
 *  Draw a lamp
 */
static void lamp(double x,double y,double z,
                     double dx,double dy, double dz,
                     double th){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glEnable(GL_TEXTURE_2D);
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.05*cos(phi)+2, -5, 0.05*sin(phi));
        glVertex3f(0.05*cos(phi)+2, -1, 0.05*sin(phi)); }
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0.15*sin(phi), 0);
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.05*cos(phi)+2, -1, 0.05*sin(phi));
        glVertex3f(0.05*cos(phi), 0, 0.05*sin(phi)); }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//  Draw a speed limit sign
static void sign(double x,double y,double z,
                 double dx,double dy, double dz,
                 double th){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.1*cos(phi), 0, 0.1*sin(phi));
        glVertex3f(0.1*cos(phi), 7, 0.1*sin(phi)); }
    glEnd();
    
    float theta = 0;
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5 + 0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-0.1, 0.8*sin(theta) + 6, 0.8*cos(theta));
    }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,texture[23]);
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5 + 0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(-0.15, 0.8*sin(theta) + 6, 0.8*cos(theta));
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void airport(double x,double y,double z,
                     double dx,double dy, double dz,
                     double th){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    
    // base top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,0,10);
    glTexCoord2f(1,0);glVertex3f(-10,0,10);
    glTexCoord2f(1,1);glVertex3f(-10,0,-10);
    glTexCoord2f(0,1);glVertex3f(10,0,-10);
    
    // base base
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(10,-0.5,10);
    glTexCoord2f(1,0);glVertex3f(-10,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(-10,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(10,-0.5,-10);
    
    // base left
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(10,0,10);
    glTexCoord2f(1,0);glVertex3f(10,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(10,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(10,0,-10);
    
    // base right
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-10,0,10);
    glTexCoord2f(1,0);glVertex3f(-10,-0.5,10);
    glTexCoord2f(1,1);glVertex3f(-10,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(-10,0,-10);
    
    // base front
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0);glVertex3f(10,0,10);
    glTexCoord2f(1,0);glVertex3f(-10,0,10);
    glTexCoord2f(1,1);glVertex3f(-10,-0.5,10);
    glTexCoord2f(0,1);glVertex3f(10,-0.5,10);
    
    // base back
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(0,0);glVertex3f(10,0,-10);
    glTexCoord2f(1,0);glVertex3f(-10,0,-10);
    glTexCoord2f(1,1);glVertex3f(-10,-0.5,-10);
    glTexCoord2f(0,1);glVertex3f(10,-0.5,-10);
    
    // Parking line 1
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(4.9,0.01,10);
    glTexCoord2f(1,0);glVertex3f(5.1,0.01,10);
    glTexCoord2f(1,1);glVertex3f(5.1,0.01,-10);
    glTexCoord2f(0,1);glVertex3f(4.9,0.01,-10);
    
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,0.01,6);
    glTexCoord2f(1,0);glVertex3f(10,0.01,6.2);
    glTexCoord2f(1,1);glVertex3f(0,0.01,6.2);
    glTexCoord2f(0,1);glVertex3f(0,0.01,6);
    
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,0.01,2);
    glTexCoord2f(1,0);glVertex3f(10,0.01,2.2);
    glTexCoord2f(1,1);glVertex3f(0,0.01,2.2);
    glTexCoord2f(0,1);glVertex3f(0,0.01,2);
    
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,0.01,-2);
    glTexCoord2f(1,0);glVertex3f(10,0.01,-2.2);
    glTexCoord2f(1,1);glVertex3f(0,0.01,-2.2);
    glTexCoord2f(0,1);glVertex3f(0,0.01,-2);
    
    glColor3f(1,1,0);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,0.01,-6);
    glTexCoord2f(1,0);glVertex3f(10,0.01,-6.2);
    glTexCoord2f(1,1);glVertex3f(0,0.01,-6.2);
    glTexCoord2f(0,1);glVertex3f(0,0.01,-6);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    // Lower half of the tower
    int i;
    int steps = 15;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[24]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(1*cos(phi)-5, 0, 1*sin(phi)-5);
        glVertex3f(1*cos(phi)-5, 7, 1*sin(phi)-5); }
    glEnd();
    
    
    //  Bottom of tower head
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_QUADS);
    glColor3f(0.5,0.5,0.5);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(-4,7,-4);
    glTexCoord2f(1,0);glVertex3f(-6,7,-4);
    glTexCoord2f(1,1);glVertex3f(-6,7,-6);
    glTexCoord2f(0,1);glVertex3f(-4,7,-6);
    glEnd();
    
    //  Top of tower head
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-3,9,-3);
    glTexCoord2f(1,0);glVertex3f(-7,9,-3);
    glTexCoord2f(1,1);glVertex3f(-7,9,-7);
    glTexCoord2f(0,1);glVertex3f(-3,9,-7);
    glEnd();
    
    //  Front of tower head
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 1);
    glTexCoord2f(0,0);glVertex3f(-4,7,-4);
    glTexCoord2f(1,0);glVertex3f(-6,7,-4);
    glTexCoord2f(1,1);glVertex3f(-7,9,-3);
    glTexCoord2f(0,1);glVertex3f(-3,9,-3);
    glEnd();
    
    //  Left of tower head
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( -1, -1, 0);
    glTexCoord2f(0,0);glVertex3f(-6,7,-4);
    glTexCoord2f(1,0);glVertex3f(-7,9,-3);
    glTexCoord2f(1,1);glVertex3f(-7,9,-7);
    glTexCoord2f(0,1);glVertex3f(-6,7,-6);
    glEnd();
    
    //  Right of tower head
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, -1, 0);
    glTexCoord2f(0,0);glVertex3f(-4,7,-4);
    glTexCoord2f(1,0);glVertex3f(-4,7,-6);
    glTexCoord2f(1,1);glVertex3f(-3,9,-7);
    glTexCoord2f(0,1);glVertex3f(-3,9,-3);
    glEnd();
    
    //  Back of tower head
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, -1, -1);
    glTexCoord2f(0,0);glVertex3f(-6,7,-6);
    glTexCoord2f(1,0);glVertex3f(-4,7,-6);
    glTexCoord2f(1,1);glVertex3f(-3,9,-7);
    glTexCoord2f(0,1);glVertex3f(-7,9,-7);
    glEnd();
    
    //  Roof
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-3,10,-3);
    glTexCoord2f(1,0);glVertex3f(-3,10,-7);
    glTexCoord2f(1,1);glVertex3f(-7,10,-7);
    glTexCoord2f(0,1);glVertex3f(-7,10,-3);
    glEnd();
    
    //  Front window
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0);glVertex3f(-7,9,-3);
    glTexCoord2f(1,0);glVertex3f(-3,9,-3);
    glTexCoord2f(1,1);glVertex3f(-3,10,-3);
    glTexCoord2f(0,1);glVertex3f(-7,10,-3);
    glEnd();
    
    //  Back window
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, -1);
    glTexCoord2f(0,0);glVertex3f(-7,9,-7);
    glTexCoord2f(1,0);glVertex3f(-3,9,-7);
    glTexCoord2f(1,1);glVertex3f(-3,10,-7);
    glTexCoord2f(0,1);glVertex3f(-7,10,-7);
    glEnd();
    
    //  Left window
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-7,9,-3);
    glTexCoord2f(1,0);glVertex3f(-7,9,-7);
    glTexCoord2f(1,1);glVertex3f(-7,10,-7);
    glTexCoord2f(0,1);glVertex3f(-7,10,-3);
    glEnd();
    
    //  Right window
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-3,9,-3);
    glTexCoord2f(1,0);glVertex3f(-3,9,-7);
    glTexCoord2f(1,1);glVertex3f(-3,10,-7);
    glTexCoord2f(0,1);glVertex3f(-3,10,-3);
    glEnd();
    
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void torch(double x,double y,double z,
                    double dx,double dy, double dz,
                    double th, double ph){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    glTranslatef(0.004,0.01,0.004);
    glScaled(dx,dy,dz);
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[25]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.5*cos(phi), 0, 0.5*sin(phi));
        glVertex3f(0.5*cos(phi), 6, 0.5*sin(phi)); }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.5*cos(phi), 6, 0.5*sin(phi));
        glVertex3f(1*cos(phi), 8, 1*sin(phi)); }
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(1*cos(phi), 8, 1*sin(phi));
        glVertex3f(1*cos(phi), 10, 1*sin(phi)); }
    glEnd();
    
    glBegin(GL_POLYGON);
    float theta = 0;
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    for(int i = 0; i<360; i++){
        theta=i*3.142/180;
        glTexCoord2f(cos(theta)*0.5+0.5,sin(theta)*0.5 + 0.5);
        glVertex3f(0.5*sin(theta), 0, 0.5*cos(theta));
    }
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void plane(double x,double y,double z,
                  double dx,double dy, double dz,
                  double th, double ph){
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    glScaled(dx,dy,dz);
    
    // Body top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-2,3,0);
    glTexCoord2f(1,0);glVertex3f(2,3,0);
    glTexCoord2f(1,1);glVertex3f(2,3,9);
    glTexCoord2f(0,1);glVertex3f(-2,3,9);
    
    // Body bottom
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 0);
    glTexCoord2f(0,0);glVertex3f(-2,0,0);
    glTexCoord2f(1,0);glVertex3f(2,0,0);
    glTexCoord2f(1,1);glVertex3f(2,0,9);
    glTexCoord2f(0,1);glVertex3f(-2,0,9);
    
    //  Body Left
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(-2,3,9);
    glTexCoord2f(1,0);glVertex3f(-2,0,9);
    glTexCoord2f(1,1);glVertex3f(-2,0,0);
    glTexCoord2f(0,1);glVertex3f(-2,3,0);
    
    //  Body Right
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(2,3,9);
    glTexCoord2f(1,0);glVertex3f(2,0,9);
    glTexCoord2f(1,1);glVertex3f(2,0,0);
    glTexCoord2f(0,1);glVertex3f(2,3,0);
    glEnd();
    
    //  Head top
    glBindTexture(GL_TEXTURE_2D,texture[21]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 1);
    glTexCoord2f(0,0);glVertex3f(-2,3,9);
    glTexCoord2f(1,0);glVertex3f(2,3,9);
    glTexCoord2f(1,1);glVertex3f(2,1.5,11);
    glTexCoord2f(0,1);glVertex3f(-2,1.5,11);
    glEnd();
    
    //  Head bottom
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, -1, 1);
    glTexCoord2f(0,0);glVertex3f(2,1.5,11);
    glTexCoord2f(1,0);glVertex3f(-2,1.5,11);
    glTexCoord2f(1,1);glVertex3f(-2,0,9);
    glTexCoord2f(0,1);glVertex3f(2,0,9);
    glEnd();
    
    //  Head left
    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-2,3,9);
    glTexCoord2f(0,0);glVertex3f(-2,0,9);
    glTexCoord2f(1,1);glVertex3f(-2,1.5,11);
    
    //  Head Right
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(2,3,9);
    glTexCoord2f(0,0);glVertex3f(2,0,9);
    glTexCoord2f(1,1);glVertex3f(2,1.5,11);
    
    //  Butt Left
    glColor3f(1,1,1);
    glNormal3f( -1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(-2,3,0);
    glTexCoord2f(0,0);glVertex3f(-2,3,-3);
    glTexCoord2f(1,1);glVertex3f(-2,0,0);
    
    //  Butt Right
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(1,0);glVertex3f(2,3,0);
    glTexCoord2f(0,0);glVertex3f(2,3,-3);
    glTexCoord2f(1,1);glVertex3f(2,0,0);
    glEnd();
    
    //  Butt top
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(-2,3,0);
    glTexCoord2f(1,0);glVertex3f(2,3,0);
    glTexCoord2f(1,1);glVertex3f(2,3,-3);
    glTexCoord2f(0,1);glVertex3f(-2,3,-3);
    
    //  Butt bottom
    glColor3f(1,1,1);
    glNormal3f( 0, -1, -1);
    glTexCoord2f(0,0);glVertex3f(-2,3,-3);
    glTexCoord2f(1,0);glVertex3f(2,3,-3);
    glTexCoord2f(1,1);glVertex3f(2,0,0);
    glTexCoord2f(0,1);glVertex3f(-2,0,0);
    
    //  tail wing
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(0,2,-8);
    glTexCoord2f(1,0);glVertex3f(0,2,-10);
    glTexCoord2f(1,1);glVertex3f(0,5,-10);
    glTexCoord2f(0,1);glVertex3f(0,5,-9);
    glEnd();
    
    // Vertical tail propeller
    glBindTexture(GL_TEXTURE_2D,texture[25]);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(1,6,-9.3);
    glTexCoord2f(1,0);glVertex3f(1,6,-9.7);
    glTexCoord2f(1,1);glVertex3f(1,2,-9.7);
    glTexCoord2f(0,1);glVertex3f(1,2,-9.3);
    
    // Horizon tail propeller
    glColor3f(1,1,1);
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0);glVertex3f(1,3.8,-7.5);
    glTexCoord2f(1,0);glVertex3f(1,3.8,-11.5);
    glTexCoord2f(1,1);glVertex3f(1,4.2,-11.5);
    glTexCoord2f(0,1);glVertex3f(1,4.2,-7.5);
    
    // Top tail propeller 1
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(10,5,3.8);
    glTexCoord2f(1,0);glVertex3f(10,5,4.2);
    glTexCoord2f(1,1);glVertex3f(-10,5,4.2);
    glTexCoord2f(0,1);glVertex3f(-10,5,3.8);
    
    // Top tail propeller 2
    glColor3f(1,1,1);
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0);glVertex3f(0.2,5,-6);
    glTexCoord2f(1,0);glVertex3f(0.2,5,14);
    glTexCoord2f(1,1);glVertex3f(-0.2,5,14);
    glTexCoord2f(0,1);glVertex3f(-0.2,5,-6);
    glEnd();
    
    // Tail
    int i;
    int steps = 32;
    float  phi, dphi = 2.*M_PI / (float)(steps);
    glBindTexture(GL_TEXTURE_2D,texture[25]);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.8*cos(phi), 0.8*sin(phi)+2,-1);
        glVertex3f(0.3*cos(phi), 0.3*sin(phi)+2,-10); }
    glEnd();
    
    // Tail propeller base
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0,0.2*cos(phi)+4, 0.2*sin(phi)-9.5);
        glVertex3f(1,0.2*cos(phi)+4, 0.2*sin(phi)-9.5); }
    glEnd();
    
    // Top propeller base
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.5*cos(phi),0, 0.5*sin(phi)+4);
        glVertex3f(0.2*cos(phi),5, 0.2*sin(phi)+4); }
    glEnd();
    
    // Right landing frame 1
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)+1.5,0, 0.2*sin(phi)+2);
        glVertex3f(0.2*cos(phi)+1.5,-1, 0.2*sin(phi)+2); }
    glEnd();
    
    // Right landing frame 2
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)+1.5,0, 0.2*sin(phi)+7);
        glVertex3f(0.2*cos(phi)+1.5,-1, 0.2*sin(phi)+7); }
    glEnd();
    
    // Right landing frame 3
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)+1.5,0.2*sin(phi)-1,0);
        glVertex3f(0.2*cos(phi)+1.5,0.2*sin(phi)-1,9); }
    glEnd();
    
    // Left landing frame 1
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)-1.5,0, 0.2*sin(phi)+2);
        glVertex3f(0.2*cos(phi)-1.5,-1, 0.2*sin(phi)+2); }
    glEnd();
    
    // Left landing frame 2
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)-1.5,0, 0.2*sin(phi)+7);
        glVertex3f(0.2*cos(phi)-1.5,-1, 0.2*sin(phi)+7); }
    glEnd();
    
    // Left landing frame 3
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);
    for(i = 0, phi = 0.; i <= steps; i ++, phi += dphi) {
        glNormal3f(0.15*cos(phi), 0, 0.15*sin(phi));
        glTexCoord2f(cos(i)*0.5+0.5,sin(i)*0.5 + 0.5);
        glVertex3f(0.2*cos(phi)-1.5,0.2*sin(phi)-1,0);
        glVertex3f(0.2*cos(phi)-1.5,0.2*sin(phi)-1,9); }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


/**********************************************************************************************/
void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    //  First person view
    if (mode == 0)
    {
        gluLookAt(eyex,eyey,eyez,
                  eyex+dirx,eyey+diry,eyez+dirz,
                  0.0f,1,0.0f);
    }
    //  Perspective
    else
    {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
    }
    //  Translate intensity to color vectors
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    //  Position for the sun
    float Position[]  = {10*distance*Cos(zh),10*distance*Sin(zh),ylight,1.0};
    //  Position for the lamps
    float Position1[] = {3,4,0,1.0};
    float Position2[] = {-3,4,0,1.0};
    float Position3[] = {3,4,30,1.0};
    float Position4[] = {-3,4,30,1.0};
    float Position5[] = {3,4,-30,1.0};
    float Position6[] = {-3,4,-30,1.0};
    float Position7[] = {eyex,eyey,eyez,1.0};
    
    float dirvector1[] = {0,-1,0};           //  Direction of the lamp light
    float dirvector2[] = {dirx,diry,dirz};   //  Direction of the torch light

    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Daytime, the sun is the only light source
    if (night == 0){
        glColor3f(0,1,0);
        ball(Position[0],Position[1],Position[2] , 1);
        ambient = 30;
        // Turn off the lamps
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
        glDisable(GL_LIGHT4);
        glDisable(GL_LIGHT5);
        glDisable(GL_LIGHT6);
        glDisable(GL_LIGHT7);
        // Turn on the sunlight
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    //  Nighttime, only the lamp lights
    else if (night == 1){
        ambient = 1;
        // Turn off the sunlight
        glDisable(GL_LIGHT0);
        // Turn off the torch
        glDisable(GL_LIGHT7);
        // Turn on the lamps
        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT1,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT1,GL_POSITION,Position1);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,1);
        
        glEnable(GL_LIGHT2);
        glLightfv(GL_LIGHT2,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT2,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT2,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT2,GL_POSITION,Position2);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT2, GL_SPOT_EXPONENT,1);
        
        glEnable(GL_LIGHT3);
        glLightfv(GL_LIGHT3,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT3,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT3,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT3,GL_POSITION,Position3);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT3, GL_SPOT_EXPONENT,1);
        
        glEnable(GL_LIGHT4);
        glLightfv(GL_LIGHT4,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT4,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT4,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT4,GL_POSITION,Position4);
        glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT4, GL_SPOT_EXPONENT,1);
        
        glEnable(GL_LIGHT5);
        glLightfv(GL_LIGHT5,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT5,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT5,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT5,GL_POSITION,Position5);
        glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT5, GL_SPOT_EXPONENT,1);
        
        glEnable(GL_LIGHT6);
        glLightfv(GL_LIGHT6,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT6,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT6,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT6,GL_POSITION,Position6);
        glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 50.0);
        glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dirvector1);
        glLightf(GL_LIGHT6, GL_SPOT_EXPONENT,1);
    }
    //  Nighttime, only torch light.
    else {
        //  Turn off the sunlight and the lamps
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
        glDisable(GL_LIGHT4);
        glDisable(GL_LIGHT5);
        glDisable(GL_LIGHT6);
        
        //  Turn on the torch
        glEnable(GL_LIGHT7);
        glLightfv(GL_LIGHT7,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT7,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT7,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT7,GL_POSITION,Position7);
        glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, 15);
        glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, dirvector2);
        glLightf(GL_LIGHT7, GL_SPOT_EXPONENT,10);
        torch(eyex,eyey,eyez,0.001,0.001,0.001,viewangle,-updown/2+90);
    }
    // Draw objects under light
    lamp(Position1[0], Position1[1], Position1[2], 1,1,1, 0);
    lamp(Position2[0], Position2[1], Position2[2], 1,1,1, 180);
    lamp(Position3[0], Position3[1], Position3[2], 1,1,1, 0);
    lamp(Position4[0], Position4[1], Position4[2], 1,1,1, 180);
    lamp(Position5[0], Position5[1], Position5[2], 1,1,1, 0);
    lamp(Position6[0], Position6[1], Position6[2], 1,1,1, 180);
    bighouse(20,-0.2,-15, 0.4,0.4,0.4, 90);
    bighouse(20,-0.2,-5, 0.4,0.4,0.4, 90);
    bighouse(20,-0.2,5, 0.4,0.4,0.4, 90);
    bighouse(20,-0.2,15, 0.4,0.4,0.4, 90);
    bighouse(9,-0.2,-15, 0.4,0.4,0.4, 270);
    bighouse(9,-0.2,-5, 0.4,0.4,0.4, 270);
    bighouse(9,-0.2, 5, 0.4,0.4,0.4, 270);
    bighouse(9,-0.2, 15, 0.4,0.4,0.4, 270);
    bighouse(-9,-0.2,-5, 0.4,0.4,0.4, 90);
    bighouse(-9,-0.2,-15, 0.4,0.4,0.4, 90);
    road(0,0.3,0, 3,2,2, 0);
    road(0,0.3,8, 3,2,2, 0);
    road(0,0.3,16, 3,2,2, 0);
    road(0,0.3,24, 3,2,2, 0);
    road(0,0.3,32, 3,2,2, 0);
    road(0,0.3,-8, 3,2,2, 0);
    road(0,0.3,-16, 3,2,2, 0);
    road(0,0.3,-24, 3,2,2, 0);
    road(0,0.3,-32, 3,2,2, 0);
    road(30,0.3,0, 3,2,2, 0);
    road(30,0.3,8, 3,2,2, 0);
    road(30,0.3,16, 3,2,2, 0);
    road(30,0.3,24, 3,2,2, 0);
    road(30,0.3,32, 3,2,2, 0);
    road(30,0.3,-8, 3,2,2, 0);
    road(30,0.3,-16, 3,2,2, 0);
    road(30,0.3,-24, 3,2,2, 0);
    road(30,0.3,-32, 3,2,2, 0);
    tank(-2,0.6,0, 0.5,0.5,0.5, 0);
    tank(-12,0.6,12, 0.5,0.5,0.5, 90);
    tank(-12,0.6,16, 0.5,0.5,0.5, 90);
    tank(-18,0.6,12, 0.5,0.5,0.5, 270);
    tank(-18,0.6,16, 0.5,0.5,0.5, 270);
    sign(5,-1,20,0.5,0.5,0.5,90);
    sign(-5,-1,5,0.5,0.5,0.5,270);
    airport(-20,-0.2,20,1,1,1,0);
    plane(-26,0.4,26,0.4,0.4,0.4,90,0);
    plane(-20,10,-5,0.4,0.4,0.4,0,25);
    ground();
    // No more lighting here
    glDisable(GL_LIGHTING);
    skybox();
    ball(Position1[0], Position1[1], Position1[2], 0.1);
    ball(Position2[0], Position2[1], Position2[2], 0.1);
    ball(Position3[0], Position3[1], Position3[2], 0.1);
    ball(Position4[0], Position4[1], Position4[2], 0.1);
    ball(Position5[0], Position5[1], Position5[2], 0.1);
    ball(Position6[0], Position6[1], Position6[2], 0.1);
    glFlush();
    glutSwapBuffers();
}

void idle()
{
    if (moving){
        //  Elapsed time in seconds
        double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
        zh = fmod(90*t,360.0);
        //  Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
    }
}

/*
 *
 *  Movement
 *
 */
//  Capture mouse input
void mouse(int x, int y) {
    int dx = x - last_x;
    int dy = y - last_y;
    viewangle -= 1*dx;
    updown -= 1*dy;
    if(updown > 89)
    updown = 89;
    else if (updown < -89)
    updown = -89;
    dirx = Sin(viewangle);
    diry = Sin(updown);
    dirz = Cos(viewangle);
    last_x = x;
    last_y = y;
    Project();
    glutPostRedisplay();
}

void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27) {exit(0);}
    //  Reset view angle
    else if (ch == '0') {
        th = ph = 0;
        eyex = eyey = eyez = 1.0f;
        dirx = diry = dirz = 0.0f;
        viewangle = updown = 0.0;
    }
    //  Switch view mode
    else if (ch == 'm' || ch == 'M'){
        mode = 1 - mode;
    }
    //  Move forward
    else if (ch == 'w' || ch == 'W'){
        eyex += dirx;
        eyez += dirz;
    }
    //  Move backward
    else if (ch == 's' || ch == 'S'){
        eyex -= dirx;
        eyez -= dirz;
    }
    //  Move to the right
    else if (ch == 'd' || ch == 'D'){
        crossx = -dirz;
        crossz = dirx;
        eyex += crossx;
        eyez += crossz;
    }
    //  Move to the left
    else if (ch == 'a' || ch == 'A'){
        crossx = -dirz;
        crossz = dirx;
        eyex -= crossx;
        eyez -= crossz;
    }
    //  Move camera upward
    else if (ch == '1'){eyey -= 0.5;}
    //  Move camera downward
    else if (ch == '2'){eyey += 0.5;}
    //  Pause the animation
    else if (ch == 32){moving = 1 - moving;}
    //  Shift day/night light mode
    else if (ch == 'l' || ch == 'L'){
        if (night < 2){night += 1;}
        else{night = 0;}
    }
    Project();
    glutIdleFunc(idle);
    glutPostRedisplay();
}

void special(int key,int x,int y)
{
    //  Right arrow key - increase azimuth by 5 degrees
    if (key == GLUT_KEY_RIGHT)
    th += 5;
    //  Left arrow key - decrease azimuth by 5 degrees
    else if (key == GLUT_KEY_LEFT)
    th -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
    ph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
    ph -= 5;
    //  Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    //  Tell GLUT it is necessary to redisplay the scene
    Project();
    glutPostRedisplay();
}

void reshape(int width,int height)
{
    //  Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Set projection
    Project();
}

int main(int argc, char *argv[])
{
    glutInit(&argc,argv);
    glutInitWindowSize(800,800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("Project_Zijun_Xu");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutPassiveMotionFunc(mouse);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);
    //  Load texture images
    texture[0] = LoadTexBMP("green.bmp");
    texture[1] = LoadTexBMP("stone1.bmp");
    texture[2] = LoadTexBMP("Dcamouflage.bmp");
    texture[3] = LoadTexBMP("metaljv2.bmp");
    texture[4] = LoadTexBMP("brownhair.bmp");
    texture[5] = LoadTexBMP("TropicalSunnyDayFront.bmp");
    texture[6] = LoadTexBMP("TropicalSunnyDayUp.bmp");
    texture[7] = LoadTexBMP("TropicalSunnyDayBack.bmp");
    texture[8] = LoadTexBMP("TropicalSunnyDayLeft.bmp");
    texture[9] = LoadTexBMP("TropicalSunnyDayRight.bmp");
    texture[10] = LoadTexBMP("TropicalSunnyDayDown.bmp");
    texture[11] = LoadTexBMP("grass.bmp");
    texture[12] = LoadTexBMP("brick.bmp");
    texture[13] = LoadTexBMP("roof.bmp");
    texture[14] = LoadTexBMP("insideroof.bmp");
    texture[15] = LoadTexBMP("FullMoonFront.bmp");
    texture[16] = LoadTexBMP("FullMoonUp.bmp");
    texture[17] = LoadTexBMP("FullMoonBack.bmp");
    texture[18] = LoadTexBMP("FullMoonLeft.bmp");
    texture[19] = LoadTexBMP("FullMoonRight.bmp");
    texture[20] = LoadTexBMP("FullMoonDown.bmp");
    texture[21] = LoadTexBMP("window.bmp");
    texture[22] = LoadTexBMP("door.bmp");
    texture[23] = LoadTexBMP("sign.bmp");
    texture[24] = LoadTexBMP("wall.bmp");
    texture[25] = LoadTexBMP("dark.bmp");
    glutMainLoop();
    return 0;
}
