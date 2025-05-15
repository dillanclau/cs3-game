# Game Design Document
## Section 0: Summary
“The Floor is Lava” by Olivia Wang, Natalie Zhou, and Dillan Lau.

“The Floor is Lava” is a single player game where the player moves a water sprite to interact with obstacles including platforms, doors, and elevators. If the player successfully collects gems and escapes through the final door, the player wins! The game will be timed, so the player with the quickest moves and most gems wins the most!

We will also describe how the game works, the special features we implemented in our game, our development timeline, and disaster recovery when things go awry. 
## Section 1: Gameplay
There will be a timer at the top of the game screen that indicates the progress of the game. The faster the player completes the level, the better their final score will be! The final score (red, orange, or green gem) will be calculated using the number of gems the player collected and the amount of time the player took to complete the level.

The player will navigate through a map filled with platform, elevator, and door obstacles. If the player successfully completes the level and enters the final door, the player wins! If the player falls into lava and dies, the player loses :(.

We will be implementing one map level, so once the player succeeds, they can restart the game and try to beat their previous score. 

The player will be able to control the movement of the sprite using the right, left, and up arrow keys. In order to move right, the player will press the right key. In order to move left, the player will move the left key. In order to jump on blocks, the player will press the up arrow key. 

Physics will be applied to the movement of the sprite, so the sprite will be able to jump and fall in a realistic manner. Furthermore, the player’s interaction with obstacle objects will be implemented such that when the sprite is standing on an elevator, both the sprite and the elevator will move together and when a button is pressed, it stays pressed down.

## Section 2: Feature Set
### Features
Priority numbers (1, 2, 3, 4) are in parentheses. 

**General Features**
- Initialization of the map (1)
- Where the obstacle objets and blocks are located in the map
- Keyboard controls (1)
- Up is jumping, sprite is facing front
    - Left is moving left, sprite faces left
    - Right is moving left, sprite faces right
- Timer (2)
- Pause (2)
- Point calculation based on gems and time (3)

**Physics**
- Player is able to walk and run around (with velocity and acceleration implemented) (1)
    - Player is also able to jump on blocks and fall off
- Player dies when landing in lava (1)
- Player can click buttons to lead to movements including…
    - Doors opening and closing (2)
    - Elevators lifting and lowering (3)

**Graphics**
- Sprites (player, gems, blocks, levels) (1)
    - Player sprite (left, right, and front)
- Lava and water animation (3)
- Background (4)

**Sounds**
- Constant background music (4)
- Sound effects (when collecting gems, jumping, etc.) (4)

### Features Assigned to Each Person

**Dillan:**
1 - Keyboard controls \\
1 - Lava leading to dying (implement the game over function) \\
2 - Timer \\
3 - Point calculation with gems and time \\
4 - Sound effects \\

**Olivia:** 
1 - Initializing the map \\
1 - Designing sprites for player (left, right, and front), gems, blocks, and levels \\
2 - Implementing pause in the game \\
3 - Moving elevator when map button is clicked \\
4 - Graphics for game background \\

**Natalie:**
1 - Walking and running (velocity + acceleration) \\
2 - Implementing doors moving when map button is clicked \\
3 - Water animation\\
4 - Constant background music\\

## Section 3: Timeline
**Week of 5/12 - 5/18**
- Planning of game design
= Writing up game design document
= Splitting up the development features to work on

**Week of 5/19 - 5/25**
- Starting to implement feature 1
    - Lava leading to dying
    - Keyboard controls
    - Initializing the map
    - Designing sprites 
    - Walking and running 

**Week of 5/26 - 6/1**
- Making sure the basic controls work and implemented features 1 work
- Making sure the physics works and is as accurate as possible
- Implementing feature 2
    - Timer
    - Implementing pause
    - Implementing doors 

**Week of 6/2 - 6/8**
- Implementing feature 3
    - Point calculation
    - Moving elevator
    - Water animation
- Implementing feature 4
    - Sound effects
    - Graphics for game background
    - Constant background music

**Week of 6/9 - 6/10**
- Testing the game and making sure it works
- Making the game look nice
- Submitting the game!

## Section 4: Disaster Recovery
**Dillan:** Dillan will understand that he is falling behind and will not overload himself with other tasks. He will ensure that he properly communicates with his teammates about the best way to go forward. Additionally, as a good friend of the head TA Maxwell Chen, Dillan will be especially motivated to go to Office Hours and ask for support from the TAs.

**Olivia:** Olivia will go to CS3 OH if she gets stuck on a certain feature and will effectively communicate with her team members if a certain implementation is difficult or ambitious. We will all meet regularly to make sure that everyone is on the same page. Olivia enjoys asking questions at OH to the TAs, Professor Blank, and Jedi.

**Natalie:** Natalie will also go to CS3 OH if she gets stuck on how to implement a feature and will aim to complete two features per week during the first week so that she has some extra time left over during the last week to make additional corrections and adjustments if needed. Natalie will effectively communicate her progress to her team members and ask for guidance/help if she has a difficult time deciding how to implement a feature.