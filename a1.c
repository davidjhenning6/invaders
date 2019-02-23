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

#define BODY_COUNT 10
#define ACCELERATION 100
#define TUBE_COUNT 10
#define ALIEN_COUNT 5
#define SONAR 10

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
extern float screenWidth, screenHeight;
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

//for each alien there is a global array of these 
typedef struct Alien{
   xyz base; // this will represent the base of the aliens stem
   xyz target; // the coordinates of the human head so should end up one above the target?? could make the coordinates the target for one above the human head
   int targetID;// this variable holds onto the human's place in the fodder array this will make it much less computationally intensive //not used to check shared targets just used to save time
   int visible; // this should be default 1 meaning the alien is default visible
   float vx;
   float vz;
   float vy;
   int state; // has 4 states 0 -> 3    0 is searching, 1 is moving to human, 2 is carry human straight up, 3 rise back to searching height
   int searchingHeight; // each alien will have a randomized height between a certain range
}alien;

typedef struct Humans{
   xyz head;
   xyz body;
   xyz feet;
   int visible; //this should be default 1 meaning the human is default visible
}human;

typedef struct myPosition{
   float x;
   float y;
   float z;
}momentus;

//dedicated memory for humans
human fodder[BODY_COUNT];

//dedicated memory for landers
alien lander[ALIEN_COUNT]; 

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
struct timeb oldAT;
struct timeb newAT;
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

