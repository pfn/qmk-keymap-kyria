#include QMK_KEYBOARD_H
#include <string.h>
#include "keymap.h"

// RGB Matrix naming
#if defined(RGB_MATRIX_ENABLE)
#    include <rgb_matrix.h>

#    if defined(RGB_MATRIX_EFFECT)
#        undef RGB_MATRIX_EFFECT
#    endif  // defined(RGB_MATRIX_EFFECT)

#    define RGB_MATRIX_EFFECT(x) RGB_MATRIX_EFFECT_##x,
enum {
    RGB_MATRIX_EFFECT_NONE,
#    include "rgb_matrix_effects.inc"
#    undef RGB_MATRIX_EFFECT
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
};

#    define RGB_MATRIX_EFFECT(x)    \
        case RGB_MATRIX_EFFECT_##x: \
            return #x;
const char *rgb_matrix_name(uint8_t effect) {
    switch (effect) {
        case RGB_MATRIX_EFFECT_NONE:
            return "NONE";
#    include "rgb_matrix_effects.inc"
#    undef RGB_MATRIX_EFFECT
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
        default:
            return "UNKNOWN";
    }
}
#endif  // defined(RGB_MATRIX_ENABLE)

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

// taken from https://github.com/brickbots/qmk_firmware/blob/brickbots_dev/keyboards/kyria/keymaps/brickbots/keymap.c#L179
#ifdef WPM_ENABLE

static uint8_t max(uint8_t a, uint8_t b) {
    return a > b ? a : b;
}

static void render_wpm_graph(void) {
    static uint16_t wpm_graph_timer = 0;
    static uint8_t  zero_bar_count  = 0;
    static uint8_t  bar_count       = 0;
    uint8_t         bar_height      = 0;
    uint8_t         bar_segment     = 0;

    if (wpm_graph_timer == 0) {
        wpm_graph_timer = timer_read();
        return;
    }
    if (timer_elapsed(wpm_graph_timer) > 500) {
        wpm_graph_timer = timer_read();

        if (OLED_DISPLAY_HEIGHT == 64) bar_height = get_current_wpm() / 2;
        if (OLED_DISPLAY_HEIGHT == 32) bar_height = get_current_wpm() / 4;
        if (bar_height > OLED_DISPLAY_HEIGHT) bar_height = OLED_DISPLAY_HEIGHT;

        if (bar_height == 0) {
            // keep track of how many zero bars we have drawn.  If
            // there is a whole screen worth, turn the display off and
            // wait until there is something to do
            if (zero_bar_count > OLED_DISPLAY_WIDTH) {
                return;
            }
            zero_bar_count++;
        } else
            zero_bar_count = 0;

        oled_pan(false);
        bar_count++;
        for (uint8_t i = (OLED_DISPLAY_HEIGHT / 8); i > 0; i--) {
            if (bar_height > 7) {
                if (i % 2 == 1 && bar_count % 3 == 0)
                    bar_segment = 254;
                else
                    bar_segment = 255;
                bar_height -= 8;
            } else {
                switch (bar_height) {
                    case 0:
                        bar_segment = 0;
                        break;

                    case 1:
                        bar_segment = 128;
                        break;

                    case 2:
                        bar_segment = 192;
                        break;

                    case 3:
                        bar_segment = 224;
                        break;

                    case 4:
                        bar_segment = 240;
                        break;

                    case 5:
                        bar_segment = 248;
                        break;

                    case 6:
                        bar_segment = 252;
                        break;

                    case 7:
                        bar_segment = 254;
                        break;
                }
                bar_height = 0;

                if (i % 2 == 1 && bar_count % 3 == 0) bar_segment++;
            }
            oled_write_raw_byte(bar_segment, (i - 1) * OLED_DISPLAY_WIDTH);
        }
    }
}

