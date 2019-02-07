/**************
Dave Henning
0921760
cis4820
Assignment 1
***************/

/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>

#define BODY_COUNT 100
#define ACCELERATION 100
#define TUBE_COUNT 10

#include "graphics.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];
extern float tubeData[TUBE_COUNT][6];
extern short tubeVisible[TUBE_COUNT];


	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void setOldViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern void addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);


	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
extern int foot;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;
	/* flag indicates use of a fixed viewpoint */
extern int fixedVP;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

	/* allows users to define colours */
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
void unsetUserColour(int);
extern void getUserColour(int, GLfloat *, GLfloat *, GLfloat *, GLfloat *,
    GLfloat *, GLfloat *, GLfloat *, GLfloat *); 

/********* end of extern variable declarations **************/



/********* start of extern tube declarations **************/
//max number of tubes is 10

//int number, float bx, float by, float bz, float ex, float ey, float ez, int colour
//b stands for beginning and e stands for end, when create is called the tube is auto set to showing
extern void createTube(int , float , float , float , float , float , float , int);

//int is the number of tube it is
extern void hideTube(int);

//this function turns all tubes to invisible
extern void initTubeArray();

// int is the number of tube to turn invisible
//extern void hideTube(int);

/********* end of extern tube declarations **************/

extern void getMotion(float* , float*);

//human struct

// i could make these floats and cast them as ints in the gravity that way they will move every 10 iterations in the world after being cast as opposed to moving 1 cube per iteration
typedef struct Coordinate{
   float x;
   float y;
   float z;
}xyz; //pay attention to order its x y z

typedef struct Humans{
   xyz head;
   xyz body;
   xyz feet;
}human;

typedef struct myPosition{
   float x;
   float y;
   float z;
}momentus;

//dedicated memory for humans
human fodder[BODY_COUNT];

int count = ACCELERATION;

float dmoveX = 0.0;
float dmoveY = 0.0;
float dmoveZ = 0.0;

float ratioX = 0.0;
float ratioY = 0.0;
float ratioZ = 0.0;

momentus movement;

 /*time variable to curb the amount of time uodate happens*/
struct timeb oldT;
struct timeb newT;
//for removing rays from the screen
struct timeb shotClock;

//func def for a2 for firing a tube called in mouse
int rayCount = 0;
// struct with sys time and int id
typedef struct RayShot{
   //int id;
   struct timeb shotAt;
}expire;
expire myRays[TUBE_COUNT];

