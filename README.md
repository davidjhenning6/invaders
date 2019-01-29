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




