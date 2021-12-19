#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

// taken from https://github.com/brickbots/qmk_firmware/blob/brickbots_dev/keyboards/kyria/keymaps/brickbots/keymap.c#L179
#ifdef WPM_ENABLE
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
#    endif

void render_keyboard_status(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case 0:
            oled_write_P(PSTR("QWERTY\n"), false);
            break;
        case 1:
            oled_write_P(PSTR("Numpad\n"), false);
            break;
        case 2:
            oled_write_P(PSTR("Symbols\n"), false);
            break;
        case 3:
            oled_write_P(PSTR("Nav\n"), false);
            break;
        case 4:
            oled_write_P(PSTR("Mouse\n"), false);
            break;
        case 5:
            oled_write_P(PSTR("Media\n"), false);
            break;
        case 6:
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
    oled_write_P(PSTR("\nPerry Nguyen\npfnguyen@hanhuy.com\n(510) 473-7796"), false);
}

static uint8_t max(uint8_t a, uint8_t b) {
    return a > b ? a : b;
}

// a rolling 65 second window of max wpm
void render_wpm_status(void) {
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

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_keyboard_status();
    } else {
        render_wpm_status();
    }
    return false;
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {

    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_WH_U);
        } else {
            tap_code(KC_WH_D);
        }
    }
    return false;
}
#endif
