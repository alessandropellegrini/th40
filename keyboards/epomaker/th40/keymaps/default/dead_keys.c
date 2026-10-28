#include "dead_keys.h"

#define A_LED_INDEX 13
#define A_LED_FLASH_DURATION 500 /* Durata flash in ms */

static enum dead_key_state dk_state = DK_NONE;
static bool android_mode = false; /* Inizialmente in modalita' US International */

static uint16_t a_led_flash_timer = 0;
static bool a_led_color_is_green = false;

void dead_keys_init(void) {
    dk_state = DK_NONE;
    android_mode = false;
    a_led_flash_timer = 0;
}

void dead_keys_toggle_mode(void) {
    android_mode = !android_mode;
    a_led_color_is_green = android_mode;
    a_led_flash_timer = timer_read();

    if (!rgb_matrix_is_enabled()) {
        rgb_matrix_set_color(A_LED_INDEX, a_led_color_is_green ? 0 : 255, a_led_color_is_green ? 255 : 0, 0);
        rgb_matrix_update_pwm_buffers();
    }
}

bool dead_keys_get_mode(void) {
    return android_mode;
}

bool dead_keys_is_flashing_led(void) {
    if (a_led_flash_timer != 0) {
        if (timer_elapsed(a_led_flash_timer) < A_LED_FLASH_DURATION) {
            return true;
        }
        a_led_flash_timer = 0;
    }
    return false;
}

bool dead_keys_led_is_green(void) {
    return a_led_color_is_green;
}

void dead_keys_housekeeping(void) {
    if (a_led_flash_timer != 0 && timer_elapsed(a_led_flash_timer) >= A_LED_FLASH_DURATION) {
        a_led_flash_timer = 0;
        if (!rgb_matrix_is_enabled()) {
            rgb_matrix_set_color(A_LED_INDEX, 0, 0, 0);
            rgb_matrix_update_pwm_buffers();
        }
    }
}

