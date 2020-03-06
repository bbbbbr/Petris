// game_pieces.h


#define GP_EMPTY_MASK 0xE0 // .5+
#define GP_ROT_MASK   0x10 // .4
#define GP_PET_MASK   0x0C // .3-2
#define GP_SEG_MASK   0x03 // .1-0

#define GP_PET_UPSHIFT  2
#define GP_PET_DOG      0
#define GP_PET_CAT      1
#define GP_PET_FISH     2
#define GP_PET_SNAKE    3

#define GP_SEG_UPSHIFT  0
#define GP_SEG_TAIL     0
#define GP_SEG_TORSO    1
#define GP_SEG_TURN     2
#define GP_SEG_HEAD     3

#define GP_ROT_UPSHIFT  4
#define GP_ROT_HORZ     0
#define GP_ROT_VERT     1

// BG Map Attributes (CGB Mode only)
 // Bit 0-2  Background Palette number  (BGP0-7)
 // Bit 3    Tile VRAM Bank number      (0=Bank 0, 1=Bank 1)
 // Bit 4    Not used
 // Bit 5    Horizontal Flip            (0=Normal, 1=Mirror horizontally)
 // Bit 6    Vertical Flip              (0=Normal, 1=Mirror vertically)
 // Bit 7    BG-to-OAM Priority         (0=Use OAM priority bit, 1=BG Priority)