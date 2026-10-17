/* Copyright 2025 Carlos Eduardo de Paula <carlosedp@gmail.com>
 * Copyright 2025 EPOMAKER <https://github.com/Epomaker>
 * Copyright 2023 LiWenLiu <https://github.com/LiuLiuQMK>
 * Copyright 2021 QMK <https://github.com/qmk/qmk_firmware>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "rdmctmzt_common.h"

// Aliases for VIA lighting keycodes
#ifndef RGB_SPI
#    define RGB_SPI UG_SPDU
#endif
#ifndef RGB_SPD
#    define RGB_SPD UG_SPDD
#endif
#ifndef BL_INC
#    define BL_INC BL_UP
#endif
#ifndef BL_DEC
#    define BL_DEC BL_DOWN
#endif
#ifndef RGB_HUI
#    define RGB_HUI UG_HUEU
#endif
#ifndef RGB_HUD
#    define RGB_HUD UG_HUED
#endif
#ifndef RGB_SAI
#    define RGB_SAI UG_SATU
#endif
#ifndef RGB_SAD
#    define RGB_SAD UG_SATD
#endif
#ifndef RGB_VAI
#    define RGB_VAI RM_VALU
#endif
#ifndef RGB_VAD
#    define RGB_VAD RM_VALD
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_tkl_ansi(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,     KC_P,     KC_BSPC,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,               KC_ENT,
        KC_LSFT, KC_GRV,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,  MO(1),   KC_SPC,           KC_RALT,  MO(2),    KC_RCTL
    ),
    [1] = LAYOUT_tkl_ansi(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,     KC_DEL,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_LBRC, KC_RBRC, KC_BSLS,            KC_NO,
        KC_LSFT, KC_SLSH, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_EQL,  KC_SCLN, KC_QUOT,  KC_UP,    KC_RALT,
        KC_LCTL, KC_NO,   KC_LALT,                   QK_BAT,  KC_NO,   MO(3),            KC_LEFT,  KC_DOWN,  KC_RGHT
    ),
    [2] = LAYOUT_tkl_ansi(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,   KC_F11,   KC_F12,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   RGB_VAI,            RGB_RTOG,
        KC_NO,   MD_BLE1, MD_BLE2, MD_BLE3, MD_24G,  MD_USB,  KC_NO,   KC_NO,   KC_NO,   KC_NO,    RGB_VAD,  KC_RALT,
        KC_LCTL, KC_PSCR, KC_LALT,                   KC_NO,   KC_NO,   KC_NO,            KC_NO,    KC_NO,    KC_NO
    ),
    [3] = LAYOUT_tkl_ansi(
        KC_NO,   KC_NO,   KC_PGUP, KC_NO,   KC_NO,   KC_NO,   KC_6,    KC_NO,   KC_NO,   KC_NO,    KC_NO,    KC_INS,
        KC_NO,   KC_HOME, KC_PGDN, KC_END,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,              MS_BTN1,
        KC_LSFT, KC_GRV,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_QUOT,  MS_UP,    MS_BTN2,
        KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS, KC_TRNS, KC_TRNS,          MS_LEFT,  MS_DOWN,  MS_RGHT
    )
};
// clang-format on

// ===========================================================================
// US-International Dead Keys FSM (Android Generic.kcm compliant)
// ===========================================================================

enum dead_key_state {
    DK_NONE = 0,
    DK_GRAVE, /* ` */
    DK_ACUTE, /* ' */
    DK_DIAER, /* " */
    DK_CIRC,  /* ^ */
    DK_TILDE  /* ~ */
};

static enum dead_key_state dk_state = DK_NONE;

static void send_dead_literal(enum dead_key_state state) {
    switch (state) {
        case DK_GRAVE:
            tap_code_delay(KC_GRV, 15);
            break;
        case DK_ACUTE:
            tap_code_delay(KC_QUOT, 15);
            break;
        case DK_DIAER:
            tap_code16_delay(S(KC_QUOT), 15);
            break;
        case DK_CIRC:
            tap_code16_delay(S(KC_6), 15);
            break;
        case DK_TILDE:
            tap_code16_delay(S(KC_GRV), 15);
            break;
        default:
            break;
    }
    wait_ms(20);
}

