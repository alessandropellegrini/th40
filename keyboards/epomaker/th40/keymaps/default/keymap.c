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
#include "dead_keys.h"

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

enum custom_keycodes {
    TG_ANDR = SAFE_RANGE,
};

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
        KC_NO,   TG_ANDR, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_LBRC, KC_RBRC, KC_BSLS,            KC_NO,
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    /* Fallback per keycode underglow/backlight se inviati da VIA */
    if (keycode == UG_VALU || keycode == BL_UP) {
        rgb_matrix_increase_val();
        return false;
    }
    if (keycode == UG_VALD || keycode == BL_DOWN) {
        rgb_matrix_decrease_val();
        return false;
    }

    /* Ignora i modificatori puri, i cambi di layer, i tasti mouse e i tasti RGB */
    if ((keycode >= KC_LCTL && keycode <= KC_RGUI) ||
        (keycode >= QK_TO && keycode <= QK_PERSISTENT_DEF_LAYER_MAX) ||
        (keycode >= QK_LAYER_MOD && keycode <= QK_LAYER_MOD_MAX) ||
        IS_MOUSE_KEYCODE(keycode) ||
        IS_RGB_MATRIX_KEYCODE(keycode)) {
        return true;
    }

    uint8_t current_layer = get_highest_layer(layer_state);

    /* Toggle Android Unicode Mode on MO(1) + A */
    if (keycode == TG_ANDR || (current_layer == 1 && record->event.key.row == 2 && record->event.key.col == 1)) {
        dead_keys_toggle_mode();
        return false;
    }

    /* Gestione Dead Keys FSM (Android Unicode / US International) */
    if (!dead_keys_process(keycode, record, current_layer)) {
        return false;
    }

    return true;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (dead_keys_is_flashing_led()) {
        if (dead_keys_led_is_green()) {
            rgb_matrix_set_color(13, 0, 255, 0); /* Verde per modalita' Android */
        } else {
            rgb_matrix_set_color(13, 255, 0, 0); /* Rosso per modalita' US International */
        }
    }
    return true;
}

void housekeeping_task_user(void) {
    dead_keys_housekeeping();
}

void keyboard_post_init_user(void) {
    dead_keys_init();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    rgb_matrix_disable_noeeprom();
}

void eeconfig_init_user(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv(0, 0, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    rgb_matrix_disable();
}

