/*
   Touch slide keyboard Slidest40 (сенсорная слайдовая клавиатура Слайдость40)
   Version: 0.7 pre-release
   Date: 2020-05-15
   Description: https://github.com/ibnteo/slidest40 (soon)
   Author: Vladimir Romanovich <ibnteo@gmail.com>
   License: MIT
   |1|2|
   |3|4|
   |5|6|
*/

#include <Keyboard.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 sensors = Adafruit_MPR121();

struct ListTouched {
  byte index;
  word minor;
  byte major;
};
struct Touched {
  word last;
  word curr;
  ListTouched list[2];
};
Touched touched = {0, 0, {{0, 0}, {0, 0}}};


struct Chord {
  word minor; // 5*3 bit
  byte major; // 2*3 bit
  byte symbol;
};

struct Chord2 {
  word minor;
  byte major;
  byte symbols[2];
};

struct ChordString {
  word minor;
  byte major;
  String macros;
};

#define S1(s1) (s1)
#define S2(s1,s2) (s1|(s2<<3))
#define S3(s1,s2,s3) (s1|(s2<<3)|(s3<<6))
#define S4(s1,s2,s3,s4) (s1|(s2<<3)|(s3<<6)|(s4<<9))
#define S5(s1,s2,s3,s4,s5) (s1|(s2<<3)|(s3<<6)|(s4<<9)|(s5<<12))
#define S6(s1,s2,s3,s4,s5,s6) (s1|(s2<<3)|(s3<<6)|(s4<<9)|(s5<<12)), (s6)

#define KEY_PRINTSCREEN 0xCE
#define KEY_SCROLL_LOCK 0xCF
#define KEY_PAUSE 0xD0

#define KEY_NUM_LOCK 0xDB
#define KEYPAD_SLASH 0xDC
#define KEYPAD_ASTERIX 0xDD
#define KEYPAD_MINUS 0xDE
#define KEYPAD_PLUS 0xDF
#define KEYPAD_ENTER 0xE0
#define KEYPAD_1 0xE1
#define KEYPAD_2 0xE2
#define KEYPAD_3 0xE3
#define KEYPAD_4 0xE4
#define KEYPAD_5 0xE5
#define KEYPAD_6 0xE6
#define KEYPAD_7 0xE7
#define KEYPAD_8 0xE8
#define KEYPAD_9 0xE9
#define KEYPAD_0 0xEA
#define KEYPAD_PERIOD 0xEB

#define KEY_LINUX 0xFF
#define KEY_WINDOWS 0xFE
#define KEY_MACOS 0xFD
#define KEY_ANDROID 0xFC

#define NUMERICS sizeof(numerics) / 4
const Chord numerics[] PROGMEM = {
  {S3(1, 2, 4), 0, KEYPAD_1},
  {S4(1, 2, 4, 3), 0, KEYPAD_1},
  {S3(2, 1, 3), 0, KEYPAD_2},
  {S4(2, 1, 3, 4), 0, KEYPAD_2},
  {S3(1, 3, 4), 0, KEYPAD_3},
  {S4(1, 3, 4, 2), 0, KEYPAD_3},
  {S3(2, 4, 3), 0, KEYPAD_4},
  {S4(2, 4, 3, 1), 0, KEYPAD_4},
  {S3(3, 1, 2), 0, KEYPAD_5},
  {S4(3, 1, 2, 4), 0, KEYPAD_5},
  {S3(4, 2, 1), 0, KEYPAD_6},
  {S4(4, 2, 1, 3), 0, KEYPAD_6},
  {S3(3, 4, 2), 0, KEYPAD_7},
  {S4(3, 4, 2, 1), 0, KEYPAD_7},
  {S3(4, 3, 1), 0, KEYPAD_8},
  {S4(4, 3, 1, 2), 0, KEYPAD_8},
  {S3(3, 4, 6), 0, KEYPAD_9},
  {S4(3, 4, 6, 5), 0, KEYPAD_9},
  {S3(4, 3, 5), 0, KEYPAD_0},
  {S4(4, 3, 5, 6), 0, KEYPAD_0},
  {S3(3, 5, 6), 0, KEY_NUM_LOCK},
  {S4(3, 5, 6, 4), 0, KEY_NUM_LOCK},
  {S3(4, 6, 5), 0, KEYPAD_PERIOD},
  {S4(4, 6, 5, 3), 0, KEYPAD_PERIOD},
  {S3(5, 3, 4), 0, KEYPAD_MINUS},
  {S4(5, 3, 4, 6), 0, KEYPAD_MINUS},
  {S3(6, 4, 3), 0, KEYPAD_PLUS},
  {S4(6, 4, 3, 5), 0, KEYPAD_PLUS},
  {S3(5, 6, 4), 0, KEYPAD_SLASH},
  {S4(5, 6, 4, 3), 0, KEYPAD_SLASH},
  {S3(6, 5, 3), 0, KEYPAD_ASTERIX},
  {S4(6, 5, 3, 4), 0, KEYPAD_ASTERIX},

  {S4(3, 4, 3, 1), 0, KEY_LINUX},
  {S4(3, 1, 3, 4), 0, KEY_WINDOWS},
  {S4(3, 5, 3, 4), 0, KEY_MACOS},
  {S4(3, 4, 3, 5), 0, KEY_ANDROID},
};