// a rolling 65 second window of max wpm
static void render_wpm_status(void) {
    static uint8_t wpm_samples[65] = {0}; // 65 to match uint16_t max
    static uint8_t wpm_sample_bucket = 0;
    static uint16_t wpm_sample_timer = 0;
    static uint8_t max_wpm = 0;
    static uint8_t avg_wpm = 0;

    if (timer_elapsed(wpm_sample_timer) > 250) {
        wpm_sample_timer = timer_read();
        uint8_t bucket = (wpm_sample_timer / 1000) % 65;
        wpm_samples[bucket] = max(bucket == wpm_sample_bucket ? wpm_samples[bucket] : 0, get_current_wpm());
        wpm_sample_bucket = bucket;
        max_wpm = 0;
        uint16_t wpm_sum = 0;
        for (uint8_t i = 0, count = 0; i < 65; ++i) {
            max_wpm = max(max_wpm, wpm_samples[i]);
            wpm_sum += wpm_samples[i];
            if (wpm_samples[i] > 0) {
                ++count;
            }
            avg_wpm = wpm_sum / (count ? count : 1);
        }
    }

    render_wpm_graph();
    oled_write_P(PSTR("WPM: "), false);
    oled_write(get_u8_str(get_current_wpm(), ' '), false);
    oled_write_P(PSTR(" / "), false);
    oled_write(get_u8_str(avg_wpm, ' '), false);
    oled_write_P(PSTR(" / "), false);
    oled_write(get_u8_str(max_wpm, ' '), false);
}
#endif

void render_keyboard_status(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("QWERTY\n"), false);
            break;
        case _COLEMAK:
            oled_write_P(PSTR("COLEMAK\n"), false);
            break;
        case _NUM:
            oled_write_P(PSTR("Numpad\n"), false);
            break;
        case _SYM:
            oled_write_P(PSTR("Symbols\n"), false);
            break;
        case _NAV:
            oled_write_P(PSTR("Nav\n"), false);
            break;
        case _MOUSE:
            oled_write_P(PSTR("Mouse\n"), false);
            break;
        case _MEDIA:
            oled_write_P(PSTR("Media\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("RGB\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    // Write host Keyboard LED Status to OLEDs
    led_t led_usb_state = host_keyboard_led_state();
    oled_write_P(get_oneshot_layer() == 1 ? PSTR("NUM LOCK ") : PSTR("         "), false);
    oled_write_P(led_usb_state.caps_lock ? PSTR("CAPSLOCK ") : PSTR("         "), false);
    oled_write_P(PSTR("\n"), false);
    if (get_mods() != 0) {
        oled_write_P(PSTR("Mods:\n"), false);
        oled_write_P((get_mods() & MOD_MASK_SHIFT) ? PSTR("SHIFT ") : PSTR("      "), false);
        oled_write_P((get_mods() & MOD_MASK_CTRL) ? PSTR("CTRL ") : PSTR("     "), false);
        oled_write_P((get_mods() & MOD_MASK_ALT) ? PSTR("ALT ") : PSTR("    "), false);
        oled_write_P((get_mods() & MOD_MASK_GUI) ? PSTR("GUI") : PSTR("    "), false);
        oled_write_P(PSTR("\n"), false);
    } else {
        oled_write_P(PSTR("\n\n"), false);
    }
    oled_write_P(PSTR("\nRGB:\n"), false);
    char buf[20] = {0};
    memcpy(buf, rgb_matrix_name(rgb_matrix_get_mode()), 20);
    for (int i = 1; i < 20; ++i) {
        if (buf[i] == 0)
            break;
        else if (buf[i] == '_')
            buf[i] = ' ';
        else if (buf[i - 1] == ' ')
            buf[i] = buf[i] > 90 ? buf[i] - 32 : buf[i];
        else if (buf[i - 1] != ' ')
            buf[i] = buf[i] < 97 ? buf[i] + 32 : buf[i];
    }
    oled_write_ln(buf, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_keyboard_status();
    } else {
        #ifdef SPLIT_WPM_ENABLE
        render_wpm_status();
        #endif
    }
    return false;
}
#endif

#ifdef ENCODER_ENABLE
#define MAKE_KC(mod, tru, els) (mod ? (tru) : (els))

bool encoder_update_user(uint8_t index, bool clockwise) {

    bool is_shift = get_mods() & MOD_MASK_SHIFT;
    if (!index) {
        if (is_shift) {
            if (clockwise)
                rgb_matrix_step();
            else
                rgb_matrix_step_reverse();
        } else
            tap_code(clockwise ? KC_VOLU : KC_VOLD);
    } else {
        // Mouse wheel
        tap_code(MAKE_KC(is_shift, clockwise ? KC_WH_R : KC_WH_L, clockwise ? KC_WH_D : KC_WH_U));
    }
    return false;
}
#endif
