#pragma once

#include QMK_KEYBOARD_H

enum dead_key_state {
    DK_NONE = 0,
    DK_GRAVE, /* ` */
    DK_ACUTE, /* ' */
    DK_DIAER, /* " */
    DK_CIRC,  /* ^ */
    DK_TILDE  /* ~ */
};

/* Inizializzazione modulo dead keys */
void dead_keys_init(void);

/* Processamento eventi tastiera per le dead keys. Ritorna false se il tasto e' stato consumato */
bool dead_keys_process(uint16_t keycode, keyrecord_t *record, uint8_t current_layer);

/* Toggle tra modalita' US International (dead keys) e modalita' Android (Unicode escape sequences) */
void dead_keys_toggle_mode(void);
bool dead_keys_get_mode(void);

/* Funzioni per indicatore LED su tasto A (LED 13) */
bool dead_keys_is_flashing_led(void);
bool dead_keys_led_is_green(void);
void dead_keys_housekeeping(void);