#define KEY_TAB_SHIFT 255
#define TABS sizeof(tabs) / 4
const Chord tabs[] PROGMEM = {
  {S2(3, 4), 0, KEY_TAB},
  {S2(4, 3), 0, KEY_TAB_SHIFT},
  {S2(5, 6), 0, KEY_RIGHT_ARROW},
  {S2(6, 5), 0, KEY_LEFT_ARROW},
  {S4(4, 3, 4, 2), 0, KEY_ESC},
};

#define SYMBOLS sizeof(symbols) / 5
const Chord2 symbols[] PROGMEM = {
  {S1(1), 0, {'o', 'j'}},
  {S2(1, 2), 0, {'m', 'v'}},
  {S2(1, 3), 0, {'h', 'd'}}, // в
  {S3(1, 3, 5), 0, {'f', 'a'}},
  {S3(1, 2, 4), 0, {'w', 'i'}}, // ш
  {S3(1, 3, 4), 0, {'k', 'x'}}, // ч
  {S1(2), 0, {'a', 'f'}},
  {S3(2, 1, 3), 0, {'d', 'l'}},
  {S3(2, 4, 3), 0, {'b', ','}},
  {S3(2, 4, 6), 0, {0, 'o'}}, // you щ
  {S4(2, 4, 6, 4), 0, {0, ']'}}, // ъ
  {S1(3), 0, {'n', 'y'}},
  {S2(3, 1), 0, {'t', 'n'}},
  {S2(3, 5), 0, {'s', 'c'}},
  {S2(3, 4), 0, {'r', 'h'}},
  {S3(3, 1, 2), 0, {'j', 'z'}}, // я
  {S3(3, 4, 2), 0, {0, 'q'}}, // the й
  {S3(3, 5, 6), 0, {'y', 'm'}}, // ь
  {S3(3, 4, 6), 0, {0, 's'}}, // same ы
  {S1(4), 0, {' ', ' '}},
  {S2(4, 3), 0, {'l', 'k'}},
  {S3(4, 2, 1), 0, {'u', 'e'}},
  {S3(4, 3, 1), 0, {0, '.'}}, // ough ю
  {S3(4, 6, 5), 0, {'q', '\''}}, // э
  {S3(4, 3, 5), 0, {0, '`'}}, // ght ё
  {S1(5), 0, {'e', 't'}},
  {S2(5, 3), 0, {'c', 'r'}}, // к
  {S3(5, 3, 1), 0, {'z', 'p'}},
  {S3(5, 3, 4), 0, {'x', '['}}, // х
  {S3(5, 6, 4), 0, {'v', ';'}}, // ж
  {S1(6), 0, {'i', 'b'}},
  {S3(6, 5, 3), 0, {'p', 'g'}},
  {S3(6, 4, 3), 0, {'g', 'u'}},
  {S3(6, 4, 2), 0, {0, 'w'}}, // tion ц
  //{S4(6,4,2,4), 0, {0, 0}}, // Reserved
  //{S5(6,4,2,4,6), 0, {0, 0}}, // Reserved
  {S4(1, 2, 4, 3), 0, {'1', '1'}},
  {S5(1, 2, 4, 3, 1), 0, {'!', '!'}},
  {S4(2, 1, 3, 4), 0, {'2', '2'}},
  {S5(2, 1, 3, 4, 2), 0, {'@', 0}},
  {S4(1, 3, 4, 2), 0, {'3', '3'}},
  {S5(1, 3, 4, 2, 1), 0, {'#', 0}},
  {S4(2, 4, 3, 1), 0, {'4', '4'}},
  {S5(2, 4, 3, 1, 2), 0, {'$', 0}},
  {S4(3, 1, 2, 4), 0, {'5', '5'}},
  {S5(3, 1, 2, 4, 3), 0, {'5', '5'}},
  {S4(4, 2, 1, 3), 0, {'6', '6'}},
  {S5(4, 2, 1, 3, 4), 0, {'^', 0}},
  {S4(3, 4, 2, 1), 0, {'7', '7'}},
  {S5(3, 4, 2, 1, 3), 0, {'&', 0}},
  {S4(4, 3, 1, 2), 0, {'8', '8'}},
  {S5(4, 3, 1, 2, 4), 0, {'*', '*'}},
  {S4(3, 4, 6, 5), 0, {'9', '9'}},
  {S5(3, 4, 6, 5, 3), 0, {'(', '('}},
  {S4(4, 3, 5, 6), 0, {'0', '0'}},
  {S5(4, 3, 5, 6, 4), 0, {')', ')'}},
  {S4(3, 5, 6, 4), 0, {',', '?'}},
  {S5(3, 5, 6, 4, 3), 0, {';', '$'}},
  {S4(4, 6, 5, 3), 0, {'.', '/'}},
  {S5(4, 6, 5, 3, 4), 0, {':', '^'}},
  {S4(5, 3, 4, 6), 0, {'[', 0}},
  {S5(5, 3, 4, 6, 5), 0, {'{', 0}},
  {S4(6, 4, 3, 5), 0, {']', 0}},
  {S5(6, 4, 3, 5, 6), 0, {'}', 0}},
  {S4(5, 6, 4, 3), 0, {'/', '|'}},
  {S5(5, 6, 4, 3, 5), 0, {'<', 0}},
  {S4(6, 5, 3, 4), 0, {'\\', '\\'}},
  {S5(6, 5, 3, 4, 6), 0, {'>', 0}},
  {S3(1, 3, 1), 0, {'?', '&'}},
  {S4(1, 3, 1, 2), 0, {'|', 0}},
  {S4(5, 3, 1, 3), 0, {'`', 0}},
  {S5(5, 3, 1, 3, 5), 0, {'~', 0}},
  {S4(1, 3, 5, 3), 0, {0, '#'}}, // №
  //{S5(1,3,5,3,1), 0, {0, '#'}}, // Reserved
  {S3(3, 1, 3), 0, {'-', '-'}},
  {S4(3, 1, 3, 5), 0, {'_', '_'}},
  {S3(3, 5, 3), 0, {'+', '+'}},
  {S4(3, 5, 3, 1), 0, {'=', '='}},
  {S3(5, 3, 5), 0, {'"', '"'}},
  {S4(5, 3, 5, 6), 0, {'\'', 0}},

  {S5(1, 2, 4, 3, 1), S1(2), KEY_F1},
  {S5(2, 1, 3, 4, 2), S1(1), KEY_F2},
  {S5(1, 3, 4, 2, 1), S1(3), KEY_F3},
  {S5(2, 4, 3, 1, 2), S1(4), KEY_F4},
  {S5(3, 1, 2, 4, 3), S1(1), KEY_F5},
  {S5(4, 2, 1, 3, 4), S1(2), KEY_F6},
  {S5(3, 4, 2, 1, 3), S1(4), KEY_F7},
  {S5(4, 3, 1, 2, 4), S1(3), KEY_F8},
  {S5(3, 4, 6, 5, 3), S1(4), KEY_F9},
  {S5(4, 3, 5, 6, 4), S1(3), KEY_F10},
  {S5(3, 5, 6, 4, 3), S1(5), KEY_F11},
  {S5(4, 6, 5, 3, 4), S1(6), KEY_F12},
  {S5(5, 3, 4, 6, 5), S1(3), KEY_CAPS_LOCK},
  {S5(6, 4, 3, 5, 6), S1(4), KEY_SCROLL_LOCK},
  {S5(5, 6, 4, 3, 5), S1(6), KEY_PRINTSCREEN},
  {S5(6, 5, 3, 4, 6), S1(5), KEY_PAUSE},

  {S5(1, 2, 4, 3, 1), S2(2, 4), KEYPAD_1},
  {S5(2, 1, 3, 4, 2), S2(1, 3), KEYPAD_2},
  {S5(1, 3, 4, 2, 1), S2(3, 4), KEYPAD_3},
  {S5(2, 4, 3, 1, 2), S2(4, 3), KEYPAD_4},
  {S5(3, 1, 2, 4, 3), S2(1, 2), KEYPAD_5},
  {S5(4, 2, 1, 3, 4), S2(2, 1), KEYPAD_6},
  {S5(3, 4, 2, 1, 3), S2(4, 2), KEYPAD_7},
  {S5(4, 3, 1, 2, 4), S2(3, 1), KEYPAD_8},
  {S5(3, 4, 6, 5, 3), S2(4, 6), KEYPAD_9},
  {S5(4, 3, 5, 6, 4), S2(3, 5), KEYPAD_0},
  {S5(3, 5, 6, 4, 3), S2(5, 6), KEY_NUM_LOCK},
  {S5(4, 6, 5, 3, 4), S2(6, 5), KEYPAD_PERIOD},
  {S5(5, 3, 4, 6, 5), S2(3, 4), KEYPAD_MINUS},
  {S5(6, 4, 3, 5, 6), S2(4, 3), KEYPAD_PLUS},
  {S5(5, 6, 4, 3, 5), S2(6, 4), KEYPAD_SLASH},
  {S5(6, 5, 3, 4, 6), S2(5, 3), KEYPAD_ASTERIX},
};