//left mouse button clicked
void fireTube(){

   // start
   float x, y, z;
   float rotX, rotY, rotZ;
   float slopeY, slopeX;
   // end
   float ax = 0;
   float ay = 0;
   float az = 0;
   double radY, radX;

   // vector of line
   float vectx = 0;
   float vecty = 0; 
   float vectz = 0;
   // compare these should all be the same
   float samex = 0;
   float samey = 0; 
   float samez = 0;
   // cube coordinates
   float cubex = 0;
   float cubey = 0;
   float cubez = 0;
   // loop through all body parts
   int rayLoop = 0;
   // one print per body!!
   int once = 1;
   // zero variable
   float zero = 0.0;
   //float ex, ey, ez;

   getViewPosition(&x, &y, &z);
   x *= -1;
   y *= -1;
   z *= -1;
   //printf("beginning :: x %lf y %lf z %lf \n", x, y+0.4, z);

   getViewOrientation(&rotX, &rotY, &rotZ);

   radY = rotY / 180.0 * 3.141592;
   radX = rotX / 180.0 * 3.141592;

   ax -= sin(radY);
   ay += sin(radX);         
   az += cos(radY);

   vectx = ax * 30.0;
   vecty = ay * 30.0;
   vectz = az * 30.0;


   vectx = x - vectx;
   vecty = y - vecty;
   vectz = z - vectz;
   //printf("end :: x %lf y %lf z %lf \n", ax, ay+0.4, az);

   createTube(rayCount, x, y, z, vectx, vecty, vectz, 2);
   // y -= 0.1;
   // ay -= 0.1;

   for(rayLoop = 1; rayLoop <= 600; rayLoop++){
      once = rayLoop / 20;
      cubex = x - (ax * once);
      cubey = y - (ay * once);
      cubez = z - (az * once);

      if(cubex > 99 || cubex < 0 || cubez > 99 || cubez < 0 || cubey > 49 || cubex < 0){
         break;
      }
      
      
      
      if(world[(int)cubex][(int)cubey][(int)cubez] == 3 || world[(int)cubex][(int)cubey][(int)cubez] == 4 || world[(int)cubex][(int)cubey][(int)cubez] == 7){
         printf("shots fired\n");
         break;
      }

   }

   


   




   ftime(&myRays[rayCount].shotAt);
   //createTube(1, x, y, 20, 60, 20, 60, 2);
   rayCount++;
   if(rayCount == TUBE_COUNT){
      rayCount = 0;
   }


   return;
}


	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {

	/* your code for collisions goes here */
   float x, y, z;
   x = 0.0;
   y = 0.0;
   z = 0.0;
   float space = 0.15;

   getViewPosition(&x, &y, &z);

   x = x * -1;
   y = y * -1;
   z = z * -1;
   //printf("y = %f\n", y);

   //check if pov position is inside a cube then bounce out 
   //ground collision detection

   //printf("x = %f, y =  %f, z = %f\n", x, y, z);

   /***** collision detection between all the cubes in the world and viewpoint  *****/
   //                   standard                            x++                                          y++                                                     z++                    
   if( world[ (int)x ][ (int)y ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)z ] != 0 || world[ (int)x ][ (int)(y+space) ][ (int)z ] != 0 || world[ (int)x ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)z ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)x ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)(x+space) ][ (int)(y+space) ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)x ][ (int)(y+space) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y+space) ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y+space) ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)(y+space) ][ (int)z ] != 0 || world[ (int)(x-space) ][ (int)(y+space) ][ (int)(z-space) ] != 0 || world[ (int)x ][ (int)(y+space) ][ (int)(z-space) ] != 0 || world[ (int)(x+space) ][ (int)(y+space) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)(z-space) ] != 0){

      float oldX, oldY, oldZ;
      oldX = 0.0;
      oldX = 0.0;
      oldZ = 0.0;
      count = ACCELERATION;
      getOldViewPosition(&oldX, &oldY, &oldZ);

      setViewPosition(oldX, oldY, oldZ);
      movement.x = oldX;
      movement.y = oldY;
      movement.z = oldZ;
   }


   //to keep within field of play
   //check if x y or z exceed 99 or 49 respectively or if its below 0
   if( x > 100 || x < 0 || z > 100 || z < 0 || y < 0 || y > 49){
      count = ACCELERATION;
      float oldX, oldY, oldZ;
      oldX = 0.0;
      oldX = 0.0;
      oldZ = 0.0;

      getOldViewPosition(&oldX, &oldY, &oldZ);
      

      setViewPosition(oldX, oldY, oldZ);

      movement.x = oldX;
      movement.y = oldY;
      movement.z = oldZ;

   }


   /* your code for collisions goes here */

}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      if (displayMap == 1) {
         GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
         set2Dcolour(green);
         draw2Dline(0, 0, 500, 500, 15);
         draw2Dtriangle(0, 0, 200, 200, 0, 200);

         GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
         set2Dcolour(black);
         draw2Dbox(500, 380, 524, 388);
      }
   } else {
	/* your code goes here */

      //loop for rays
      int rayLoop;
      int rayBX, rayBZ, rayEX, rayEZ;

      //variables
      int i = 0;
      int x = 0;
      int z = 0;
      float py = 0;
      float px = 0;
      float pz = 0;
      int bigMapX = 1;
      int bigMapZ = 1;

      GLfloat red[] = {0.5, 0.0, 0.0, 1};
      GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
      GLfloat white[] = {1, 1, 1, 1};
      GLfloat purple[] = {1.0, 0.0, 1.0, 1.0};
      set2Dcolour(black);

      if (displayMap == 1) {
   //draw character position on the map
         set2Dcolour(white);

         getViewPosition(&px, &py, &pz);
         px *= -2;
         pz *= -2;
         py *= -2;

         //format x, z(y) , x, z(y), x, z(y)
         draw2Dtriangle(screenWidth - (220 - px), screenHeight - (220 - pz), screenWidth - (218 - px), screenHeight - (226 - pz), screenWidth - (222-px), screenHeight - (226-pz));
   //draw rays on the map
         set2Dcolour(purple);
         
         for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
            if(tubeVisible[rayLoop] == 1){
               //printf("tubevisible %d\n", rayLoop);
               rayBX = tubeData[rayLoop][0] * 2;
               rayBZ = tubeData[rayLoop][2] * 2;
               rayEX = tubeData[rayLoop][3] * 2;
               rayEZ = tubeData[rayLoop][5] * 2;
               if( (220 - rayEX) < 20 ){
                  rayEX = 200;
               }
               if( (220 - rayEZ) < 20 ){
                  rayEZ = 200;
               }
               if( (220 - rayEX) > 220 ){
                  rayEX = 0;
               }
               if( (220 - rayEZ) > 220 ){
                  rayEZ = 0;
               }
               draw2Dline(screenWidth - (220 - rayBX), screenHeight - (220 - rayBZ), screenWidth - (220 - rayEX), screenHeight - (220 - rayEZ), 2);


            }
         }      
   
   //draw all the humans on the map
         set2Dcolour(red);
         for(i = 0; i < BODY_COUNT; i++){
            x = fodder[i].head.x * 2;
            z = fodder[i].head.z * 2;
            draw2Dbox(screenWidth - (220 - x), screenHeight - (220 - z), screenWidth - (218 - x), screenHeight - (218 - z));
         }
         
   //draw the map background must be done last as the first thing drawn is drawn ontop
         set2Dcolour(white);
         draw2Dline(screenWidth - 19, screenHeight - 19, screenWidth - 221, screenHeight - 19, 2);
         draw2Dline(screenWidth - 221, screenHeight - 19, screenWidth - 221, screenHeight - 221, 2);
         draw2Dline(screenWidth - 221, screenHeight - 221, screenWidth - 19, screenHeight - 221, 2);
         draw2Dline(screenWidth - 19, screenHeight - 221, screenWidth - 19, screenHeight - 19, 2);

         set2Dcolour(black);
         draw2Dbox(screenWidth - 20, screenHeight - 20, screenWidth - 220, screenHeight - 220);

      }
   //big map
      if (displayMap == 2) {///////////////////////////////////////////////////////////////////////////
         //loop for rays
         int rayLoop;
         int rayBX, rayBZ, rayEX, rayEZ;

   //draw character position on the map
         set2Dcolour(white);
         getViewPosition(&px, &py, &pz);
         px *= -1;
         pz *= -1;
         py *= -1;
         //printf("px = %lf, pz = %lf\n", px, pz);
         if(px >= 49){
            px = (px-49) * 6;
            // bigMapX = 3;
         }
         else{
            px = (49-px) * (-6);
            //bigMapX = -3;
         }
         if(pz >= 49){
            pz = (pz-49)* 6;
            //bigMapZ = 3;
         }
         else{
            pz = (49-pz) * (-6);
            //bigMapZ = -1;
         }
         // //format x, z(y) , x, z(y), x, z(y)
         draw2Dtriangle(screenWidth/2 + (px), screenHeight/2 + (pz), screenWidth/2 + (px+3), screenHeight/2 + (pz-10), screenWidth/2 + (px-3), screenHeight/2 + (pz-10));
   //draw all the rays on the map
         set2Dcolour(purple);
         for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
            if(tubeVisible[rayLoop] == 1){
               //printf("tubevisible %d\n", rayLoop);
               rayBX = tubeData[rayLoop][0];
               rayBZ = tubeData[rayLoop][2];
               rayEX = tubeData[rayLoop][3];
               rayEZ = tubeData[rayLoop][5];
               if(rayBX >= 49){
                  rayBX = (rayBX-49) * 6;
                  // bigMapX = 3;
               }
               else{
                  rayBX = (49-rayBX) * (-6);
                  //bigMapX = -3;
               }
               if(rayBZ >= 49){
                  rayBZ = (rayBZ-49)* 6;
                  //bigMapZ = 3;
               }
               else{
                  rayBZ = (49-rayBZ) * (-6);
                  //bigMapZ = -1;
               }
               if(rayEX >= 49){
                  rayEX = (rayEX-49) * 6;
                  // bigMapX = 3;
               }
               else{
                  rayEX = (49-rayEX) * (-6);
                  //bigMapX = -3;
               }
               if(rayEZ >= 49){
                  rayEZ = (rayEZ-49)* 6;
                  //bigMapZ = 3;
               }
               else{
                  rayEZ = (49-rayEZ) * (-6);
                  //bigMapZ = -1;
               }

               if(rayEX > 300){
                  rayEX = 300;
               }
               if(rayEX < -300){
                  rayEX = -300;
               }
               if(rayEZ > 300){
                  rayEZ = 300;
               }
               if(rayEZ < -300){
                  rayEZ = -300;
               }

               draw2Dline(screenWidth/2 + (rayBX), screenHeight/2 + (rayBZ), screenWidth/2 + (rayEX), screenHeight/2 + (rayEZ), 2);


            }
         }
   //draw all the humans on the map
         set2Dcolour(red);
         for(i = 0; i < BODY_COUNT; i++){
            x = fodder[i].head.x;
            z = fodder[i].head.z;

            if(x >= 49){
               x = (x-49) * 6;
               // bigMapX = 3;
            }
            else{
               x = (49-x) * (-6);
               //bigMapX = -3;
            }
            if(z >= 49){
               z = (z-49)* 6;
               //bigMapZ = 3;
            }
            else{
               z = (49-z) * (-6);
               //bigMapZ = -1;
            }
            draw2Dbox( (screenWidth/2) + x - 3, (screenHeight/2) + z - 3, (screenWidth/2) + x + 3 , (screenHeight/2) + z + 3);
         }


   //base colour and border of map
         set2Dcolour(white);
         draw2Dline(screenWidth/2 - 303, screenHeight/2 + 303, screenWidth/2 + 303, screenHeight/2 + 303, 2);
         draw2Dline(screenWidth/2 + 303, screenHeight/2 - 303, screenWidth/2 + 303, screenHeight/2 + 303, 2);
         draw2Dline(screenWidth/2 - 303, screenHeight/2 - 303, screenWidth/2 - 303, screenHeight/2 + 303, 2);
         draw2Dline(screenWidth/2 - 303, screenHeight/2 - 303, screenWidth/2 + 303, screenHeight/2 - 303, 2);
         set2Dcolour(black);
         draw2Dbox(screenWidth/2 - 303, screenHeight/2 - 303, screenWidth/2 + 303, screenHeight/2 + 303);
         
      }


   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
   int i, j, k, loop;
   float *la;

	/* sample animation for the test world, don't remove this code */
	/* demo of animating mobs */
   if (testWorld) {

      /* sample of rotation and positioning of mob */
      /* coordinates for mob 0 */
         static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
         static float mob0ry = 0.0;
         static int increasingmob0 = 1;
      /* coordinates for mob 1 */
         static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
         static float mob1ry = 0.0;
         static int increasingmob1 = 1;
      /* counter for user defined colour changes */
         static int colourCount = 0;
         static GLfloat offset = 0.0;

      /* move mob 0 and rotate */
      /* set mob 0 position */
         setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

      /* move mob 0 in the x axis */
         if (increasingmob0 == 1)
            mob0x += 0.2;
         else 
            mob0x -= 0.2;
         if (mob0x > 50) increasingmob0 = 0;
         if (mob0x < 30) increasingmob0 = 1;

      /* rotate mob 0 around the y axis */
         mob0ry += 1.0;
         if (mob0ry > 360.0) mob0ry -= 360.0;

      /* move mob 1 and rotate */
         setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

      /* move mob 1 in the z axis */
      /* when mob is moving away it is visible, when moving back it */
      /* is hidden */
         if (increasingmob1 == 1) {
            mob1z += 0.2;
            showMob(1);
         } else {
            mob1z -= 0.2;
            hideMob(1);
         }
         if (mob1z > 72) increasingmob1 = 0;
         if (mob1z < 52) increasingmob1 = 1;

      /* rotate mob 1 around the y axis */
         mob1ry += 1.0;
         if (mob1ry > 360.0) mob1ry -= 360.0;

      /* change user defined colour over time */
         if (colourCount == 1) offset += 0.05;
         else offset -= 0.01;
         if (offset >= 0.5) colourCount = 0;
         if (offset <= 0.0) colourCount = 1;
         setUserColour(9, 0.7, 0.3 + offset, 0.7, 1.0, 0.3, 0.15 + offset, 0.3, 1.0);

      /* end testworld animation */
   } else { 

	/* your code goes here */
   /****** times out rays once they've been on long enough ******/
      int rayLoop = 0;
      for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
         if(tubeVisible[rayLoop] == 1){
            ftime(&shotClock);
            if(myRays[rayLoop].shotAt.time < shotClock.time - 5){
               tubeVisible[rayLoop] = 0;
            }
            
         }      
      }





   /****** Gravity ******/
      ftime(&newT);

      /* newT should be checked here to curb framerate */
      float diff = 0.0;
      diff = newT.millitm - oldT.millitm;
      //printf("%lf\n", diff);
      //diff *= 1000;

      if( diff > 25 || diff < -25){
         ftime(&oldT);
         //oldT.time = newT.time;
         /****** Gravity ******/
         float gravityRate = 0.4;
         for( loop = 0; loop < BODY_COUNT; loop++){
            //x value will piggyback onto loop

            if( world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].feet.y-1  ][ (int)fodder[loop].feet.z  ] == 0  ){ //will fall if enter this loop
               //update whole body in world and in fodder array
               //update head
               //fodder[loop].head.x = loop;
               fodder[loop].head.y = fodder[loop].head.y - gravityRate;
               //fodder[loop].head.z = random;

            //update body
               //fodder[loop].head.x = loop;
               fodder[loop].body.y = fodder[loop].body.y - gravityRate;
               //fodder[loop].head.z = random;

            //update feet
               //fodder[loop].head.x = loop;
               fodder[loop].feet.y = fodder[loop].feet.y - gravityRate;
               //fodder[loop].head.z = random;

            //move humans in the world b\c of gravity
               world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].head.y ][ (int)fodder[loop].feet.z ] = 3;//head red
               world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].body.y ][ (int)fodder[loop].feet.z ] = 4;//body black
               world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].feet.y ][ (int)fodder[loop].feet.z ] = 7;//feet orange
               
               world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].head.y+1 ][ (int)fodder[loop].feet.z ] = 0;//remove old head
            }
            // if( world[ (int)fodder[loop].feet.x ][ (int)fodder[loop].feet.y-1  ][ (int)fodder[loop].feet.z  ] != 0  ){
            //    printf("person %d, head :: x %d y %d z %d\n", loop, (int)fodder[loop].head.x, (int)fodder[loop].head.y, (int)fodder[loop].head.z );
            // }
         }


      
   /****** Momentum ******/
         float x, y, z;
         float oldX, oldY, oldZ;
         float check = 1;
         float dPlus = 0.025;
         float dNeg = -0.025;
         float accX = 0;
         float accY = 0;
         float accZ = 0;
         float maxV = 0.6;
         float minV = -0.6;
         



         getViewPosition(&x, &y, &z);
         getOldViewPosition( &oldX, &oldY, &oldZ);

         if(oldX == 0 && oldY == 0 && oldZ == 0){
            oldX = x;
            oldY = y;
            oldZ = z;
            movement.x = x;
            movement.y = y;
            movement.z = z;
         }
         //printf(" x = %f, y = %f, z = %f\n old x %f, old y %f, old z %f\n\n", x, y, z, oldX, oldY, oldZ);

         if(movement.x != x || movement.y != y || movement.z != z){
            if(x > movement.x){
               dmoveX = dPlus;
               if(count == ACCELERATION){
                  ratioX = x - movement.x;
               }
            } else if(x == movement.x){
               dmoveX = 0;
               ratioX = 1;
            }else {
               dmoveX = dNeg;
               if(count == ACCELERATION){
                  ratioX = movement.x - x;
               }
            }
            //x = x + check;

            if(y > movement.y){
               dmoveY = dPlus;
               if(count == ACCELERATION){
                  ratioY = y - movement.y;
               }
            } else if(y == movement.y){
               dmoveY = 0;
               //ratioY = 1;
            } else {
               dmoveY = dNeg;
               if(count == ACCELERATION){
                  ratioY = movement.y - y;
               }
               
            }
            //y = y + check;

            if(z > movement.z){
               dmoveZ = dPlus;
               if(count == ACCELERATION){
                  ratioZ = z - movement.z;
               }
            } else if(z == movement.z){
               dmoveZ = 0;
               ratioZ = 1;
            } else {
               dmoveZ = dNeg;
               if(count == ACCELERATION){
                  ratioZ = movement.z - z;
               }
            }
            //z = z + check;
            count--;

         }

         if(count != ACCELERATION){
            setOldViewPosition( x, y, z);
            oldX = x;
            oldY = y;
            oldZ = z;

            accX = ((dmoveX * (count/20)) * ratioX);
            accY = ((dmoveY * (count/20)) * ratioY);
            accZ = ((dmoveZ * (count/20)) * ratioZ);

            if( (accX) > maxV ){
               accX = maxV;
            }
            if( (accY) > maxV ){
               accX = maxV;
            }
            if( (accZ) > maxV ){
               accX = maxV;
            }
            if( (accX) < minV ){
               accX = minV;
            }
            if( (accY) < minV ){
               accX = minV;
            }
            if( (accZ) < minV ){
               accX = minV;
            }

            x = x + accX;
            y = y + accY;
            z = z + accZ;
            
            
           
            setViewPosition( x, y, z);
            movement.x = x;
            movement.y = y;
            movement.z = z;
            
            collisionResponse();
            count--;
            if(count == 0){
               count = ACCELERATION;
               movement.x = x;
               movement.y = y;
               movement.z = z;

            }

         }

      }

   /* your code goes here */

   }
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON){
      //printf("left button - \n"); //use this for firing tubes

    }else if (button == GLUT_MIDDLE_BUTTON){
      //printf("middle button - ");
    }else{
      //printf("right button - ");
    }

    if (state == GLUT_UP){
       //printf("fire!!\n");
       fireTube();
      //    printf("up - ");
       
    }else{
      //  if (myFlag == 1){
      //    printf("down - ");
      // }
      
    }

   //printf("%d %d\n", x, y);
}


