
# Petris: A game for the Game Boy Color
Build your favorite pets in all shapes and sizes, keep ahead of dropping pieces and make it to the next level!

Petris is a new home-brew 2D puzzle game built for the Game Boy Color (CGB). It will run on original hardware (CGB and GBA) and in emulators.

#### Running under Emulation
If you run the "Cartridge" build of Petris in an emulator then you may want to turn on "inter-frame blending" to fix potential flicker on the title screen. If that is not an option on your emulator you can to run the "Web" build instead.


#### Game Play Controls
* **B/A**: Rotate Left/Right
* **Down**: Move piece down faster
* **Left/Right**: Move piece Left/Right

- **Start**: Pause
- **Select**: Show length of recent pets
- **Select + Up**: Change High Contrast mode setting


#### Game types
##### Long Pet
- Complete progressively longer pets to reach the next level.
- Board clears at the start of each level

##### Crunch-up
- Keep ahead as the screen crunches upward and new pets appear
- Complete 20 pets to reach the next level
- Board clears at the start of each level
- Note: No L-corner pieces will appear in this level

##### Tail Cleanup
- Clear all tails off the screen to reach the next level
- Board is cleared and filled with tails at the start of each level
- Note: No tail pieces will drop in this level

##### Level Up
- Classic level-up style
- Complete 20 pets to reach the next level
- Board clears at the start of each level

##### Marathon
- For the long distance player
- Board does not clear on level change


#### Head-to-Head 2 Player
- Two players can compete over Game Boy Link Cable (on CGB and GBA) in all game types.
- Send crunch-ups to your opponent when you complete pets and on level-up.
- **One crunch-up is sent for every 4 tiles in a completed pet.**
- The last player standing wins the match.
- Recommended game type: Crunch-up
- **To start 2 Player**
  1. Go to the options screen *(both players)*
  1. Connect link cable *(both players)*
  1. When the link is detected a "2P" icon will appear in the upper-left of the screens
  1. Difficulty: Players may choose different difficulty settings
  1. Music: It is recommended for only *one* player to have music turned on
  1. One of the players should:
     1. Set *"2 Player VS"* to *"ON"*
     1. Choose a game type
     1. Press "Start" to begin the match


#### Difficulty Setting
There are difficulty levels for all players, from relaxing to full-tilt speed.
 * Easy / Normal / Hard / Expert / Beast
 * Note: Next piece preview is hidden on the Beast difficulty level

#### Other Options and features:
 * Choose from multiple sound tracks
 * High Contrast options for small and non-backlit screens
 * Visual hinting for drop location and special pieces can be turned on/off
 * Game background colors will fade to a different palette every 5 levels

#### Special Pieces
##### Bomb
- Icon: ![Special: Bomb icon][image_special_bomb]
- When landed the bomb removes any adjacent pieces (left/right/above/below)
- Pet length required to obtain a bomb (by difficulty level):
  - Easy: 5
  - Normal: 6
  - Hard: 7
  - Expert: 8
  - Beast: 7
##### 4-Way Merge
- Icon: ![Special: Merge icon][image_special_merge]
- When landed the 4-way merge will complete and remove any connected pets and pieces regardless of whether their color/pet type matches. As long as the body segments connect they will be followed and removed. (left/right/above/below)
- No points or pet completion credit is given for pets removed by the 4-way merge, however removal of tails does count in the Tail Cleanup game type.
- Merge pieces appear automatically after a given number of pieces have been played:
  - Easy: 32
  - Normal: 64
  - Hard: 64
  - Expert: 128
  - Beast: 128

#### Difficulty Level Point Bonus
- Easy: 1x
- Normal: 1x
- Hard: 2x
- Expert: 3x
- Beast: 10x


#### Speed by Difficulty Level

    FpD:   Frames per Drop
    Inc:   Decrease FpD once every Inc Levels
    Msec:  Msec between Drop
    % Max: Speed % of max
    EZ:    Easy
    NM:    Normal
    HD:    Hard
    EX:    Expert
    BT:    Beast

    FpD Inc Msec    % Max   EZ  NM  HD  EX  BT
    60  1   1000    5       1
    45  1   750     7       10  1
    30  1   500     10      20  10
    20  2   333     15      30  20
    15  2   250     20      40  30  1
    10  5   167     30          40  10
    8   0   133     38              20  1
    7   0   116     43              30  10
    6   0   100     50              40  20  1
    5   0   83      83                  30  10
    4   0   67      75                  40  20
    4   0   67      75                      30
    3   0   50      100                     40

[image_special_bomb]:
data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACgAAAAoAgMAAADxkFD+AAAADFBMVEVlLWcAAAA4ODj///+3cnGMAAAAAXRSTlMAQObYZgAAAENJREFUGNNjYA2FggAG+f9Q8IWhHsb8i8pctWoVsUwgDeYQw4SwVhHHXLUKzCaeuYr6TKgdxDGRvEmYiRnqSNGCiCwAFcg4pHSNWMQAAAAASUVORK5CYII=

[image_special_merge]:
data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACgAAAAoAgMAAADxkFD+AAAADFBMVEVlLWcAAAA4ODj///+3cnGMAAAAAXRSTlMAQObYZgAAAEFJREFUGNNjYA2FggAG+f9Q8IWhHsb8i8ZcBQTUZ4IZQEwkE6yROCbI4cQyiXcDUMt/hC8IMkkIEiyhjhQtiMgCAEiyN1AKA6GXAAAAAElFTkSuQmCC