// Macros
#define MACROS sizeof(macros) / 9
const ChordString macros[] = {
  {S3(2, 4, 6), 0, "you"},
  {S3(3, 4, 2), 0, "the"},
  {S3(3, 4, 6), 0, "same"},
  {S3(4, 3, 1), 0, "ough"},
  {S3(4, 3, 5), 0, "ght"},
  {S3(6, 4, 2), 0, "tion"},
};

#define KEY_LAYOUT_0 255
#define KEY_LAYOUT_1 254
#define KEY_LAYOUT_NUM 253
#define KEY_RESET_MODS 252
byte layout = 0;
byte mods = 0;
#define LAYOUT 0
#define LAYOUT_NUM 1
#define LAYOUT_TAB 2
byte mode = LAYOUT;

#define CONTROLS sizeof(controls) / 4
const Chord controls[] PROGMEM = {
  {S2(2, 1), 0, KEY_BACKSPACE},
  {S4(2, 1, 2, 4), 0, KEY_INSERT},
  {S2(2, 4), 0, KEY_RETURN},
  {S2(4, 6), 0, KEY_DOWN_ARROW},
  {S3(4, 6, 4), 0, KEY_PAGE_DOWN},
  {S2(6, 4), 0, KEY_UP_ARROW},
  {S3(6, 4, 6), 0, KEY_PAGE_UP},
  {S2(6, 5), 0, KEY_LEFT_ARROW},
  {S4(6, 5, 6, 5), 0, KEY_HOME},
  {S2(5, 6), 0, KEY_RIGHT_ARROW},
  {S4(5, 6, 5, 6), 0, KEY_END},
  {S3(1, 2, 1), 0, KEY_DELETE},
  {S4(1, 2, 1, 3), 0, KEY_LAYOUT_0},
  {S5(1, 2, 1, 3, 5), 0, KEY_LAYOUT_1},
  {S5(1, 2, 1, 3, 4), 0, KEY_LAYOUT_NUM},
  {S2(4, 2), 0, KEY_LEFT_SHIFT},
  {S3(4, 3, 4), 0, KEY_TAB},
  {S4(4, 3, 4, 2), 0, KEY_ESC},
  {S3(3, 4, 3), 0, KEY_LEFT_CTRL},
  {S3(4, 2, 4), 0, KEY_LEFT_ALT},
  {S4(4, 2, 4, 6), 0, KEY_LEFT_GUI},
  {S4(4, 2, 4, 3), 0, KEY_RESET_MODS},
};