void drawLander(int, int, int);
void removeLander(int, int, int);
void drawHuman(int x, int y, int z);
void removeHuman(int x, int y, int z);
void alienMovement();
void humanGravity();
void viewMomentum();
void collisionResponse();
void removeRayLoop();



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
   //for moving past a person in ray collision
   int prevX, prevY, prevZ;
   int curX, curY, curZ;

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
   prevX = -1;
   prevY = -1;
   prevZ = -1;

   for(rayLoop = 1; rayLoop <= 300; rayLoop++){
      once = rayLoop / 10;
      cubex = x - (ax * once);
      cubey = y - (ay * once);
      cubez = z - (az * once);
      curX = (int)cubex;
      curY = (int)cubey;
      curZ = (int)cubez;

      if(cubex > 99 || cubex < 0 || cubez > 99 || cubez < 0 || cubey > 49 || cubex < 0){
         break;
      }
      
      
      //printf("%d %d %d       %d %d %d \n", prevX, curX, prevY, curY, prevZ, curZ);
      if( (prevX != curX) || (prevY != curY) || (prevZ != curZ) ){
         if( prevX == curX && prevZ == curZ && curY != prevY+1 && curY != prevY-1 ){
            //do nothing
         }
         else{
            if(world[(int)cubex][(int)cubey][(int)cubez] == 3 || world[(int)cubex][(int)cubey][(int)cubez] == 4 || world[(int)cubex][(int)cubey][(int)cubez] == 7){
               printf("Human shot by Ray!\n\n\n");
               prevX = (int)cubex;
               prevY = (int)cubey;
               prevZ = (int)cubez;

      //if human is shot i now have to erase it and have the lander carrying it return to searching
      //have to check which human has been shot!!
               int humanLoop;
               int tempX, tempZ;
               for(humanLoop=0; humanLoop < BODY_COUNT; humanLoop++){
                  //printf("hit::%f %f      stock::%f %f \n", cubex, cubez, fodder[humanLoop].head.x, fodder[humanLoop].head.z);
                  tempX = (int)fodder[humanLoop].head.x;
                  tempZ = (int)fodder[humanLoop].head.z;
                  if( tempX == (int)cubex && tempZ == (int)cubez ){
                     //printf("target located\n");
                     removeHuman(fodder[humanLoop].head.x, fodder[humanLoop].head.y, fodder[humanLoop].head.z);
                     fodder[humanLoop].visible = 0;
                     break;
                  }
               }
               //use humanLoop and match it to a landers targetID
               int landerLoop;
               for(landerLoop=1; landerLoop < ALIEN_COUNT; landerLoop++){
                  if(humanLoop == lander[landerLoop].targetID){
                     //printf("target hit\n");
                     lander[landerLoop].state = 3;
                  //reset the target to 0 so other aliens don't mistake the currently picked one with one dropped
                  //redundent here since it doesn't exist anymore but will be needed when the alien is the thing shot
                     lander[landerLoop].target.x = 0;
                     lander[landerLoop].target.y = 0;
                     lander[landerLoop].target.z = 0;
                     break;
                  }
               }
            }
         }  
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

/**
 * draws the lander alien the coordinates it recieves are the base of the stem of the lander
 * the coordinates must be within the range:: for x & z from 2 -> 97, for y from 1 -> 45
 * the values scanned in are checked at the beginning 
*/
void drawLander(int x, int y, int z){

   //treat 50 25 50 as x,y,z recieved in function

      //checks to make sure the game won't draw alien outside of world and segfault
      if(y+4 > 49){
         y = 49 - 4;
      }else if( y-1< 0){
         y = 1;
      }
      if(x + 2 > 99 ){
         x = 97;
      }else if(x - 2 < 0 ){
         x = 2;
      }
      if(z + 2 > 99 ){
         z = 97;
      }else if(z - 2 < 0 ){
         z = 2;
      }

      int alienX = x;
      int alienY = y;
      int alienZ = z;

      //stem of alien starting at base
      world[alienX][alienY][alienZ] = 1;
      world[alienX][alienY+1][alienZ] = 1;
      world[alienX][alienY+2][alienZ] = 1;
      world[alienX][alienY+3][alienZ] = 1;
      world[alienX][alienY+4][alienZ] = 1;

      //4 legs of alien
      world[alienX-1][alienY][alienZ] = 1;
      world[alienX-2][alienY-1][alienZ] = 1;

      world[alienX+1][alienY][alienZ] = 1;
      world[alienX+2][alienY-1][alienZ] = 1;

      world[alienX][alienY][alienZ-1] = 1;
      world[alienX][alienY-1][alienZ-2] = 1;

      world[alienX][alienY][alienZ+1] = 1;
      world[alienX][alienY-1][alienZ+2] = 1;

      //head of alien
      //layer 1
      world[alienX+1][alienY+2][alienZ] = 1;
      world[alienX-1][alienY+2][alienZ] = 1;
      world[alienX][alienY+2][alienZ+1] = 1;
      world[alienX][alienY+2][alienZ-1] = 1;
      //layer 2
      world[alienX+1][alienY+3][alienZ] = 1;
      world[alienX-1][alienY+3][alienZ] = 1;
      world[alienX][alienY+3][alienZ+1] = 1;
      world[alienX][alienY+3][alienZ-1] = 1;
      world[alienX+2][alienY+3][alienZ] = 1;
      world[alienX-2][alienY+3][alienZ] = 1;
      world[alienX][alienY+3][alienZ+2] = 1;
      world[alienX][alienY+3][alienZ-2] = 1;
      //eyes
      world[alienX+1][alienY+3][alienZ-1] = 12;
      world[alienX+1][alienY+3][alienZ+1] = 12;
      world[alienX-1][alienY+3][alienZ+1] = 12;
      world[alienX-1][alienY+3][alienZ-1] = 12;

      //layer 3
      world[alienX+1][alienY+4][alienZ] = 1;
      world[alienX-1][alienY+4][alienZ] = 1;
      world[alienX][alienY+4][alienZ+1] = 1;
      world[alienX][alienY+4][alienZ-1] = 1;

   return;
}

void removeLander(int x, int y, int z){
   int alienX = x;
      int alienY = y;
      int alienZ = z;

      //stem of alien starting at base
      world[alienX][alienY][alienZ] = 0;
      world[alienX][alienY+1][alienZ] = 0;
      world[alienX][alienY+2][alienZ] = 0;
      world[alienX][alienY+3][alienZ] = 0;
      world[alienX][alienY+4][alienZ] = 0;

      //4 legs of alien
      world[alienX-1][alienY][alienZ] = 0;
      world[alienX-2][alienY-1][alienZ] = 0;

      world[alienX+1][alienY][alienZ] = 0;
      world[alienX+2][alienY-1][alienZ] = 0;

      world[alienX][alienY][alienZ-1] = 0;
      world[alienX][alienY-1][alienZ-2] = 0;

      world[alienX][alienY][alienZ+1] = 0;
      world[alienX][alienY-1][alienZ+2] = 0;

      //head of alien
      //layer 1
      world[alienX+1][alienY+2][alienZ] = 0;
      world[alienX-1][alienY+2][alienZ] = 0;
      world[alienX][alienY+2][alienZ+1] = 0;
      world[alienX][alienY+2][alienZ-1] = 0;
      //layer 2
      world[alienX+1][alienY+3][alienZ] = 0;
      world[alienX-1][alienY+3][alienZ] = 0;
      world[alienX][alienY+3][alienZ+1] = 0;
      world[alienX][alienY+3][alienZ-1] = 0;
      world[alienX+2][alienY+3][alienZ] = 0;
      world[alienX-2][alienY+3][alienZ] = 0;
      world[alienX][alienY+3][alienZ+2] = 0;
      world[alienX][alienY+3][alienZ-2] = 0;
      //eyes
      world[alienX+1][alienY+3][alienZ-1] = 0;
      world[alienX+1][alienY+3][alienZ+1] = 0;
      world[alienX-1][alienY+3][alienZ+1] = 0;
      world[alienX-1][alienY+3][alienZ-1] = 0;

      //layer 3
      world[alienX+1][alienY+4][alienZ] = 0;
      world[alienX-1][alienY+4][alienZ] = 0;
      world[alienX][alienY+4][alienZ+1] = 0;
      world[alienX][alienY+4][alienZ-1] = 0;

      return;

}

/**
 * draws/removes the humans given the coordinates of the "head"
 * call remove before draw or it won't work
 * will not update fodder[] at all
 * only used for changing the world[][][] array
 * 3 //head red
 * 4 //body black
 * 7 //feet orange
*/ 
void drawHuman(int x, int y, int z){
   world[x][y][z] = 3;
   world[x][y-1][z] = 4;
   world[x][y-2][z] = 7;
   return;
}

void removeHuman(int x, int y, int z){
   world[x][y][z] = 0;
   world[x][y-1][z] = 0;
   world[x][y-2][z] = 0;
   return;
}

void alienMovement(){
   int loopAlien;
   int aaCollision;
   int humanLoop;
   int targetCollision;
   int notTarget = 0;
   float tempVelo[ALIEN_COUNT*2];
   float tempMagnitude[ALIEN_COUNT*2];

   for(loopAlien=1; loopAlien<ALIEN_COUNT*2; loopAlien++){
      tempVelo[loopAlien] = 1;
   }

   //llop through each lander
   for(loopAlien = 1; loopAlien < ALIEN_COUNT; loopAlien++){
      //if state is searching
   /************ state == 0 searching ************/
      tempMagnitude[loopAlien*2] = lander[loopAlien].vx;
      tempMagnitude[loopAlien*2+1] = lander[loopAlien].vz;

      removeLander((int)lander[loopAlien].base.x, (int)lander[loopAlien].base.y, (int)lander[loopAlien].base.z);

      if(lander[loopAlien].state == 0 && lander[loopAlien].visible == 1 ){
         //printf("%d\n", loopAlien);
         //removeLander((int)lander[loopAlien].base.x, (int)lander[loopAlien].base.y, (int)lander[loopAlien].base.z);

         
         //check if lander[loopAlien] needs to bouce off another lander
         for(aaCollision=1; aaCollision<ALIEN_COUNT; aaCollision++){ //check location of the other landers loop through the other landers
            //check if aa collision doesn't equal loopAlien so you don't compare position too itself
            if(aaCollision != loopAlien){
               //check if the landers are on the same height ::check if the y values are within 5 of each other
               if( (lander[loopAlien].base.y < lander[aaCollision].base.y + 5 && lander[loopAlien].base.y >= lander[aaCollision].base.y ) || (lander[aaCollision].base.y < lander[loopAlien].base.y + 5 && lander[aaCollision].base.y >= lander[loopAlien].base.y ) ){ 
                  //check if landers are within x +-5 and z+-5 of eachother
                  if( (lander[aaCollision].base.x <= lander[loopAlien].base.x+5 && lander[aaCollision].base.x >= lander[loopAlien].base.x) || (lander[aaCollision].base.x >= lander[loopAlien].base.x-5 && lander[aaCollision].base.x <= lander[loopAlien].base.x) ){
                     //check for z like above
                     if( (lander[aaCollision].base.z <= lander[loopAlien].base.z+5 && lander[aaCollision].base.z >= lander[loopAlien].base.z) || (lander[aaCollision].base.z >= lander[loopAlien].base.z-5 && lander[aaCollision].base.z <= lander[loopAlien].base.z) ){
                        //A collision has occured!!!! flip velos of 
         
                        /*FIXED::  this doesn't work because i have a perminent flip so when the check comes around for the othe rguy in search
                           *          it will give the wrong result
                           *          store a local var and store it it can be an array of some kind perhaps n*2 and (n*2)+1 for x and z
                           *          then outside this for loop loopn once again and each iteration check if the alien is in a search state 
                           *          before adjusting the velos
                           */

                           /**
                           * FIXED::
                           * add more checks if theres a collision
                           * 
                           * if collision and aliens are going the same way (only needs to be along one axis can still bounce off on the other)
                           * just swap the velos on the axis where both are going the same direction if one behind is faster than the one in front
                           * only change if ine will over take the other
                           * 
                           * 
                        **/
                        //check x velos
                        if(lander[aaCollision].vx < 0 && lander[loopAlien].vx > 0){
                           tempVelo[loopAlien*2] = -1;

                        }else if(lander[aaCollision].vx > 0 && lander[loopAlien].vx < 0){
                           tempVelo[loopAlien*2] = -1;
                        }else if(lander[aaCollision].vx > 0 && lander[loopAlien].vx > 0){
                           tempMagnitude[loopAlien*2] = lander[aaCollision].vx;
                        }else if(lander[aaCollision].vx < 0 && lander[loopAlien].vx < 0){
                           tempMagnitude[loopAlien*2] = lander[aaCollision].vx;
                        }
                        //check z velos
                        if(lander[aaCollision].vz < 0 && lander[loopAlien].vz > 0){
                           tempVelo[loopAlien*2+1] = -1;
                        }else if(lander[aaCollision].vz > 0 && lander[loopAlien].vz < 0){
                           tempVelo[loopAlien*2+1] = -1;
                        }else if(lander[aaCollision].vz > 0 && lander[loopAlien].vz > 0){
                           tempMagnitude[loopAlien*2+1] = lander[aaCollision].vz;
                        }else if(lander[aaCollision].vz < 0 && lander[loopAlien].vz < 0){
                           tempMagnitude[loopAlien*2+1] = lander[aaCollision].vz;
                        }
                     }
                  }
               }
            }
         }

         /**
             * check if a human is in range ** 
             * of the alien set the xyz velos they will be updated in the movement loop outside of this loop
             * 
             * 'target' coordinate struct in the alien struct
             * 
             * set velo based on current position and human position
             * 
             * shouldn't always be based on current position otherwise lander will speed up towards the human once a velo is set 
             * it shouldn't be changed till the alien has reached its destination
             * 
             * so state 1 update should update position not velo as an alien wont run into anything on its way to a human the other alien will 
             * bounce off
             * 
             * check if any of the other landers have the same target location
             * and if they do need to, stop alienLoop lander because the remaining one will not match any other lander
             * this will favor the landers with a later id but if two find the same human in the same cycle the first lander will keep 
             * their target 
             * 
          **/

         for(humanLoop = 0; humanLoop < BODY_COUNT; humanLoop++){
            //check if these are within the radius 
            //then take y of the head +1 and make a beeline
            if( lander[loopAlien].base.x - fodder[humanLoop].head.x <= SONAR && lander[loopAlien].base.x - fodder[humanLoop].head.x >= -SONAR && fodder[humanLoop].visible == 1){
               if( lander[loopAlien].base.z - fodder[humanLoop].head.z <= SONAR && lander[loopAlien].base.z - fodder[humanLoop].head.z >= -SONAR && fodder[humanLoop].visible == 1){
               
                  float distance = 0.0;
                  distance = sqrt( pow(lander[loopAlien].base.x - fodder[humanLoop].head.x, 2) + pow( lander[loopAlien].base.z - fodder[humanLoop].head.z, 2) );
                  if(distance <= SONAR){

                  //loop through checking what all the landers targets are
                     for(targetCollision=1; targetCollision<ALIEN_COUNT; targetCollision++){
                        if( lander[targetCollision].target.x == fodder[humanLoop].head.x && lander[targetCollision].target.z == fodder[humanLoop].head.z ){
                           notTarget++;
                        }
                     }
                  //if no target collisions are found set target and switch to beeline
                     if(notTarget==0){
                        lander[loopAlien].target.x = fodder[humanLoop].head.x;
                        lander[loopAlien].target.z = fodder[humanLoop].head.z;
                        lander[loopAlien].target.y = (fodder[humanLoop].head.y)+1;

                        //set targetiD for alien 
                        //targetID is the array position of the human in fodder
                        int findID;
                        for(findID = 0; findID < BODY_COUNT; findID++){
                           if( fodder[findID].head.x == lander[loopAlien].target.x && fodder[findID].head.z == lander[loopAlien].target.z ){
                              lander[loopAlien].targetID = findID;
                              break;
                           }
                        }

                        lander[loopAlien].state = 1;

                        //set new velos 
                        lander[loopAlien].vx = (lander[loopAlien].base.x - lander[loopAlien].target.x) /100;
                        lander[loopAlien].vy = (lander[loopAlien].base.y - lander[loopAlien].target.y) /100;
                        lander[loopAlien].vz = (lander[loopAlien].base.z - lander[loopAlien].target.z) /100;


                     }
                     


                     //loop here to check if any other alien has this target and if it does set target values back to 0 and set state back to search

                           

                  }
               }
            }


         }
         
   /************ state == 1 bee line to human head ************/
      }else if(lander[loopAlien].state == 1 && lander[loopAlien].visible == 1 ){
         if( lander[loopAlien].base.y <= lander[loopAlien].target.y){// && lander[loopAlien].base.x <= lander[loopAlien].target.x && lander[loopAlien].base.z <= lander[loopAlien].target.z ){
            lander[loopAlien].vy = 0;

         //this seems to work
            // lander[loopAlien].vx = lander[loopAlien].base.x - lander[loopAlien].target.x;
            // lander[loopAlien].vz = lander[loopAlien].base.z - lander[loopAlien].target.z;
            lander[loopAlien].base.x = lander[loopAlien].target.x;
            lander[loopAlien].base.z = lander[loopAlien].target.z;

            
         }
         if(lander[loopAlien].base.x == lander[loopAlien].target.x){
            lander[loopAlien].vx = 0;
         }
         if(lander[loopAlien].base.z == lander[loopAlien].target.z){
            lander[loopAlien].vz = 0;
         }

   /************ state == 2 abduct human ************/
      }else if(lander[loopAlien].state == 2 && lander[loopAlien].visible == 1 ){
         lander[loopAlien].vy = 0.1;

         removeHuman(fodder[ lander[loopAlien].targetID ].head.x, fodder[ lander[loopAlien].targetID ].head.y, fodder[ lander[loopAlien].targetID ].head.z);
         fodder[ lander[loopAlien].targetID ].head.y += 0.1;
         drawHuman(fodder[ lander[loopAlien].targetID ].head.x, fodder[ lander[loopAlien].targetID ].head.y, fodder[ lander[loopAlien].targetID ].head.z);
   /************ state == 3 return to human searching altitude ************/
      }else if(lander[loopAlien].state == 3 && lander[loopAlien].visible == 1 ){
         lander[loopAlien].vy = 0.2;

      }
   }
   /**
    * finalize the velos of each alien and then set new position and draw on the map
    * checking for edge of the world collision must happen last in this process to avoid bus error**
    * 
    * use the tempVelo to change the velos where collisions were detected
    * if a collision wasn't detected the value will just be 1 so there's no harm in running through the whole loop
    * 
    * aliens will only bounce if they are in a searching state so cover this loop in an if statement
    * 
   **/
   
   /************ this loop sets the new position and draws the lander in everything before this should only change the velos! ************/
   for(loopAlien=1; loopAlien<ALIEN_COUNT; loopAlien++){
      if(lander[loopAlien].state == 0){
         //magnitude
         //magnitude is used when 2 aliens swap velocities as one catches up to another so it must be equals not anything else
         lander[loopAlien].vx = tempMagnitude[2*loopAlien];
         lander[loopAlien].vz = tempMagnitude[2*loopAlien+1];


         //direction
         //velo is used when 2 run into eachother and the velocity is multiplied by -1 or 1
         lander[loopAlien].vx *= tempVelo[2*loopAlien];
         lander[loopAlien].vz *= tempVelo[2*loopAlien+1];
         //this should be the last thing done before updating as if it does not have the highest prioity it will cause a bus error
         //check if it needs to bouce off an edge
         if( lander[loopAlien].base.x + lander[loopAlien].vx > 97 || lander[loopAlien].base.x + lander[loopAlien].vx < 2 ){
            lander[loopAlien].vx *= -1;
         }
         if( lander[loopAlien].base.z + lander[loopAlien].vz > 97 || lander[loopAlien].base.z + lander[loopAlien].vz < 2 ){
            lander[loopAlien].vz *= -1;
         }

         lander[loopAlien].base.x += lander[loopAlien].vx;
         lander[loopAlien].base.z += lander[loopAlien].vz;
      
      }else if(lander[loopAlien].state == 1){
         //update position toward human head and set velos to 0 once the head is reached

         if( lander[loopAlien].base.x + lander[loopAlien].vx > 97 || lander[loopAlien].base.x + lander[loopAlien].vx < 2 ){
            lander[loopAlien].vx *= -1;
         }
         if( lander[loopAlien].base.z + lander[loopAlien].vz > 97 || lander[loopAlien].base.z + lander[loopAlien].vz < 2 ){
            lander[loopAlien].vz *= -1;
         }

         // if( lander[loopAlien].base.x > lander[loopAlien].target.x){
         //    lander[loopAlien].base.x -= lander[loopAlien].vx;
         // }else{
         //    lander[loopAlien].base.x += lander[loopAlien].vx;
         // }

         // if( lander[loopAlien].base.z > lander[loopAlien].target.z){
         //    lander[loopAlien].base.z -= lander[loopAlien].vz;
         // }else{
         //    lander[loopAlien].base.z += lander[loopAlien].vz;
         // }
         lander[loopAlien].base.x -= lander[loopAlien].vx;
         lander[loopAlien].base.z -= lander[loopAlien].vz;
         lander[loopAlien].base.y -= lander[loopAlien].vy;
         if(lander[loopAlien].vx == 0 && lander[loopAlien].vy == 0 && lander[loopAlien].vz == 0){
            lander[loopAlien].state = 2;
         }
         
      }else if(lander[loopAlien].state == 2){
         if( lander[loopAlien].base.y + lander[loopAlien].vy > 44 ){
            lander[loopAlien].visible = 0;
            removeHuman(fodder[ lander[loopAlien].targetID ].head.x, fodder[ lander[loopAlien].targetID ].head.y, fodder[ lander[loopAlien].targetID ].head.z);
            fodder[ lander[loopAlien].targetID ].visible = 0;
         } else{
            lander[loopAlien].base.y += lander[loopAlien].vy;
         }
         
      }else if(lander[loopAlien].state == 3){
         if(lander[loopAlien].searchingHeight <= lander[loopAlien].base.y){
            
            lander[loopAlien].state = 0;
         //add new x,z velos to the lander
            int randVelo, randVeloZ;
            randVelo = rand() % 4;
            if(randVelo == 0){
               lander[loopAlien].vx = 0.1;
            }
            if(randVelo == 1){
               lander[loopAlien].vx = -0.1;
            }
            if(randVelo == 2){
               lander[loopAlien].vx = 0.3;
            }
            if(randVelo == 3){
               lander[loopAlien].vx = -0.3;
            }
            if(randVelo == 4){
               lander[loopAlien].vx = 0.2;
            }

            //this loop prevents the alien having the same x and z velo causing it to get stuck in the long diagonal of the world
            do{
               randVeloZ = rand() % 4;
            }while(randVelo == randVeloZ);
            
            if(randVeloZ == 0){
               lander[loopAlien].vz = 0.1;
            }
            if(randVeloZ == 1){
               lander[loopAlien].vz = -0.1;
            }
            if(randVeloZ == 2){
               lander[loopAlien].vz = 0.3;
            }
            if(randVeloZ == 3){
               lander[loopAlien].vz = -0.3;
            }
            if(randVeloZ == 4){
               lander[loopAlien].vz = 0.2;
            }




         } else{
            lander[loopAlien].base.y += lander[loopAlien].vy;
         }
         
      }

      if(lander[loopAlien].visible == 1){
         drawLander( (int)lander[loopAlien].base.x, (int)lander[loopAlien].base.y, (int)lander[loopAlien].base.z);
      }
   } 




   

   return;
}

void humanGravity(){

   float gravityRate = 0.4;
   int loop = 0;
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

   return;
}

void viewMomentum(){
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


   return;
}

void removeRayLoop(){
   int rayLoop = 0;
   for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
      if(tubeVisible[rayLoop] == 1){
         ftime(&shotClock);
         if(myRays[rayLoop].shotAt.time < shotClock.time - 5){
            tubeVisible[rayLoop] = 0;
         }
      }      
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
   float spaceY = 0.6;

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
   if( world[ (int)x ][ (int)y ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)z ] != 0 || world[ (int)x ][ (int)(y+spaceY) ][ (int)z ] != 0 || world[ (int)x ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)z ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)x ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)(x+space) ][ (int)(y+spaceY) ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)x ][ (int)(y+spaceY) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y+spaceY) ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)y ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)y ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y+spaceY) ][ (int)(z+space) ] != 0 || world[ (int)(x-space) ][ (int)(y+spaceY) ][ (int)z ] != 0 || world[ (int)(x-space) ][ (int)(y+spaceY) ][ (int)(z-space) ] != 0 || world[ (int)x ][ (int)(y+spaceY) ][ (int)(z-space) ] != 0 || world[ (int)(x+space) ][ (int)(y+spaceY) ][ (int)(z-space) ] != 0 || world[ (int)(x-space) ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)x ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)(z+space) ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)z ] != 0 || world[ (int)(x+space) ][ (int)(y-space) ][ (int)(z-space) ] != 0){

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
   float smallW = screenWidth/400;
   float smallH = screenHeight/300;

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
      GLfloat green[] = {0.0, 0.5, 0.0, 1};
      set2Dcolour(black);

      if (displayMap == 1) {
          
   //draw character position on the map
         set2Dcolour(white);

         getViewPosition(&px, &py, &pz);
         px *= -1;
         pz *= -1;
         py *= -1;

         //format x, z(y) , x, z(y), x, z(y)
         draw2Dtriangle( screenWidth - ((110 * smallW) - (px * smallW)), screenHeight - ((110 * smallH) - (pz * smallH)), screenWidth - ((111 * smallW) - (px * smallW)), screenHeight - ((112 * smallH) - (pz * smallH)), screenWidth - ((109 * smallW) - (px * smallW)), screenHeight - ((112 * smallH) - (pz * smallH)) );
          
   //draw rays on the map
         set2Dcolour(purple);
         
         for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
            if(tubeVisible[rayLoop] == 1){
               //printf("tubevisible %d\n", rayLoop);
               rayBX = tubeData[rayLoop][0];
               rayBZ = tubeData[rayLoop][2];
               rayEX = tubeData[rayLoop][3];
               rayEZ = tubeData[rayLoop][5];
               if( (110 - rayEX) < 10 ){
                  rayEX = 100;
               }
               if( (110 - rayEZ) < 10 ){
                  rayEZ = 100;
               }
               if( (110 - rayEX) > 110 ){
                  rayEX = 0;
               }
               if( (110 - rayEZ) > 110 ){
                  rayEZ = 0;
               }
               draw2Dline(screenWidth - ((110 * smallW) - (rayBX * smallW)), screenHeight - ((110 * smallH) - (rayBZ * smallH)), screenWidth - ((110 * smallW) - (rayEX * smallW)), screenHeight - ((110 * smallH) - (rayEZ * smallH)), 2);
            }
         }      
   
   //draw all the humans on the map
         set2Dcolour(red);
          int xb = 0;
          int zb = 0;
          int xe = 0;
          int ze = 0;
         for(i = 0; i < BODY_COUNT; i++){
            if(fodder[i].visible == 1){
               x = fodder[i].head.x;
               z = fodder[i].head.z;
               xb = screenWidth - ((110 * smallW) - (x * smallW));
               zb = screenHeight - ((110 * smallH) - (z * smallH));
               xe = screenWidth - ((110 * smallW) - (x * smallW));
               ze = screenHeight - ((110 * smallH) - (z * smallH));
               xe += (smallW * 1.5);
               ze += (smallH * 1.5);
                 
               draw2Dbox( xb, zb, xe, ze);
            }
         }
   //draw the Landers on the map  
         set2Dcolour(green);    
         xb = 0;
         zb = 0;
         xe = 0;
         ze = 0;
         for(i = 1; i < ALIEN_COUNT; i++){
            if(lander[i].visible == 1){
               x = lander[i].base.x;
               z = lander[i].base.z;
               xb = screenWidth - ((110 * smallW) - (x * smallW));
               zb = screenHeight - ((110 * smallH) - (z * smallH));
               xe = xb;
               ze = zb;

               //horizontal line
               xb -= (smallW * 1.5);
               xe += (smallH * 1.5);
               draw2Dline( xb, zb, xe, ze, 2);
               //reset
               xb += (smallW * 1.5);
               xe -= (smallH * 1.5);
               //vertical line
               zb -= (smallW * 1.5);
               ze += (smallH * 1.5);
               draw2Dline( xb, zb, xb, ze, 2);
            }


         }
   //draw the map background must be done last as the first thing drawn is drawn ontop
         set2Dcolour(white);
         draw2Dline(screenWidth - (9 * smallW), screenHeight - (9 * smallH), screenWidth - (111 * smallW), screenHeight - (9 * smallH), 4);
         draw2Dline(screenWidth - (111 * smallW), screenHeight - (9 * smallH), screenWidth - (111 * smallW), screenHeight - (111 * smallH), 4);
         draw2Dline(screenWidth - (111 * smallW), screenHeight - (111 * smallH), screenWidth - (9 * smallW), screenHeight - (111 * smallH), 4);
         draw2Dline(screenWidth - (9 * smallW), screenHeight - (111 * smallH), screenWidth - (9 * smallW), screenHeight - (9 * smallH), 4);

         set2Dcolour(black);
         draw2Dbox(screenWidth - (10 * smallW), screenHeight - (10 * smallH), screenWidth - (110 * smallW), screenHeight - (110 * smallH));

      }
   //big map
      if (displayMap == 2) {
         //loop for rays
         int rayLoop;
         int rayBX, rayBZ, rayEX, rayEZ;

   //draw character position on the map
          set2Dcolour(white);
          
          getViewPosition(&px, &py, &pz);
          px *= -2;
          pz *= -2;
          py *= -2;
          
          //format x, z(y) , x, z(y), x, z(y)
          draw2Dtriangle( screenWidth - ((300 * smallW) - (px * smallW)), screenHeight - ((250 * smallH) - (pz * smallH)), screenWidth - ((301 * smallW) - (px * smallW)), screenHeight - ((253 * smallH) - (pz * smallH)), screenWidth - ((299 * smallW) - (px * smallW)), screenHeight - ((253 * smallH) - (pz * smallH)) );
   //draw all the rays on the map
          set2Dcolour(purple);
          
          for(rayLoop=0; rayLoop < TUBE_COUNT; rayLoop++){
              if(tubeVisible[rayLoop] == 1){
                  //printf("tubevisible %d\n", rayLoop);
                  rayBX = tubeData[rayLoop][0];
                  rayBZ = tubeData[rayLoop][2];
                  rayEX = tubeData[rayLoop][3];
                  rayEZ = tubeData[rayLoop][5];
                  if( (110 - rayEX) < 10 ){
                      rayEX = 100;
                  }
                  if( (110 - rayEZ) < 10 ){
                      rayEZ = 100;
                  }
                  if( (110 - rayEX) > 110 ){
                      rayEX = 0;
                  }
                  if( (110 - rayEZ) > 110 ){
                      rayEZ = 0;
                  }
                  rayEZ *= 2;
                  rayEX *= 2;
                  rayBZ *= 2;
                  rayBX *= 2;
                  draw2Dline(screenWidth - ((300 * smallW) - (rayBX * smallW)), screenHeight - ((250 * smallH) - (rayBZ * smallH)), screenWidth - ((300 * smallW) - (rayEX * smallW)), screenHeight - ((250 * smallH) - (rayEZ * smallH)), 2);
                  
                  
              }
          }
   //draw all the humans on the map
          set2Dcolour(red);
          int xb = 0;
          int zb = 0;
          int xe = 0;
          int ze = 0;
          for(i = 0; i < BODY_COUNT; i++){
             if(fodder[i].visible == 1){
               x = fodder[i].head.x * 2;
               z = fodder[i].head.z * 2;
               xb = screenWidth - ((300 * smallW) - (x * smallW));
               zb = screenHeight - ((250 * smallH) - (z * smallH));
               xe = screenWidth - ((300 * smallW) - (x * smallW));
               ze = screenHeight - ((250 * smallH) - (z * smallH));
               xe += (smallW * 2);
               ze += (smallH * 2);
              
               draw2Dbox( xb, zb, xe, ze);
             }
          }
   //draw all the Landers on the map
         set2Dcolour(green);
         xb = 0;
         zb = 0;
         xe = 0;
         ze = 0;
         for(i = 1; i < ALIEN_COUNT; i++){
            if(lander[i].visible == 1){
               x = lander[i].base.x * 2;
               z = lander[i].base.z * 2;
               xb = screenWidth - ((300 * smallW) - (x * smallW));
               zb = screenHeight - ((250 * smallH) - (z * smallH));
               
               xe = xb;
               ze = zb;

               //horizontal line
               xb -= (smallW * 2.5);
               xe += (smallH * 2.5);
               draw2Dline( xb, zb, xe, ze, 3);
               //reset
               xb += (smallW * 2.5);
               xe -= (smallH * 2.5);
               //vertical line
               zb -= (smallW * 2.5);
               ze += (smallH * 2.5);
               draw2Dline( xb, zb, xb, ze, 3);
            }
          }


   //base colour and border of map
         set2Dcolour(white);
          draw2Dline(screenWidth - (98 * smallW), screenHeight - (49 * smallH), screenWidth - (301 * smallW), screenHeight - (49 * smallH), 4);
          draw2Dline(screenWidth - (301 * smallW), screenHeight - (49 * smallH), screenWidth - (301 * smallW), screenHeight - (251 * smallH), 4);
          draw2Dline(screenWidth - (301 * smallW), screenHeight - (251 * smallH), screenWidth - (98 * smallW), screenHeight - (251 * smallH), 4);
          draw2Dline(screenWidth - (98 * smallW), screenHeight - (251 * smallH), screenWidth - (98 * smallW), screenHeight - (49 * smallH), 4);
          
          set2Dcolour(black);
          draw2Dbox(screenWidth - (100 * smallW), screenHeight - (50 * smallH), screenWidth - (300 * smallW), screenHeight - (250 * smallH));
         
      }


   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
   //int i, j, k, loop;
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
      removeRayLoop();
      
   /****** alien movement ******/
      ftime(&newAT);
      float diffA = 0.0;
      diffA = newAT.millitm - oldAT.millitm;
      if( diffA > 25 || diffA < -25){
         ftime(&oldAT);
      
         alienMovement();
      }
      
   /****** Gravity && momentum ******/
      ftime(&newT);
      /* newT should be checked here to curb framerate */
      float diff = 0.0;
      diff = newT.millitm - oldT.millitm;
      if( diff > 25 || diff < -25){
         ftime(&oldT);
   /****** Gravity ******/
         humanGravity();
   /****** Momentum ******/
         viewMomentum();
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

   /*************** initializing my world ***************/
      FILE *fp;
      fp = fopen("ground.pgm", "r");

      time_t t;
      ftime(&oldT);
      srand((unsigned) time(&t));

      int loop;
      int random;
      int randomX;
      int xVal;
      int groundLevel;
      int height=0;
      char str[10];
      movement.x = 0;
      movement.y = 0;
      movement.z = 0;

      //create a new colour for ground so its not neon green
      setUserColour(9, 0.29, 0.60, 0.42, 0.7, 0.29, 0.60, 0.42, 0.7);  //ground
      setUserColour(15, 0.96, 0.96, 0.99, 1.0, 0.96, 0.96, 0.99, 1.00); // snow

      

      for(i=0; i<8; i++){
         fscanf(fp, "%s",  str);
         //printf("%s\n", str);
      }
      
      //make ground from pgm file
      for(i=0; i<100; i++){
         for(j=0; j<100; j++){

            fscanf(fp, "%s",  str);
            height = atoi(str);
            height = height / 25;    //early front runner is 25 for choice      
               

            do{
               if( height > 8){
                  world[i][height][j] = 5;//snow
                  //printf("the max height is :: %d\n", height);
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

         //just for testing take out when testing over and body count needs to be changed back too 100
         randomX = rand() % 10;// z value 
         xVal = loop * 10;
         xVal += randomX;
         //make sure a human is far enough away from the edge for aliens to abduct for x borders
         if(xVal > 97){
            xVal = 97;
         } else if( xVal < 2){
            xVal = 2;
         }


         //x needs to be looped through will piggyback on 'loop'; all will start at 49, 48, 47; z will be random

         //make sure a human is far enough away from the edge for aliens to abduct for z borders
         random = rand() % WORLDZ;// z value 
         if(random > 97){
            random = 97;
         } else if( random < 2){
            random = 2;
         }

         while( world[xVal][6][random] == 0 || world[xVal+2][8][random] != 0|| world[xVal-2][8][random] != 0 || world[xVal][8][random+2] != 0 || world[xVal][8][random-2] != 0 || world[xVal+1][8][random] != 0|| world[xVal-1][8][random] != 0 || world[xVal][8][random+1] != 0 || world[xVal][8][random-1] != 0){
            random ++;
            if(random > 97){
               random = 2;
            }

         }

         //create a loop here to loop throught the start at one unit above highest possible ground at these x, z coordinates to find the ground and place the people from there
         for( groundLevel = 11; groundLevel>0; groundLevel-- ){
            if(world[xVal][groundLevel][random] != 0 ){
               groundLevel++;
               break;
            }
         }
      //make human visible
         fodder[loop].visible = 1;

      //make head
         fodder[loop].head.x = xVal;
         fodder[loop].head.y = groundLevel + 2;
         fodder[loop].head.z = random;

      //make body
         fodder[loop].body.x = xVal;
         fodder[loop].body.y = groundLevel + 1;
         fodder[loop].body.z = random;

      //make feet
         fodder[loop].feet.x = xVal;
         fodder[loop].feet.y = groundLevel;
         fodder[loop].feet.z = random;

      //used to test gravity sets people at the top of the world
         // fodder[loop].head.y = 49;
         // fodder[loop].body.y = 48;
         // fodder[loop].feet.y = 47;

      //place humans in the world at the top of the sky
         world[xVal][groundLevel + 2][random] = 3;//head red
         world[xVal][groundLevel + 1][random] = 4;//body black
         world[xVal][groundLevel][random] = 7;//feet orange

      }
   //*************** load aliens/Landers into the world ***************//

      
      //coordinates of landers
      int alienX, alienY, alienZ;
      int randVelo = 0;
      int randVeloZ = 0;
      ftime(&oldAT);

      //drawLander(alienX, alienY, alienZ);

      for(loop=1; loop<ALIEN_COUNT; loop++){

         if(loop == 0){
            alienX = 15;
         }
         alienX = (loop * 20)-6;
         alienY = 30;// + (loop * 3);
         alienZ = rand() % WORLDZ;
         if(alienZ > 97){
            alienZ = 97;
         }
         if(alienZ < 2){
            alienZ = 2;
         }



         lander[loop].base.x = alienX;
         lander[loop].base.y = alienY;
         lander[loop].base.z = alienZ;

         //setting target to 0,0,0 as default
         lander[loop].target.x = 0;
         lander[loop].target.y = 0;
         lander[loop].target.z = 0;

         //1 is visible the default
         lander[loop].visible = 1;

         randVelo = rand() % 4;
         if(randVelo == 0){
            lander[loop].vx = 0.1;
         }
         if(randVelo == 1){
            lander[loop].vx = -0.1;
         }
         if(randVelo == 2){
            lander[loop].vx = 0.3;
         }
         if(randVelo == 3){
            lander[loop].vx = -0.3;
         }
         if(randVelo == 4){
            lander[loop].vx = 0.2;
         }

      //this loop prevents the alien having the same x and z velo causing it to get stuck in the long diagonal of the world
         do{
            randVeloZ = rand() % 4;
         }while(randVelo == randVeloZ);
         
         if(randVeloZ == 0){
            lander[loop].vz = 0.1;
         }
         if(randVeloZ == 1){
            lander[loop].vz = -0.1;
         }
         if(randVeloZ == 2){
            lander[loop].vz = 0.3;
         }
         if(randVeloZ == 3){
            lander[loop].vz = -0.3;
         }
         if(randVeloZ == 4){
            lander[loop].vz = 0.2;
         }

         // lander[loop].vx = 0.5;
         // lander[loop].vz = 0.5;
         lander[loop].vy = 0; // because initial state is searching so it should not move vertically initially
         lander[loop].state = 0;
         lander[loop].searchingHeight = lander[loop].base.y;

         drawLander((int)lander[loop].base.x, (int)lander[loop].base.y, (int)lander[loop].base.z);

      }




   }//end of world initialization!!!
      


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}



