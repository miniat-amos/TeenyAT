#ifndef __EDISON_SPRITE_LOCATIONS_H__
#define __EDISON_SPRITE_LOCATIONS_H__


/* character LCD: 4 rows of 20 */
const int LOC_LCD_TL[2] = {60, 89};

/* Buttons on Port A: 2 rows of 4 */
const int LOC_BUTTONS_PORTA_TL[2] = {48, 533};

/* Left buzzer: just 1 */
const int LOC_BUZZER_LEFT_TL[2] = {363, 520};

/* Right buzzer: just 1 */
const int LOC_BUZZER_RIGHT_TL[2] = {440, 520};

/* LEDs Port A: 4 rows of 4 */
const int LOC_LEDS_PORTA_TL[2] = {392, 359};

/* LEDs Port B: 4 rows of 4 */
const int LOC_LEDS_PORTB_TL[2] = {392, 409};

/* DIPs Port A: 1 row of 4 */
const int LOC_DIPS_PORTA_TL[2] = {227, 543};

/* 7-Seg (Port A) DIPs (system only): 1 row of 2 */
const int LOC_7SEG_DIPS_PORTA_TL[2] = {247, 350};

/* 7-Seg (Port B) DIPs (system only): 1 row of 2 */
const int LOC_7SEG_DIPS_PORTB_TL[2] = {247, 412};

/* Left Fader, top-left and bottom-right: just 1 fader */
const int LOC_FADER_LEFT_TL_BR[2][2] = {{556, 397}, {576, 533}};

/* Rightt Fader, top-left and bottom-right: just 1 fader */
const int LOC_FADER_RIGHT_TL_BR[2][2] = {{604, 397}, {624, 533}};

/* Pause button estimate location top-left */
const int LOC_BUTTONS_PAUSE[2] = {743, 411};

/* Reset button estimate location top-left */
const int LOC_BUTTONS_RESET[2] = {679, 411};

/* 7-Seg Display (Port A) : 1 row of 4 */
const int LOC_7SEG_PORTA_TL[4][2] = {{42, 353}, {92, 353}, {142,353}, {192,353}};

/* 7-Seg Display (Port B) : 1 row of 4 */
const int LOC_7SEG_PORTB_TL[4][2] = {{42, 416}, {92, 416}, {142,416}, {192,416}};

/*
 * 7-Seg hex (and thereby decimal) row:col pairs indexing
 * into the positions of the seven segment sprite sheet.
 * These are NOT the pixel offsets into the sprite sheet.
 */
const int INDEX_7SEG_HEX[16][2] = {
    {3, 15},  /* 0 */
    {0, 6},   /* 1 */
    {5, 11},  /* 2 */
    {4, 15},  /* 3 */
    {6, 6},   /* 4 */
    {6, 13},  /* 5 */
    {7, 13},  /* 6 */
    {0, 7},   /* 7 */
    {7, 15},  /* 8 */
    {6, 7},   /* 9 */
    {7, 7},   /* A */
    {7, 12},  /* B */
    {3, 9},   /* C */
    {5, 14},  /* D */
    {7, 9},   /* E */
    {7, 1}    /* F */
};

#endif /* __EDISON_SPRITE_LOCATIONS_H__ */