#define CONTROLS_MODS sizeof(controls_mods) / 4
const Chord controls_mods[] PROGMEM = {
  {S3(2, 1, 2), 0, KEY_BACKSPACE},
  {S3(1, 3, 1), 0, KEY_RETURN},
  {S3(5, 6, 5), 0, KEY_RIGHT_ARROW},
  {S3(6, 5, 6), 0, KEY_LEFT_ARROW},
  {S4(4, 3, 4, 6), 0, KEY_TAB},
};

#define LED_LAYOUT LED_BUILTIN_RX
void led_layout(bool light) {
  digitalWrite(LED_LAYOUT, light ? LOW : HIGH); // LOW = light
}

#define LINUX   0
#define WINDOWS 1
#define MACOS   2
#define ANDROID 3
byte os = LINUX;
void change_layout(byte layer) {
  if (layout != layer) {
    Keyboard.releaseAll();
    if (os == MACOS || os == ANDROID) {
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(' ');
    } else if (os == WINDOWS) {
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_CTRL);
    } else {
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_ALT);
    }
    Keyboard.releaseAll();
  }
  layout = layer;
  led_layout(layout != 0);
}

void release_mods() {
  Keyboard.releaseAll();
  mods = 0;
  led_layout(layout != 0);
}

bool chord_symbols(byte k) {
  bool result = false;
  byte l = layout;
  if (mods & (~ ((KEY_LEFT_SHIFT - KEY_LEFT_CTRL) & (KEY_RIGHT_SHIFT - KEY_LEFT_CTRL)))) {
    l = 0;
  }
  for (byte i = 0; i < SYMBOLS; i ++) {
    if (touched.list[k].minor == pgm_read_word(&symbols[i].minor) && touched.list[k].major == pgm_read_byte(&symbols[i].major)) {
      if (l == layout && pgm_read_byte(&symbols[i].symbols[layout]) == 0) { // Macros
        for (byte j = 0; j < MACROS; j ++) {
          if (touched.list[k].minor == macros[j].minor && touched.list[k].major == macros[j].major) {
            release_mods();
            Keyboard.print(macros[j].macros);
            result = true;
            break;
          }
        }
        byte m = layout ? 0 : 1;
        if (! result && pgm_read_byte(&symbols[i].symbols[m])) {
          // TODO: save and clean mods
          change_layout(layout ? 0 : 1);
          // TODO: restore mods
          Keyboard.write(pgm_read_byte(&symbols[i].symbols[m]));
          change_layout(layout ? 0 : 1);
          release_mods();
          result = true;
        }
      } else {
        if (! (mods && pgm_read_byte(&symbols[i].symbols[l]) == ' ')) {
          Keyboard.write(pgm_read_byte(&symbols[i].symbols[l]));
        }
        release_mods();
        result = true;
      }
      break;
    }
  }
  return result;
}