//initialize world upon game start
int main(int argc, char** argv){
   int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);

	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* create user defined colour and draw cube */
      setUserColour(9, 0.7, 0.3, 0.7, 1.0, 0.3, 0.15, 0.3, 1.0);
      world[54][25][50] = 9;


	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);
   } else if(foot == 1){
	   /* your code to build the world goes here */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 1;
         //side lines
            if(j == (WORLDZ*0.33) || j == 66){
               world[i][24][j] = 5;
            }
         //end zone
         if(i == 0 || i == 99 || i == 10 || i == 90 || i == 49 || i == 9 || i == 89 ){
            if(j >= (WORLDZ*0.33) && j <= 66){
               world[i][24][j] = 5;
            }
               
            }
         }
      }
   } else {

      //initializing my attempt
      FILE *fp;
      fp = fopen("ground.pgm", "r");

      time_t t;
      ftime(&oldT);
      srand((unsigned) time(&t));

      int loop;
      int random;
      int height=0;
      char str[10];
      movement.x = 0;
      movement.y = 0;
      movement.z = 0;

      //create a new colour for ground so its not neon green
      setUserColour(9, 0.29, 0.60, 0.42, 0.7, 0.29, 0.60, 0.42, 0.7);  //ground
      setUserColour(10, 0.96, 0.96, 0.99, 1.0, 0.96, 0.96, 0.99, 1.00); // snow

      

      for(i=0; i<8; i++){
         fscanf(fp, "%s",  str);
         //printf("%s\n", str);
      }
      
      
      for(i=0; i<100; i++){
         for(j=0; j<100; j++){

            fscanf(fp, "%s",  str);
            height = atoi(str);
            height = height / 25;    //early front runner is 25 for choice      
               

            do{
               if( height > 8){
                  world[i][height][j] = 10;//snow
               }
               else if(height < 5){
                  world[i][height][j] = 2;//water
               }
               else{
                  world[i][height][j] = 9;//grass/rock
               }               
               height--;

            }while(height >= 0);
            
         }
      }
      fclose(fp);
      //ground loading complete

      //*************** load humans into the world ***************//

      for(loop = 0; loop < BODY_COUNT; loop++){
         
         //x needs to be looped through will piggyback on 'loop'; all will start at 49, 48, 47; z will be random

         random = rand() % BODY_COUNT;// z value 

      //make head
         fodder[loop].head.x = loop;
         fodder[loop].head.y = 49;
         fodder[loop].head.z = random;

      //make body
         fodder[loop].body.x = loop;
         fodder[loop].body.y = 48;
         fodder[loop].body.z = random;

      //make feet
         fodder[loop].feet.x = loop;
         fodder[loop].feet.y = 47;
         fodder[loop].feet.z = random;

      //place humans in the world at the top of the sky
         world[loop][49][random] = 3;//head red
         world[loop][48][random] = 4;//body black
         world[loop][47][random] = 7;//feet orange

      }



   }//end of world initialization!!!
      


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}



