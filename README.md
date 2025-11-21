# Game Design Document
## Section 0: Summary
“The Floor is Lava” by Olivia Wang, Natalie Zhou, and Dillan Lau.

“The Floor is Lava” is a single player game where the player moves a water sprite to interact with obstacles including platforms, doors, and elevators. If the player successfully collects gems and escapes through the final door, the player wins! The game will be timed, so the player with the quickest moves and most gems wins the most!

We will also describe how the game works, the special features we implemented in our game, our development timeline, and disaster recovery when things go awry. 
## Section 1: Gameplay
The game will begin at a home screen that shows the three levels. The player will be able to press the 1, 2, or 3 key to choose the corresponding level. The player can only select level 2 once level 1 has been completed, and can only select level 3 once both level 1 and level 2 have been completed.

Once the player enters a level, there will be a timer at the top of the game screen that indicates the progress of the game. The faster the player completes the level, the better their final score will be! The final score (red, orange, or green gem) will be calculated using the number of gems the player collected and the amount of time the player took to complete the level. The player’s top score will then be displayed on the level select screen.

The player will navigate through a map filled with various obstacles: Level 1 - stationary obstacles only; Level 2 - stationary obstacles and doors; Level 3 - stationary obstacles, doors, and elevators. If the player successfully completes the level and enters the final door, the player wins! If the player falls into lava and dies, the player loses :(.

We will be implementing three map levels, so once the player succeeds, they can restart the game and try to beat their previous score, or move on to the next level. When the player returns to the homescreen, they will be able to choose which level to play.

The player will be able to pause the game, upon which they may restart the level or return to the home screen. 

The player will be able to control the movement of the sprite using the right, left, and up arrow keys. In order to move right, the player will press the right key. In order to move left, the player will move the left key. In order to jump on blocks, the player will press the up arrow key. 

Physics will be applied to the movement of the sprite, so the sprite will be able to jump and fall in a realistic manner. Furthermore, the player’s interaction with obstacle objects will be implemented such that when the sprite is standing on an elevator, both the sprite and the elevator will move together and when a button is pressed, it stays pressed down.
## Section 2: Feature Set
### Features
Priority numbers (1, 2, 3, 4) are in parentheses. 

**General Features**
- Initialization of the 3 maps (1)
    - Where the obstacle objects and blocks are located in the map
- Keyboard controls (1)
    - Controlling the sprite
        - Up is jumping, sprite is facing front
        - Left is moving left, sprite faces left
        - Right is moving left, sprite faces right
    - Selecting level during homepage
- Homepage (2)
- Timer (3)
- Restart, Pause, Return to Homepage (3)
- Point calculation based on gems and time (4)

**Physics**
- Player is able to walk and run around (with velocity and acceleration implemented) (1)
    - Player is also able to jump on blocks and fall off
- Player dies when landing in lava (1)
- Player can click buttons to lead to movements including…
    - Doors opening and closing (2)
    - Elevators lifting and lowering (3)

**Graphics** (2)
- Sprites (player, gems, blocks, levels)
    - Player sprite (left, right, and front)
- Lava and water animation (3-4 frames)
- Background

**Sounds**
- Constant background music (4)
- Sound effects (when collecting gems, jumping, etc.) (4)