bool chord_controls(byte k) {
  bool result = false;
  byte multiple = 0;
  if (! (touched.list[k ? 0 : 1].minor & (~ 0b111))) {
    multiple = touched.list[k ? 0 : 1].minor & 0b111;
    if (multiple == 5) multiple = 10;
    else if (multiple == 6) multiple = 20;
  }
  for (byte i = 0; i < CONTROLS; i ++) {
    if (touched.list[k].minor == pgm_read_word(&controls[i].minor) && touched.list[k].major == pgm_read_byte(&controls[i].major)) {
      result = true;
      byte symbol = pgm_read_byte(&controls[i].symbol);
      if (mode == LAYOUT_NUM && symbol == KEY_RETURN) {
        symbol = KEYPAD_ENTER;
      }
      if (multiple) {
        touched.list[k ? 0 : 1].index = 0;
      }
      if (symbol == KEY_LAYOUT_NUM) {
        mode = (mode == LAYOUT_NUM) ? LAYOUT : LAYOUT_NUM;
        led_layout(mode == LAYOUT_NUM || layout != 0);
      } else if (symbol == KEY_LAYOUT_0 || symbol == KEY_LAYOUT_1) {
        if (mode != LAYOUT) {
          mode = LAYOUT;
        }
        byte l = (symbol == KEY_LAYOUT_0) ? 0 : 1;
        if (l != layout) change_layout(l);
        led_layout(layout != 0);
      } else if (symbol == KEY_RESET_MODS) {
        release_mods();
      } else if (symbol >= KEY_LEFT_CTRL && symbol <= KEY_RIGHT_GUI) {
        if (os == MACOS && symbol == KEY_LEFT_CTRL) {
          symbol = KEY_LEFT_GUI;
        } else if (os == MACOS && symbol == KEY_LEFT_GUI) {
          symbol = KEY_LEFT_CTRL;
        }
        if (k == 1 && symbol < KEY_RIGHT_CTRL) { // Right modificators
          symbol = symbol + 4;
        }
        byte mod = (1 << (symbol - KEY_LEFT_CTRL));
        if (mods & mod) {
          mods &= ~ mod;
          Keyboard.release(symbol);
        } else {
          mods |= mod;
          Keyboard.press(symbol);
        }
        led_layout(mods || layout != 0);
      } else if (multiple == 1) { // Multiple 1 = Ctrl+ (Cmd+ on Macos)
        byte modificator = KEY_LEFT_CTRL;
        if (os == MACOS) {
          modificator = KEY_LEFT_GUI;
        }
        Keyboard.press(modificator);
        Keyboard.write(symbol);
        Keyboard.release(modificator);
        mods &= ~ (modificator - KEY_LEFT_CTRL);
        led_layout(mods || layout != 0);
      } else if (multiple) {
        for (byte m = 0; m < multiple; m ++) {
          Keyboard.write(symbol);
        }
      } else {
        Keyboard.write(symbol);
      }
      break;
    }
  }
  return result;
}

