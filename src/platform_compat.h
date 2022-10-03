// platform_compat.h

#ifndef PLATFORM_COMPAT_H
#define PLATFORM_COMPAT_H

#if defined(SEGA)
    #if defined(GAMEGEAR)
        #define VIEWPORT_HEIGHT DEVICE_SCREEN_HEIGHT
        #define VIEWPORT_WIDTH DEVICE_SCREEN_WIDTH
        #define VIEWPORT_X_OFS 0
        #define SPRITE_OFS_X (DEVICE_SPRITE_PX_OFFSET_X - 8)
        #define SPRITE_OFS_Y (DEVICE_SPRITE_PX_OFFSET_Y - 16)
    #elif defined(MASTERSYSTEM)
        #define VIEWPORT_HEIGHT 18
        #define VIEWPORT_WIDTH (DEVICE_SCREEN_WIDTH - 1)
        #define VIEWPORT_X_OFS 1
        #define SPRITE_OFS_X DEVICE_SPRITE_PX_OFFSET_X
        #define SPRITE_OFS_Y (DEVICE_SPRITE_PX_OFFSET_Y - 16)
    #endif
#elif defined(NINTENDO)
    #define VIEWPORT_HEIGHT DEVICE_SCREEN_HEIGHT
    #define VIEWPORT_WIDTH DEVICE_SCREEN_WIDTH
    #define VIEWPORT_X_OFS 0
    #define SPRITE_OFS_X (DEVICE_SPRITE_PX_OFFSET_X - 8)
    #define SPRITE_OFS_Y (DEVICE_SPRITE_PX_OFFSET_Y - 16)
    #define RAND_SOURCE DIV_REG
#endif

#if defined(SEGA)

    #define GG_TILE_ATTR_PAL_0 (uint16_t)0x0000u
    #define GG_TILE_ATTR_PAL_1 (uint16_t)0x0800u
    #define GG_TILE_ATTR_MSB_PAL_0 0x00u
    #define GG_TILE_ATTR_MSB_PAL_1 0x08u

    extern const uint16_t gg_pal_select[];

#elif defined(NINTENDO)
#endif


// Convert colors in 16 bit BGR:5:5:5 into platform specific colors
#ifdef NINTENDO
    #define CGB2PAL(BGR555) (BGR555) // Nothing to do
#elif defined(GAMEGEAR)
    // Truncate to BGR444
    #define CGB2PAL(BGR555) ( ((((BGR555) >> (10 + 1)) & 0x000FU) << 8) | ((((BGR555) >> (5  + 1)) & 0x000FU) << 4) | ((((BGR555) >> (0  + 1)) & 0x000FU) << 0) )
#endif

// Convert individual color components in 16 bit BGR:5:5:5 into platform specific colors
#ifdef NINTENDO
    #define CGB2COL(C555) (C555) // Nothing to do
#elif defined(GAMEGEAR)
    // Truncate to BGR444
    #define CGB2COL(C555) (((C555) >> 1) & 0x0F)
#endif



void set_sprite_data_pal_compat(uint8_t tiles_start, uint8_t tile_count, const uint8_t * p_tile_data, const uint8_t * p_tile_pal_list);
void set_bkg_data_pal_compat(uint8_t tiles_start, uint8_t tile_count, const uint8_t * p_tile_data, const uint8_t * p_tile_pal_list);
void set_bkg_tiles_attr_compat(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t * p_attr_map);

#endif