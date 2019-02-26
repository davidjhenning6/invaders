# invaders
cis4820 Assignments


###############
Dave Henning
cis4820
#Assignment 1
###############
############################################ 1. Finish collision detection ############################################
    ---------------------DONE---------------------
    -> only check the 6 faces of a cube right now so can still get very close to edges and verticies
    -> can't get within a whole cube unit of a face which is a little to far game
    note** whole cube has been checked to increase hit bix of viewpoint adjust space(float) value in collisionResponse function
    

############################################ 2. Momentum this has not been started ############################################

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

############################################ 3. stretch goal* make a new colour thats less blinding for ground ############################################
    ---------------------DONE---------------------
    note** colours created in the else{} of main

//note f controls vertical movement, it can be turned off

!!!!!!!!!!!!time limiter on update needs to be implemented but first attempt caused issues 
time.h's time(NULL) may not be accurate enough!




###############
Dave Henning
cis4820
#Assignment 2
###############


############################################ 1. Timing ############################################
    --------------------Active--------------------
    ---------------------DONE---------------------
    sys/timeb.h was used to slow the amount of times gravity and momentum is called inside of update()
    - 25 miliseconds has elapsed since the last call
    - time.h was kept for rand in main
    - gravityRate was changed to 0.4 to make gravity a playable speed while keeping momentum useable since it relys on a fairly high update speed

############################################  2. Acceleration ############################################
    --------------------Active--------------------
    ---------------------DONE---------------------
    this was done in the previous iteration with momentum


############################################ 3. Firing a ray in the viewpoint direction. ############################################
    --------------------Active--------------------
    UNSURE - ray has been launched though it seems to have trouble following the y axis when the player looks up similar to what happens  when you move



    ---------------------DONE---------------------

    DONE - a message needs to be printed to stdout when a ray intersect with a human a simple message is fine its not implicitly specified


    Launch a ray from the viewpoint. It should travel in the direction which the viewpoint was facing when it is launched.
    - the Tube functions need to be used to draw and hide the ray

    - ray launched on click of mouse
    
    solved equation done
        -- current major problem is that c's math.h uses radians for sin and cos so i need to convert my angles to make it work

    - add the ability for more than one ray to form
        -- capped at 10 max on screen at a time

     DONE - remove ray after a time 
        -> ** perhaps a call in update **
        -- images on phone explaining how to do that

        DONE **
        -> in update 
            only update time when shoot
                to activate only set new time when a ray is fired in fireTube()

            struct -> has an int id, and sys/time time fired

            for loop check each id's time
                check each id's time if 'x' time has elapsed set to invisible
                no need to reset time as the tube will already be invisible so no harm in setting it to invisible when its already invisible


    ****scrapped****
    if x or z is close to zero than they dont have to be close!!!!!!!!!!!!!!!!!!!!!!!
    if a value is greater than 0. ignore the cube
    && samex < 0.31 && samex > -0.31 && samez < 0.31 && samez > -0.31 )
    this can't happen!!!! -> samex 0.913306,  samey 0.705073  samez 0.028021

    try converting back to block like ash did with the cube example

    
############################################ 4. Displaying a Two Dimensional World Map ############################################
    --------------------Active--------------------
    Stretch goal -> control direction the triangle points

    ---------------------DONE---------------------

    - maps now scale based on the window size

     - map at 0 displays no map

    - map at 1 displays small map in upper right corner
        - people are shown as squares
        - player is shown as a triangle which needs to be constantly updated
        - map is a square with borders that reflects the games xz coordinates
        - When a ray is fired it should be drawn on the map while it exists
            --rays appear however ask if the rays should end at the edges of the map or not **

    - map at 2 displays a large map over the screen
        - people are shown as squares
        - player is shown as a triangle which needs to be constantly updated
        - map is a square with borders that reflects the games xz coordinates
        - When a ray is fired it should be drawn on the map while it exists
            --rays appear however ask if the rays should end at the edges of the map or not **

    - the screenWidth and screenHeight variables need be used to scale the size of the maps when the window size is changed.   

    
    
#################
Dave Henning
cis4820
#Assignment 3
#################




############################################ 1. Draw Aliens and aliens colliding with others ############################################
    --------------------Active--------------------
   
   an alien may move through the viewpoint it is soo fast its hard to tell but its easily shown
    
    
    **if an alien is in any state other than 0 it is not effected by a collision with another alien
    
    ** make sure alien movement does not let player view point enter it!
        - could push viewpoint up with it

    

    ---------------------DONE---------------------
    when creating struct keep track of this point it will be block connecting to human
    keep track of top and bottom 

    use colour 12 for alien's eyes 4 black is reserved for human ray detection

    make a function to remove alien after image after it moves

    keep track of outer x,z range ie base point +-x and +-z

    **check if aliens collide and have them bounce off of each other


############################################ 2. AI for Aliens ############################################
    --------------------Active--------------------
    4 states 0 -> 4 monitered by value in alien struct
    
    0 searching state

    1 bee line to human head x,y,z must all change each step when detected

    2 lift straight up

    3 return to searching height an
   
   ****** add a human head target coordinate to the alien class using coordinate struct!!

    ---------------------DONE---------------------


############################################ 3. Collision for Aliens/humans getting shot ############################################
    --------------------Active--------------------
   

    ---------------------DONE---------------------

############################################ 4. make humans spawn on ground ############################################
    --------------------Active--------------------
    check that all 100 humans all start on ground
    think about lowering body count to make game more possible and for aliens to search state longer

    

    ---------------------DONE---------------------
    my test 10 humans all start on ground

    **think about making humans only spawn on ground of a certain height it will prevent the aliens from connecting with the ground
    - spawn humans in a formation where an alien picking one up will never hit another with its foot
        -- must check 4 directions around a human before setting it there ie x value +/- 2 and z value +/-2 at the humans head y value
        -- must prevent humans from spawning to close to the edge of the world or the ship wont be able to reach it



### UPDATED TODO LIST ###

1. draw aliens on the map DONE
DONE forgot to remove from map when not visible

2. aliens searching DONE
- search radius its like a cylinder

*** i'll need to reset the target at some point DONE
- reset when the lander first enters search state

i changed aliens to update .25 instead of .1


3. beeline to pick up human DONE

4. grab human and fly away DONE
- disappear with human when hit roof
### current task below
- add a flag to the human struct to control if gravity should effect it or not may help prevent the blinking
- flag added to human gravity is set at initialization, and gravity turned off in the second check of alienMovement at the end of state == 1, the state is changed to 2 

5. if shot when holding human drop it and DIE!!!!
- will have to set gravity flag back to 1 to turn gravity back on



6. if human shot then alien returns to searching at search height and human disappears DONE

7. if human drops from a certain height than human disappears when it hits the ground DONE

LEFT TO DO
is identify lander shot 
prevent the same lander from being shot more than once per ray DONE



NOTE
if human shot there isn't a need to reset the gravity flag however if the alien is shot the flag will have to be reset



to make alien leave
when human shot

human shot switch alien to return to search height state
set its velo and  create the 2 new if else sections

remove human

must add an visible value to human to control if they are shown on the map or not

//reset the target to 0 so other aliens don't mistake the currently picked one with one dropped
//redundent here since it doesn't exist anymore but will be needed when the alien is the thing shot