static bool is_valid_dead_target(enum dead_key_state state, uint16_t keycode) {
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

static void trigger_android_kcm_deadkey(enum dead_key_state state) {
    switch (state) {
        case DK_GRAVE:
            tap_code16_delay(LALT(KC_GRV), 15); /* Generic.kcm: \u0300 */
            break;
        case DK_ACUTE:
            tap_code16_delay(LALT(KC_E), 15);   /* Generic.kcm: \u0301 */
            break;
        case DK_CIRC:
            tap_code16_delay(LALT(KC_I), 15);   /* Generic.kcm: \u0302 */
            break;
        case DK_TILDE:
            tap_code16_delay(LALT(KC_N), 15);   /* Generic.kcm: \u0303 */
            break;
        case DK_DIAER:
            tap_code16_delay(LALT(KC_U), 15);   /* Generic.kcm: \u0308 */
            break;
        default:
            break;
    }
    wait_ms(20);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    /* Fallback per keycode underglow/backlight se inviati da VIA */
    if (keycode == UG_VALU || keycode == BL_UP) {
        if (record->event.pressed) {
            rgb_matrix_increase_val();
        }
        return false;
    }
    if (keycode == UG_VALD || keycode == BL_DOWN) {
        if (record->event.pressed) {
            rgb_matrix_decrease_val();
        }
        return false;
    }

    /* Ignora i modificatori puri, i cambi di layer, i tasti mouse e i tasti RGB per non resettare lo stato */
    if ((keycode >= KC_LCTL && keycode <= KC_RGUI) ||
        (keycode >= QK_TO && keycode <= QK_PERSISTENT_DEF_LAYER_MAX) ||
        (keycode >= QK_LAYER_MOD && keycode <= QK_LAYER_MOD_MAX) ||
        IS_MOUSE_KEYCODE(keycode) ||
        IS_RGB_MATRIX_KEYCODE(keycode)) {
        return true;
    }

    uint8_t mods = get_mods();
    uint8_t weak_mods = get_weak_mods();
    bool shifted = (mods | weak_mods) & MOD_MASK_SHIFT;
    bool alt_ctrl_gui = (mods | weak_mods) & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI);

    uint8_t current_layer = get_highest_layer(layer_state);

    /* 1. Rilevamento dell'innesco di una dead key sul Layer 3 quando lo stato e' neutro */
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

    /* 2. Risoluzione dello stato quando una dead key e' gia' in attesa */
    if (dk_state != DK_NONE) {
        /* Se l'utente preme una vocale tenendo ancora premuto fn + space (Layer 2),
         * rimappa il tasto al codice alfabetico per evitare di innescare MD_BLE1 su A o MD_USB su G */
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

        /* Spazio dopo dead key -> emette solo il carattere letterale */
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

        /* Backspace o Esc -> annulla la dead key in attesa */
        if (keycode == KC_BSPC || keycode == KC_ESC) {
            return false;
        }

        /* Vocale/carattere compatibile -> invia la sequenza Alt di Generic.kcm + tasto */
        if (!alt_ctrl_gui && is_valid_dead_target(current_dk, keycode)) {
            del_mods(MOD_MASK_SHIFT);
            del_weak_mods(MOD_MASK_SHIFT);
            send_keyboard_report();

            trigger_android_kcm_deadkey(current_dk);

            set_mods(mods);
            set_weak_mods(weak_mods);
            send_keyboard_report();

            /* Se l'utente e' ancora su un layer secondario (es. Fn+Space premuto),
             * emette la vocale esplicitamente e consuma l'evento per non attivare il comando del layer */
            if (current_layer != 0) {
                if (shifted) {
                    tap_code16(S(keycode));
                } else {
                    tap_code(keycode);
                }
                return false;
            }

            /* Se siamo gia' su Layer 0, lascia proseguire QMK normalmente */
            return true;
        }

        /* Caso speciale US-Intl: ' + c = ç */
        if (!alt_ctrl_gui && current_dk == DK_ACUTE && keycode == KC_C) {
            tap_code16(shifted ? LALT(S(KC_C)) : LALT(KC_C));
            return false;
        }

        /* Qualsiasi altro tasto (es. consonante) -> emette il simbolo + il tasto premuto */
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

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    rgb_matrix_disable_noeeprom();
}

void eeconfig_init_user(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv(0, 0, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    rgb_matrix_disable();
}
