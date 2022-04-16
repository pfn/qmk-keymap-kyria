#include QMK_KEYBOARD_H

#ifdef RGB_MATRIX_ENABLE
#include "process_keymap.h"
#include "keymap.h"

void matrix_init_user(void) {
    numpad_layer = _NUM;
    sym_layer = _SYM;
    mouse_layer = _MOUSE;
    // Remap under glow LEDs to nearby keys
    g_led_config = (led_config_t) {
        {
            { NO_LED, NO_LED, 4,      5,      5,      6,      6,      10     },
            { NO_LED, NO_LED, 4,      5,      5,      6,      6,      10     },
            { 1     , 1,      2,      3,      8,      7,      7,      9      },
            { 1     , 1     , 2,      3,      8,      NO_LED, NO_LED, NO_LED },
            { NO_LED, NO_LED, 14,     15,     15,     16,     16,     20     },
            { NO_LED, NO_LED, 14,     15,     15,     16,     16,     20     },
            { 11,     11,     12,     13,     18,     17,     17,     19     },
            { 11,     11,     12,     13,     18,     NO_LED, NO_LED, NO_LED },
        },
        {
            {91,40},{77,40},{63,40},{77,24},{63,8},{21,8},{21,40},{49,40},{7,40},{7,8},
            {133,40},{147,40},{161,40},{148,24},{161,8},{203,8},{203,40},{175,40},{217,40},{217,8}
        },
        {
            255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255
        }
    };
}
#endif
