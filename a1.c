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
#include <time.h>

#define BODY_COUNT 100
#define ACCELERATION 160


#include "graphics.h"

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
time_t oldT;
time_t newT;








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
   if( x > 99 || x < 0 || z > 99 || z < 0 || y < 0 || y > 49){
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

      newT = time(NULL);

   /* newT should be checked here to curb framerate */
      if( newT >= oldT){
         oldT = newT;
         /****** Gravity ******/
         float gravityRate = 0.2;
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
      //printf("left button - ");
    }else if (button == GLUT_MIDDLE_BUTTON){
      //printf("middle button - ");
    }else{
      //printf("right button - ");
    }

    if (state == GLUT_UP){
      //printf("up - ");
    }else{
      //printf("down - ");
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
      oldT = time(NULL);
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



