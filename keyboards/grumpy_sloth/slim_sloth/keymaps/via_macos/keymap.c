/* Copyright 2023 Grumpy Sloth (@RonnyM82)
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
#include "via.h"

typedef union {
    uint32_t raw;
    struct {
        bool caps_lock_indicator   : 1;
        bool num_lock_indicator    : 1;
        bool scroll_lock_indicator : 1;
    };
} user_config_t;

user_config_t user_config;

void eeconfig_init_user(void) {
    user_config.raw                  = 0;
    user_config.caps_lock_indicator   = true;
    user_config.num_lock_indicator    = true;
    user_config.scroll_lock_indicator = true;
    eeconfig_update_user(user_config.raw);
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
}

enum indicator_value_id {
    id_caps_lock_indicator   = 1,
    id_num_lock_indicator    = 2,
    id_scroll_lock_indicator = 3,
};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);
    uint8_t *value_id   = &(data[2]);
    uint8_t *value_data = &(data[3]);

    if (*channel_id == id_custom_channel) {
        if (*command_id == id_custom_save) {
            return;
        }
        switch (*value_id) {
            case id_caps_lock_indicator:
                if (*command_id == id_custom_get_value) {
                    *value_data = user_config.caps_lock_indicator;
                } else if (*command_id == id_custom_set_value) {
                    user_config.caps_lock_indicator = *value_data;
                    eeconfig_update_user(user_config.raw);
                }
                break;
            case id_num_lock_indicator:
                if (*command_id == id_custom_get_value) {
                    *value_data = user_config.num_lock_indicator;
                } else if (*command_id == id_custom_set_value) {
                    user_config.num_lock_indicator = *value_data;
                    eeconfig_update_user(user_config.raw);
                }
                break;
            case id_scroll_lock_indicator:
                if (*command_id == id_custom_get_value) {
                    *value_data = user_config.scroll_lock_indicator;
                } else if (*command_id == id_custom_set_value) {
                    user_config.scroll_lock_indicator = *value_data;
                    eeconfig_update_user(user_config.raw);
                }
                break;
            default:
                *command_id = id_unhandled;
                break;
        }
        return;
    }

    *command_id = id_unhandled;
}

enum layers {
    _BL1,
    _BL2,
    _BL1_Fn,
    _BL2_Fn,
    _LAYERS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│ │PSc│Scr│Pse│ │M1 │M2 │M3 │M4 │
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘ └───┴───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Ins│Hom│PgU│ │Num│ / │ * │ - │
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤ ├───┼───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │Del│End│PgD│ │ 7 │ 8 │ 9 │   │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘ ├───┼───┼───┤ + │
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │               │ 4 │ 5 │ 6 │   │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤     ┌───┐     ├───┼───┼───┼───┤
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │     │ ↑ │     │ 1 │ 2 │ 3 │   │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐ ├───┴───┼───┤Ent│
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│Menu│Ctrl│ │ ← │ ↓ │ → │ │   0   │ . │   │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘ └───────┴───┴───┘
     */

    [_BL1] = LAYOUT_fullsize_ansi(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,      KC_F12,     KC_F13,      KC_F14,  KC_F15,     KC_F16,  KC_F17,    KC_F18,   KC_F19,

        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,      KC_BSPC,    MO(_LAYERS), KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS,   KC_PAST,  KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,     KC_BSLS,    KC_DEL,      KC_END,  KC_PGDN,    KC_P7,   KC_P8,     KC_P9,    KC_PPLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,              KC_ENT,                                       KC_P4,   KC_P5,     KC_P6,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,              KC_RSFT,                 KC_UP,               KC_P1,   KC_P2,     KC_P3,    KC_PENT,
        KC_LCTL, KC_LOPT, KC_LCMD,                            KC_SPC,                             KC_RCMD, KC_ROPT, MO(_BL1_Fn), KC_RCTL,    KC_LEFT,     KC_DOWN, KC_RGHT,    KC_P0,              KC_PDOT
    ),
    [_BL2] = LAYOUT_fullsize_ansi(
        KC_ESC,           KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, RM_VALD, RM_VALU, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD,     KC_VOLU,    SCMD(KC_5),  KC_F14,  KC_F15,     KC_F16,   KC_F17,   KC_F18,   KC_F19,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                                      _______,  _______,  _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                 _______,             _______,  _______,  _______,  _______,
        _______, _______, _______,                            _______,                            _______, _______, MO(_BL2_Fn), _______,    _______,     _______, _______,    _______,            _______
    ),
    [_BL1_Fn] = LAYOUT_fullsize_ansi(
        EE_CLR,           KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, RM_VALD, RM_VALU, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD,     KC_VOLU,    SCMD(KC_5),  _______, _______,    _______,  _______,  _______,  _______,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     RM_HUEU, RM_SATU,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     RM_HUED, RM_SATD,    RM_NEXT,  RM_SPDU,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                                      RM_PREV,  RM_SPDD,  _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                 _______,             RM_TOGG,  _______,  _______,  _______,
        _______, _______, _______,                            _______,                            _______, _______, _______,     _______,    _______,     _______, _______,    _______,            _______
    ),
    [_BL2_Fn] = LAYOUT_fullsize_ansi(
        EE_CLR,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,      KC_F12,     KC_F13,      _______, _______,    _______,  _______,  _______,  _______,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                                      _______,  _______,  _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                 _______,             _______,  _______,  _______,  _______,
        _______, _______, _______,                            _______,                            _______, _______, _______,     _______,    _______,     _______, _______,    _______,            _______
    ),
    [_LAYERS] = LAYOUT_fullsize_ansi(
        _______,          TO(_BL1),TO(_BL2),_______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,    _______,     _______, _______,    _______,  _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                                      _______,  _______,  _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______,                 _______,             _______,  _______,  _______,  _______,
        _______, _______, _______,                            _______,                            _______, _______, _______,     _______,    _______,     _______, _______,    _______,            _______
    )
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (user_config.caps_lock_indicator && host_keyboard_led_state().caps_lock) {
        uint8_t i = g_led_config.matrix_co[3][8];
        if (i >= led_min && i < led_max) rgb_matrix_set_color(i, 255, 0, 0);
    }
    if (user_config.num_lock_indicator && host_keyboard_led_state().num_lock) {
        uint8_t i = g_led_config.matrix_co[2][1];
        if (i >= led_min && i < led_max) rgb_matrix_set_color(i, 0, 255, 0);
    }
    if (user_config.scroll_lock_indicator && host_keyboard_led_state().scroll_lock) {
        uint8_t i = g_led_config.matrix_co[0][14];
        if (i >= led_min && i < led_max) rgb_matrix_set_color(i, 0, 0, 255);
    }
    return false;
}
