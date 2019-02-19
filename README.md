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



############################################ 5. human detection for aliens must be a circle ############################################
    --------------------Active--------------------
   

    ---------------------DONE---------------------



A3 discussion

purpose is to add aliens and ai to game
line added today ** have to update map to show aliens

aliens we will implement are called landers
ball shape with 3 legs and 2 black eyes

- we will make them out of cubes using the world array
- dont make it smaller than 3x3x3 cube allowed to make it look like anything bigger must be within reason

aliens mission

- must get above human and pick up
  - make it an odd number across so it picks up human in center not requirement just recommendation
  - make a function to draw lander 
    - drawLander(x, y, z)
  - will have to store location and behaviour

AI Control

- Agents
- if shoot alien drops human
  - if close to ground human fine if too high human die
- if human shot alien goes back to searching for prey
- AI statemachine → for aliens
  - useful for game AI
  - what state system is in and what states it can go too from that point
  - start condition place aliens anywhere in the map
  - search for human
  - pickup human
  - flyaway with human


- ai will always be in one of these states in game
  - must do this for each alien
  - they can all be doing different things
- store the state of the agents(along with x,y,z location)

step 1 

  place in random location
    - make sure they are high enough that they wont crash into the ground!

step 2 

  searching
  between + or - 1 in each direction randomly selected motion on the x & z directions
  store x and z for each agent
  range of motion will be adjusted after each frame(time update is entered)


  what happens when you run into the edge of the world
    if an alien is about to leave the world depending on which side its leaving you flip the sign on the vector( multiply by -1) so if alien x is going to be 101 and its x vector is +1 make it -1
    cubes are integer values so moving between -1 and +1 is just a suggestion by Calvert not a requirement
    dont worry about changing elevation
    *** if you want to add different elevation between the aliens you can but its not required*****
  nothing is required to be done if they collied
  - if you want you can make them bounce off each other when they collide

step 3

  picking up humans
  if it gets near** a human
    - near is an ambiguous word calvert is leaving this up to you what do you want the range to be?
    - moves in a straight line towards a human( moves in all 3 dimensions toward the human)
    - if you know beginning and end just divide it in to a reasonable number of steps  if difference is 5, 10, 5 divided by 5 the alien will move 1, 2, 1 each step
  we have to figure out what happens if more than one alien goes for the same human??
  alien range for detecting humans should be a circle not a square

step 4

  fly away with human
    - alien and human move up
    - if they reach top of world then remove from game → so alien will need a visible or not value
    - if alien shot - human falls to ground, move alien back to searching height 
    - human shot - remove human, move alien back to searching height 

Include a readme to explain any assumptions you’ve made about the assignment to make it easier for ta

- you don’t have to its just a thought!

make sure you look at calverts ascii diagram in a font where all the characters have the same proportion so it is properly read


- Update the Map
- --------------------
- 4. Draw the agents on the map. Update their position as they move.




1. draw aliens on the map

2. aliens searching
- search radius its like a cylinder

3. beeline to pick up human

4. grab human and fly away
- disappear with human when hit roof

5. if shot when holding human drop it and return to searching height

6. if human drops from a certain height than human disappears when it hits the ground