bool chord_controls_mods(byte k) {
  bool result = false;
  byte multiple = 0;
  if (! (touched.list[k ? 0 : 1].minor & (~ 0b111))) {
    multiple = touched.list[k ? 0 : 1].minor & 0b111;
    if (multiple == 5) multiple = 10;
    else if (multiple == 6) multiple = 20;
  }
  for (byte i = 0; i < CONTROLS_MODS; i ++) {
    if (touched.list[k].minor == pgm_read_word(&controls_mods[i].minor) && touched.list[k].major == pgm_read_byte(&controls_mods[i].major)) {
      result = true;
      byte symbol = pgm_read_byte(&controls_mods[i].symbol);
      if (mode == LAYOUT_NUM && symbol == KEY_RETURN) {
        symbol = KEYPAD_ENTER;
      }
      if (multiple) {
        touched.list[k ? 0 : 1].index = 0;
      }
      if (symbol == KEY_TAB) { // Alt+Tab (Alt hold)
        if (mods) {
          Keyboard.write(' ');
          release_mods();
        } else {
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.write(symbol);
          mode = LAYOUT_TAB;
          led_layout(true);
        }
      } else { // Ctrl+ (Cmd+ on Macos)
        byte modificator = KEY_LEFT_CTRL;
        if (os == MACOS) {
          modificator = KEY_LEFT_GUI;
        }
        Keyboard.press(modificator);
        if (multiple) {
          for (byte m = 0; m < multiple; m ++) {
            Keyboard.write(symbol);
          }
        } else {
          Keyboard.write(symbol);
        }
        Keyboard.release(modificator);
        mods &= ~ (modificator - KEY_LEFT_CTRL);
        led_layout(mods || layout != 0);
      }
      break;
    }
  }
  return result;
}