static void send_hex_digit(char c) {
    uint8_t kc = KC_NO;
    if (c == '0') {
        kc = KC_0;
    } else if (c >= '1' && c <= '9') {
        kc = KC_1 + (c - '1');
    } else if (c >= 'a' && c <= 'f') {
        kc = KC_A + (c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        kc = KC_A + (c - 'A');
    }
    if (kc != KC_NO) {
        tap_code_delay(kc, 2);
    }
}

static void send_unicode_ibus(const char *hex) {
    uint8_t saved_mods = get_mods();
    uint8_t saved_weak_mods = get_weak_mods();
    clear_mods();
    clear_weak_mods();
    send_keyboard_report();

    tap_code16_delay(LCTL(LSFT(KC_U)), 4);
    wait_ms(4);

    while (*hex) {
        send_hex_digit(*hex++);
    }

    wait_ms(4);
    tap_code_delay(KC_SPC, 4);

    set_mods(saved_mods);
    set_weak_mods(saved_weak_mods);
    send_keyboard_report();
}

void dead_keys_send_super_a(void) {
    if (android_mode) {
        send_unicode_ibus("1d43");
    } else {
        uint8_t saved_mods = get_mods();
        uint8_t saved_weak_mods = get_weak_mods();
        clear_mods();
        clear_weak_mods();
        send_keyboard_report();

        tap_code16_delay(LALT(KC_9), 10);

        set_mods(saved_mods);
        set_weak_mods(saved_weak_mods);
        send_keyboard_report();
    }
}

void dead_keys_send_degree(void) {
    if (android_mode) {
        send_unicode_ibus("00b0");
    } else {
        uint8_t saved_mods = get_mods();
        uint8_t saved_weak_mods = get_weak_mods();
        clear_mods();
        clear_weak_mods();
        send_keyboard_report();

        tap_code16_delay(LALT(S(KC_8)), 10);

        set_mods(saved_mods);
        set_weak_mods(saved_weak_mods);
        send_keyboard_report();
    }
}

static void send_dead_literal(enum dead_key_state state) {
    switch (state) {
        case DK_GRAVE:
            tap_code_delay(KC_GRV, 10);
            break;
        case DK_ACUTE:
            tap_code_delay(KC_QUOT, 10);
            break;
        case DK_DIAER:
            tap_code16_delay(S(KC_QUOT), 10);
            break;
        case DK_CIRC:
            tap_code16_delay(S(KC_6), 10);
            break;
        case DK_TILDE:
            tap_code16_delay(S(KC_GRV), 10);
            break;
        default:
            break;
    }
    wait_ms(15);
}

static void trigger_us_intl_deadkey(enum dead_key_state state) {
    switch (state) {
        case DK_GRAVE:
            tap_code16_delay(LALT(KC_GRV), 10);
            break;
        case DK_ACUTE:
            tap_code16_delay(LALT(KC_E), 10);
            break;
        case DK_CIRC:
            tap_code16_delay(LALT(KC_I), 10);
            break;
        case DK_TILDE:
            tap_code16_delay(LALT(KC_N), 10);
            break;
        case DK_DIAER:
            tap_code16_delay(LALT(KC_U), 10);
            break;
        default:
            break;
    }
    wait_ms(15);
}

static const char* get_unicode_hex(enum dead_key_state dk, uint16_t keycode, bool is_capital) {
    switch (dk) {
        case DK_GRAVE:
            switch (keycode) {
                case KC_A: return is_capital ? "00c0" : "00e0"; /* À / à */
                case KC_E: return is_capital ? "00c8" : "00e8"; /* È / è */
                case KC_I: return is_capital ? "00cc" : "00ec"; /* Ì / ì */
                case KC_O: return is_capital ? "00d2" : "00f2"; /* Ò / ò */
                case KC_U: return is_capital ? "00d9" : "00f9"; /* Ù / ù */
                default: return NULL;
            }
        case DK_ACUTE:
            switch (keycode) {
                case KC_A: return is_capital ? "00c1" : "00e1"; /* Á / á */
                case KC_E: return is_capital ? "00c9" : "00e9"; /* É / é */
                case KC_I: return is_capital ? "00cd" : "00ed"; /* Í / í */
                case KC_O: return is_capital ? "00d3" : "00f3"; /* Ó / ó */
                case KC_U: return is_capital ? "00da" : "00fa"; /* Ú / ú */
                case KC_C: return is_capital ? "00c7" : "00e7"; /* Ç / ç */
                default: return NULL;
            }
        case DK_CIRC:
            switch (keycode) {
                case KC_A: return is_capital ? "00c2" : "00e2"; /* Â / â */
                case KC_E: return is_capital ? "00ca" : "00ea"; /* Ê / ê */
                case KC_I: return is_capital ? "00ce" : "00ee"; /* Î / î */
                case KC_O: return is_capital ? "00d4" : "00f4"; /* Ô / ô */
                case KC_U: return is_capital ? "00db" : "00fb"; /* Û / û */
                default: return NULL;
            }
        case DK_TILDE:
            switch (keycode) {
                case KC_N: return is_capital ? "00d1" : "00f1"; /* Ñ / ñ */
                case KC_A: return is_capital ? "00c3" : "00e3"; /* Ã / ã */
                case KC_O: return is_capital ? "00d5" : "00f5"; /* Õ / õ */
                default: return NULL;
            }
        case DK_DIAER:
            switch (keycode) {
                case KC_A: return is_capital ? "00c4" : "00e4"; /* Ä / ä */
                case KC_E: return is_capital ? "00cb" : "00eb"; /* Ë / ë */
                case KC_I: return is_capital ? "00cf" : "00ef"; /* Ï / ï */
                case KC_O: return is_capital ? "00d6" : "00f6"; /* Ö / ö */
                case KC_U: return is_capital ? "00dc" : "00fc"; /* Ü / ü */
                case KC_Y: return is_capital ? "0178" : "00ff"; /* Ÿ / ÿ */
                default: return NULL;
            }
        default:
            return NULL;
    }
}

static bool is_valid_us_intl_dead_target(enum dead_key_state state, uint16_t keycode) {
    switch (state) {
        case DK_GRAVE:
        case DK_ACUTE:
        case DK_CIRC:
            return (keycode == KC_A || keycode == KC_E ||
                    keycode == KC_I || keycode == KC_O ||
                    keycode == KC_U);
        case DK_DIAER:
            return (keycode == KC_A || keycode == KC_E ||
                    keycode == KC_I || keycode == KC_O ||
                    keycode == KC_U || keycode == KC_Y);
        case DK_TILDE:
            return (keycode == KC_A || keycode == KC_N ||
                    keycode == KC_O);
        default:
            return false;
    }
}

bool dead_keys_process(uint16_t keycode, keyrecord_t *record, uint8_t current_layer) {
    if (!record->event.pressed) {
        return true;
    }

    uint8_t mods = get_mods();
    uint8_t weak_mods = get_weak_mods();
    bool shifted = (mods | weak_mods) & MOD_MASK_SHIFT;
    bool alt_ctrl_gui = (mods | weak_mods) & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI);
    bool caps_on = host_keyboard_led_state().caps_lock;
    bool is_capital = shifted ^ caps_on;

    /* 1. Rilevamento innesco dead key sul Layer 3 quando lo stato e' neutro */
    if (dk_state == DK_NONE && !alt_ctrl_gui && current_layer == 3) {
        if (keycode == KC_GRV) {
            dk_state = shifted ? DK_TILDE : DK_GRAVE;
            return false;
        }
        if (keycode == KC_QUOT) {
            dk_state = shifted ? DK_DIAER : DK_ACUTE;
            return false;
        }
        if (keycode == KC_6) {
            dk_state = DK_CIRC;
            return false;
        }
    }

    /* 2. Risoluzione dello stato quando una dead key e' in attesa */
    if (dk_state != DK_NONE) {
        /* Se l'utente preme una vocale tenendo ancora premuto fn + space,
         * rimappa al codice alfabetico per non attivare il comando del layer */
        if (record->event.key.row == 2 && record->event.key.col == 1) {
            keycode = KC_A;
        } else if (record->event.key.row == 1 && record->event.key.col == 3) {
            keycode = KC_E;
        } else if (record->event.key.row == 1 && record->event.key.col == 8) {
            keycode = KC_I;
        } else if (record->event.key.row == 1 && record->event.key.col == 9) {
            keycode = KC_O;
        } else if (record->event.key.row == 1 && record->event.key.col == 7) {
            keycode = KC_U;
        } else if (record->event.key.row == 3 && record->event.key.col == 4) {
            keycode = KC_C;
        } else if (record->event.key.row == 3 && record->event.key.col == 7) {
            keycode = KC_N;
        } else if (record->event.key.row == 1 && record->event.key.col == 6) {
            keycode = KC_Y;
        }

        enum dead_key_state current_dk = dk_state;
        dk_state = DK_NONE;

        /* Spazio -> emette solo il carattere letterale */
        if (keycode == KC_SPC) {
            del_mods(MOD_MASK_SHIFT);
            del_weak_mods(MOD_MASK_SHIFT);
            send_keyboard_report();
            send_dead_literal(current_dk);
            set_mods(mods);
            set_weak_mods(weak_mods);
            send_keyboard_report();
            return false;
        }

        /* Backspace o Esc -> annulla la dead key */
        if (keycode == KC_BSPC || keycode == KC_ESC) {
            return false;
        }

        /* MODALITA' ANDROID (Unicode escape sequences) */
        if (android_mode && !alt_ctrl_gui) {
            const char *hex = get_unicode_hex(current_dk, keycode, is_capital);
            if (hex != NULL) {
                send_unicode_ibus(hex);
                return false;
            }
        }

        /* MODALITA' US INTERNATIONAL (Dead keys) */
        if (!android_mode && !alt_ctrl_gui) {
            if (current_dk == DK_ACUTE && keycode == KC_C) {
                del_mods(MOD_MASK_SHIFT);
                del_weak_mods(MOD_MASK_SHIFT);
                send_keyboard_report();
                tap_code16(is_capital ? LALT(S(KC_C)) : LALT(KC_C));
                set_mods(mods);
                set_weak_mods(weak_mods);
                send_keyboard_report();
                return false;
            }

            if (is_valid_us_intl_dead_target(current_dk, keycode)) {
                del_mods(MOD_MASK_SHIFT);
                del_weak_mods(MOD_MASK_SHIFT);
                send_keyboard_report();

                trigger_us_intl_deadkey(current_dk);

                set_mods(mods);
                set_weak_mods(weak_mods);
                send_keyboard_report();

                if (current_layer != 0) {
                    if (shifted) {
                        tap_code16(S(keycode));
                    } else {
                        tap_code(keycode);
                    }
                    return false;
                }
                return true;
            }
        }

        /* Fallback (es. consonante non accentabile) -> emette dead key + tasto */
        del_mods(MOD_MASK_SHIFT);
        del_weak_mods(MOD_MASK_SHIFT);
        send_keyboard_report();
        send_dead_literal(current_dk);
        set_mods(mods);
        set_weak_mods(weak_mods);
        send_keyboard_report();

        if (current_layer != 0) {
            if (shifted) {
                tap_code16(S(keycode));
            } else {
                tap_code(keycode);
            }
            return false;
        }
        return true;
    }

    return true;
}
