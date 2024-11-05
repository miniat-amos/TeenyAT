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
const int LOC_FADER_LEFT_TL_BR[2][2] = {{556, 397}, {576, 576}};

/* Rightt Fader, top-left and bottom-right: just 1 fader */
const int LOC_FADER_RIGHT_TL_BR[2][2] = {{604, 397}, {624, 576}};

/* Pause button estimate location top-left */
const int LOC_BUTTONS_PAUSE[2] = {743, 411};

/* Reset button estimate location top-left */
const int LOC_BUTTONS_RESET[2] = {679, 411};



#endif /* __EDISON_SPRITE_LOCATIONS_H__ */