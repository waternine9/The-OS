#pragma once
#include <stdint.h>
#include "kernel_constants.h"

// TODO: CHANGE SNAKE CASE TO PASCAL CASE!

typedef struct {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed)) vesa_vbe_mode_info;

extern vesa_vbe_mode_info VbeModeInfo;

// PS2 keyboard functions
extern char kernel_WaitForKey();
extern char kernel_GetKeyPressed();
extern char kernel_WaitForKeyNoRepeat();
extern char kernel_GetKeyPressedNoRepeat();

static char mapPS2toChar[] = { ESC_PS2_KEYCODE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE_PS2_KEYCODE, TAB_PS2_KEYCODE,
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', ENTER_PS2_KEYCODE, LEFT_CONTROL_PS2_KEYCODE, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
'K', 'L', ';', 0x27, '`', LEFT_SHIFT_PS2_KEYCODE, 0x5C, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',
RIGHT_SHIFT_PS2_KEYCODE, 0, LEFT_ALT_PS2_KEYCODE, SPACE_PS2_KEYCODE };

// PS2 helper functions
static char ps2tochar(char ps2in)
{
	return mapPS2toChar[ps2in - 1];
}

int KPrintf(const char *fmt, ...);
