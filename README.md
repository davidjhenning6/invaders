# invaders
cis4820 Assignments


/**************
Dave Henning
cis4820
Assignment 1
***************/
******************************************** 1. Finish collision detection ********************************************
    ---------------------DONE---------------------
    -> only check the 6 faces of a cube right now so can still get very close to edges and verticies
    -> can't get within a whole cube unit of a face which is a little to far game
    note** whole cube has been checked to increase hit bix of viewpoint adjust space(float) value in collisionResponse function
    

******************************************** 2. Momentum this has not been started ********************************************

    --------------------Active--------------------
    

    ---------------------DONE---------------------
    X-> speed has been altered but momentum has not been implemented
        //note search('note') to find speed controls og value was 0.3

    X->odd momentum movement FIXXED i only made one move var not one for each coordinate

    done/Fixed results in speed error for momentum,-> *******need to build some kind of ratio as momentum is only +-1, +-1, +-1, so it favors to the corners not the actual momentum!!

    done*****error found in momentum push back from collision detection results in "momentum" can be infinite if caught bouncing back and forth between two objects in a canyon
    X->FIXXED count set to ACCELERATION ended any momentum currently happening

    note momentum also had to result in a movement in every direction x, y, and z because i didn't check if 'x = momentum.x'

    DONE->momentum can end shorter than expected as the count starts midflight and ends when the count is done not at the final push of 'w''a''s''d'

    DONE->movement speeds out of control when a key is held for more than 3 seconds caused by ratio!!!!
        solution idea check difference when setting update position to limit it to a maximum amount 
        could even be implemented in the setter theoretically

   ---------------------Note---------------------
    
   new patch momentum slows into stop 

******************************************** 3. stretch goal* make a new colour thats less blinding for ground ********************************************
    ---------------------DONE---------------------
    note** colours created in the else{} of main

//note f controls vertical movement, it can be turned off

!!!!!!!!!!!!time limiter on update needs to be implemented but first attempt caused issues 
time.h's time(NULL) may not be accurate enough!




/**************
Dave Henning
cis4820
Assignment 2
***************/


******************************************** 1. Timing ********************************************
    --------------------Active--------------------
    ---------------------DONE---------------------
    sys/timeb.h was used to slow the amount of times gravity and momentum is called inside of update()
    - 25 miliseconds has elapsed since the last call
    - time.h was kept for rand in main
    - gravityRate was changed to 0.4 to make gravity a playable speed while keeping momentum useable since it relys on a fairly high update speed

********************************************  2. Acceleration ********************************************
    --------------------Active--------------------
    ---------------------DONE---------------------
    this was done in the previous iteration with momentum


******************************************** 3. Firing a ray in the viewpoint direction. ********************************************
    --------------------Active--------------------
    - ray has been launched though it seems to have trouble following the y axis when the player looks up similar to what happens when you move
    - ray may need to be drawn peice by peice and appear to grow

    - remove ray after a time 
        -> ** perhaps a call in update **
        -- images on phone explaining how to do that
    - a message needs to be printed to stdout when a ray intersect with a human a simple message is fine its not implicitly specified

    ---------------------DONE---------------------
    Launch a ray from the viewpoint. It should travel in the direction which the viewpoint was facing when it is launched.
    - the Tube functions need to be used to draw and hide the ray

    - ray launched on click of mouse
    
    solved equation done
        -- current major problem is that c's math.h uses radians for sin and cos so i need to convert my angles to make it work

    - add the ability for more than one ray to form
        -- capped at 10 max on screen at a time

    
******************************************** 4. Displaying a Two Dimensional World Map ********************************************
    --------------------Active--------------------

    - map at 1 displays small map in upper right corner
        - When a ray is fired it should be drawn on the map while it exists
            --rays appear however ask if the rays should end at the edges of the map or not **

    - map at 2 displays a large map over the screen
        - When a ray is fired it should be drawn on the map while it exists
            --rays appear however ask if the rays should end at the edges of the map or not **

    Stretch goal -> control direction the triangle points

    ---------------------DONE---------------------

     - map at 0 displays no map

    - map at 1 displays small map in upper right corner
        - people are shown as squares
        - player is shown as a triangle which needs to be constantly updated
        - map is a square with borders that reflects the games xz coordinates

    - map at 2 displays a large map over the screen
        - people are shown as squares
        - player is shown as a triangle which needs to be constantly updated
        - map is a square with borders that reflects the games xz coordinates

    - the screenWidth and screenHeight variables need be used to scale the size of the maps when the window size is changed.   

    
    