bool chord_numerics(byte k) {
  bool result = false;
  for (byte i = 0; i < NUMERICS; i ++) {
    if (touched.list[k].minor == pgm_read_word(&numerics[i].minor) && touched.list[k].major == pgm_read_byte(&numerics[i].major)) {
      result = true;
      byte symbol = pgm_read_byte(&numerics[i].symbol);
      if (symbol >= KEY_ANDROID) {
        os = 0xFF - symbol;
      } else {
        Keyboard.write(symbol);
      }
      break;
    }
  }
  return result;
}

void chord_tabs(byte k) {
  if (touched.list[k].index != 3) {
    for (byte i = 0; i < TABS; i ++) {
      if (touched.list[k].minor == pgm_read_word(&tabs[i].minor) && touched.list[k].major == pgm_read_byte(&tabs[i].major)) {
        byte symbol = pgm_read_byte(&tabs[i].symbol);
        if (symbol == KEY_TAB_SHIFT) {
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_SHIFT);
          mods &= ~ (KEY_LEFT_SHIFT - KEY_LEFT_CTRL);
        } else if (symbol == KEY_ESC) {
          Keyboard.write(symbol);
          mode = LAYOUT;
          release_mods();
        } else {
          Keyboard.write(symbol);
        }
        break;
      }
    }
  } else {
    mode = LAYOUT;
    release_mods();
  }
}

void press(byte k) { // Press on release
  if (mode == LAYOUT_TAB) {
    chord_tabs(k);
  } else if (mode == LAYOUT_NUM) {
    chord_numerics(k) & chord_controls(k) & chord_controls_mods(k);
  } else {
    chord_symbols(k) & chord_controls(k) & chord_controls_mods(k);
  }
}

void setup() {
  pinMode(LED_BUILTIN_TX, INPUT); // disable LED TX
  pinMode(LED_LAYOUT, OUTPUT);
  Keyboard.begin();
  sensors.begin(0x5A);
}

void loop() {
  touched.curr = sensors.touched();

  for (byte i = 0; i < 12; i++) {
    byte k = 0; // Side left or right
    byte j = i; // Section convert to 1-6 value on each side
    if ((touched.curr & 0b110011001100 & (1 << i)) || (touched.last & 0b110011001100 & (1 << i))) { // Right side
      k = 1;
      j = j - 2;
    }
    if (j > 1) j = j - 2;
    if (j > 3) j = j - 2;
    j ++;
    j = j & 0b111;

    if ((touched.curr & (1 << i)) && ! (touched.last & (1 << i)) ) { // Touch
      if (touched.list[k].index < 15) {
        touched.list[k].minor |= (j << touched.list[k].index);
        touched.list[k].index = touched.list[k].index + 3;
      } else if (touched.list[k].index < 21) {
        touched.list[k].major |= (j << (touched.list[k].index - 15));
        touched.list[k].index = touched.list[k].index + 3;
      } else {
        touched.list[k].major = 0;
        touched.list[k].minor = 0;
      }
    }
    if (! (touched.curr & (1 << i)) && (touched.last & (1 << i)) ) { // Release
      if (! (touched.curr & (0b1100110011 << (k + k)))) { // All release
        if (touched.list[k].index && touched.list[k].minor) {
          press(k);
        }
        touched.list[k].index = 0;
        touched.list[k].major = 0;
        touched.list[k].minor = 0;
      }
    }
  }

  touched.last = touched.curr;
}
