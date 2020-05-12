/*
 * Touch slide keyboard Slidest40 (сенсорная слайдовая клавиатура Слайдость40)
 * Version: 0.6 pre-release
 * Date: 2020-05-12
 * Description: https://github.com/ibnteo/slidest40 (soon)
 * Author: Vladimir Romanovich <ibnteo@gmail.com>
 * License: MIT
 * |1|2|
 * |3|4|
 * |5|6|
 */

#include <Keyboard.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 sensors = Adafruit_MPR121();

struct ListTouched {
  byte index;
  uint16_t chord;
};
struct Touched {
  uint16_t last;
  uint16_t curr;
  ListTouched list[2];
};
Touched touched = {0, 0, {{0, 0}, {0, 0}}};


struct Chord {
  uint16_t chord; // 5*3 bit
  byte symbol;
};

struct Chord2 {
  uint16_t chord;
  byte symbols[2];
};

struct ChordM {
  uint16_t chord;
  String macros;
};

#define S1(s1) (s1)
#define S2(s1,s2) (s1|(s2<<3))
#define S3(s1,s2,s3) (s1|(s2<<3)|(s3<<6))
#define S4(s1,s2,s3,s4) (s1|(s2<<3)|(s3<<6)|(s4<<9))
#define S5(s1,s2,s3,s4,s5) (s1|(s2<<3)|(s3<<6)|(s4<<9)|(s5<<12))

// Chords
#define CHORD sizeof(chord) / 4
Chord2 chord[] = {
  {S1(1), {'o', 'j'}},
  {S2(1,2), {'m', 'v'}},
  {S2(1,3), {'h', 'd'}}, // в
  {S3(1,3,5), {'f', 'a'}},
  {S3(1,2,4), {'w', 'i'}}, // ш
  {S3(1,3,4), {'k', 'x'}}, // ч
  {S1(2), {'a', 'f'}},
  {S3(2,1,3), {'d', 'l'}},
  {S3(2,4,3), {'b', ','}},
  {S3(2,4,6), {0, 'o'}}, // you щ
  {S4(2,4,6,4), {0, ']'}}, // ъ
  {S1(3), {'n', 'y'}},
  {S2(3,1), {'t', 'n'}},
  {S2(3,5), {'s', 'c'}},
  {S2(3,4), {'r', 'h'}},
  {S3(3,1,2), {'j', 'z'}}, // я
  {S3(3,4,2), {0, 'q'}}, // the й
  {S3(3,5,6), {'y', 'm'}}, // ь
  {S3(3,4,6), {0, 's'}}, // same ы
  {S1(4), {' ', ' '}},
  {S2(4,3), {'l', 'k'}},
  {S3(4,2,1), {'u', 'e'}},
  {S3(4,3,1), {0, '.'}}, // ough ю
  {S3(4,6,5), {'q', '\''}}, // э
  {S3(4,3,5), {0, '`'}}, // ght ё
  {S1(5), {'e', 't'}},
  {S2(5,3), {'c', 'r'}}, // к
  {S3(5,3,1), {'z', 'p'}},
  {S3(5,3,4), {'x', '['}}, // х
  {S3(5,6,4), {'v', ';'}}, // ж
  {S1(6), {'i', 'b'}},
  {S3(6,5,3), {'p', 'g'}},
  {S3(6,4,3), {'g', 'u'}},
  {S3(6,4,2), {0, 'w'}}, // tion ц
  //{S4(6,4,2,4), {0, 0}}, // Reserv
  //{S5(6,4,2,4,6), {0, 0}}, // Reserv
  {S4(1,2,4,3), {'1', '1'}},
  {S5(1,2,4,3,1), {'!', '!'}},
  {S4(2,1,3,4), {'2', '2'}},
  {S5(2,1,3,4,2), {'@', 0}},
  {S4(1,3,4,2), {'3', '3'}},
  {S5(1,3,4,2,1), {'#', 0}},
  {S4(2,4,3,1), {'4', '4'}},
  {S5(2,4,3,1,2), {'$', 0}},
  {S4(3,1,2,4), {'5', '5'}},
  {S5(3,1,2,4,3), {'5', '5'}},
  {S4(4,2,1,3), {'6', '6'}},
  {S5(4,2,1,3,4), {'^', 0}},
  {S4(3,4,2,1), {'7', '7'}},
  {S5(3,4,2,1,3), {'&', 0}},
  {S4(4,3,1,2), {'8', '8'}},
  {S5(4,3,1,2,4), {'*', '*'}},
  {S4(3,4,6,5), {'9', '9'}},
  {S5(3,4,6,5,3), {'(', '('}},
  {S4(4,3,5,6), {'0', '0'}},
  {S5(4,3,5,6,4), {')', ')'}},
  {S4(3,5,6,4), {',', '?'}},
  {S5(3,5,6,4,3), {';', '$'}},
  {S4(4,6,5,3), {'.', '/'}},
  {S5(4,6,5,3,4), {':', '^'}},
  {S4(5,3,4,6), {'[', 0}},
  {S5(5,3,4,6,5), {'{', 0}},
  {S4(6,4,3,5), {']', 0}},
  {S5(6,4,3,5,6), {'}', 0}},
  {S4(5,6,4,3), {'/', '|'}},
  {S5(5,6,4,3,5), {'<', 0}},
  {S4(6,5,3,4), {'\\', '\\'}},
  {S5(6,5,3,4,6), {'>', 0}},
  {S3(1,3,1), {'?', '&'}},
  {S4(1,3,1,2), {'|', 0}},
  {S4(5,3,1,3), {'`', 0}},
  {S5(5,3,1,3,5), {'~', 0}},
  {S4(1,3,5,3), {0,'#'}}, // №
  //{S5(1,3,5,3,1), {0, '#'}}, // Reserv
  {S3(3,1,3), {'-', '-'}},
  {S4(3,1,3,5), {'_', '_'}},
  {S3(3,5,3), {'+', '+'}},
  {S4(3,5,3,1), {'=', '='}},
  {S3(5,3,5), {'"', '"'}},
  {S4(5,3,5,6), {'\'', 0}},
};

// Macros
#define CHORDM sizeof(chordm) / 8
ChordM chordm[] = {
  {S3(2,4,6), "you"},
  {S3(3,4,2), "the"},
  {S3(3,4,6), "same"},
  {S3(4,3,1), "ough"},
  {S3(4,3,5), "ght"},
  {S3(6,4,2), "tion"},
};

#define KEY_PRINTSCREEN 0xCE
#define KEY_SCROLL_LOCK 0xCF
#define KEY_PAUSE 0xD0

#define CHORDF sizeof(chordf) / 3
Chord chordf[] {
  {S3(1,2,4), KEY_F1},
  {S4(1,2,4,3), KEY_F1},
  {S3(2,1,3), KEY_F2},
  {S4(2,1,3,4), KEY_F2},
  {S3(1,3,4), KEY_F3},
  {S4(1,3,4,2), KEY_F3},
  {S3(2,4,3), KEY_F4},
  {S4(2,4,3,1), KEY_F4},
  {S3(3,1,2), KEY_F5},
  {S4(3,1,2,4), KEY_F5},
  {S3(4,2,1), KEY_F6},
  {S4(4,2,1,3), KEY_F6},
  {S3(3,4,2), KEY_F7},
  {S4(3,4,2,1), KEY_F7},
  {S3(4,3,1), KEY_F8},
  {S4(4,3,1,2), KEY_F8},
  {S3(3,4,6), KEY_F9},
  {S4(3,4,6,5), KEY_F9},
  {S3(4,3,5), KEY_F10},
  {S4(4,3,5,6), KEY_F10},
  {S3(3,5,6), KEY_F11},
  {S4(3,5,6,4), KEY_F11},
  {S3(4,6,5), KEY_F12},
  {S4(4,6,5,3), KEY_F12},
  {S3(5,3,4), KEY_CAPS_LOCK},
  {S4(5,3,4,6), KEY_CAPS_LOCK},
  {S3(6,4,3), KEY_SCROLL_LOCK},
  {S4(6,4,3,5), KEY_SCROLL_LOCK},
  {S3(5,6,4), KEY_PRINTSCREEN},
  {S4(5,6,4,3), KEY_PRINTSCREEN},
  {S3(6,5,3), KEY_PAUSE},
  {S4(6,5,3,4), KEY_PAUSE},
};

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

#define CHORDN sizeof(chordn) / 3
Chord chordn[] {
  {S3(1,2,4), KEYPAD_1},
  {S4(1,2,4,3), KEYPAD_1},
  {S3(2,1,3), KEYPAD_2},
  {S4(2,1,3,4), KEYPAD_2},
  {S3(1,3,4), KEYPAD_3},
  {S4(1,3,4,2), KEYPAD_3},
  {S3(2,4,3), KEYPAD_4},
  {S4(2,4,3,1), KEYPAD_4},
  {S3(3,1,2), KEYPAD_5},
  {S4(3,1,2,4), KEYPAD_5},
  {S3(4,2,1), KEYPAD_6},
  {S4(4,2,1,3), KEYPAD_6},
  {S3(3,4,2), KEYPAD_7},
  {S4(3,4,2,1), KEYPAD_7},
  {S3(4,3,1), KEYPAD_8},
  {S4(4,3,1,2), KEYPAD_8},
  {S3(3,4,6), KEYPAD_9},
  {S4(3,4,6,5), KEYPAD_9},
  {S3(4,3,5), KEYPAD_0},
  {S4(4,3,5,6), KEYPAD_0},
  {S3(3,5,6), KEY_NUM_LOCK},
  {S4(3,5,6,4), KEY_NUM_LOCK},
  {S3(4,6,5), KEYPAD_PERIOD},
  {S4(4,6,5,3), KEYPAD_PERIOD},
  {S3(5,3,4), KEYPAD_MINUS},
  {S4(5,3,4,6), KEYPAD_MINUS},
  {S3(6,4,3), KEYPAD_PLUS},
  {S4(6,4,3,5), KEYPAD_PLUS},
  {S3(5,6,4), KEYPAD_SLASH},
  {S4(5,6,4,3), KEYPAD_SLASH},
  {S3(6,5,3), KEYPAD_ASTERIX},
  {S4(6,5,3,4), KEYPAD_ASTERIX},
};

#define KEY_TAB_SHIFT 255
#define CHORDT sizeof(chordt) / 3
Chord chordt[] {
  {S2(3,4), KEY_TAB},
  {S2(4,3), KEY_TAB_SHIFT},
  {S2(5,6), KEY_RIGHT_ARROW},
  {S2(6,4), KEY_LEFT_ARROW},
};

#define KEY_LAYOUT 255
#define KEY_LAYOUT_FUNC 254
#define KEY_LAYOUT_NUM 253
#define KEY_RESET_MODS 252
byte layout = 0;
byte mods = 0;
#define LAYOUT 0
#define LAYOUT_FUNC 1
#define LAYOUT_NUM 2
#define LAYOUT_TAB 3
byte mode = LAYOUT;

// Controls
#define CHORDC sizeof(chordc) / 3
Chord chordc[] = {
  {S2(2,1), KEY_BACKSPACE},
  {S4(2,1,2,1), KEY_DELETE},
  {S4(1,2,1,2), KEY_INSERT},
  {S2(2,4), KEY_RETURN},
  {S2(4,6), KEY_DOWN_ARROW},
  {S3(4,6,4), KEY_PAGE_DOWN},
  {S2(6,4), KEY_UP_ARROW},
  {S3(6,4,6), KEY_PAGE_UP},
  {S2(6,5), KEY_LEFT_ARROW},
  {S4(6,5,6,5), KEY_HOME},
  {S2(5,6), KEY_RIGHT_ARROW},
  {S4(5,6,5,6), KEY_END},
  {S3(1,2,1), KEY_LAYOUT},
  {S4(1,2,1,3), KEY_LAYOUT_FUNC},
  {S5(1,2,1,3,5), KEY_LAYOUT_NUM},
  {S2(4,2), KEY_LEFT_SHIFT},
  {S3(4,3,4), KEY_TAB},
  {S3(3,4,3), KEY_LEFT_CTRL},
  {S4(3,4,3,1), KEY_ESC},
  {S3(4,2,4), KEY_LEFT_ALT},
  {S4(4,2,4,6), KEY_LEFT_GUI},
  {S4(4,2,4,2), KEY_RESET_MODS},
};

// Ctrl+Controls or Alt+Tab
#define CHORDCC sizeof(chordcc) / 3
Chord chordcc[] = {
  {S3(2,1,2), KEY_BACKSPACE},
  {S3(1,3,1), KEY_RETURN},
  {S3(5,6,5), KEY_RIGHT_ARROW},
  {S3(6,5,6), KEY_LEFT_ARROW},
  {S4(4,3,4,6), KEY_TAB},
};

#define LED_LAYOUT LED_BUILTIN_RX
void led_layout(bool light) {
  digitalWrite(LED_LAYOUT, light ? LOW : HIGH); // LOW = light
}

#define LINUX   0
#define WINDOWS 1
#define MACOS   2
#define OS LINUX
void layout2(byte layer) {
  if (layout != layer) {
    Keyboard.releaseAll();
    #if OS == MACOS
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(' ');
    #elif OS == WINDOWS
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
    #else
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_SHIFT);
    #endif
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

bool chord_symbol(byte k) {
  bool result = false;
  for (uint8_t i = 0; i < CHORD; i ++) {
    if (touched.list[k].chord == chord[i].chord) {
      if (chord[i].symbols[layout] == 0) { // Macros
        for (byte j = 0; j < CHORDM; j ++) {
          if (touched.list[k].chord == chordm[j].chord) {
            release_mods();
            Keyboard.print(chordm[j].macros);
            result = true;
            break;
          }
        }
        byte m = layout ? 0 : 1;
        if (! result && chord[i].symbols[m]) {
          // TODO: save and clean mods
          layout2(layout ? 0 : 1);
          // TODO: restore mods
          Keyboard.write(chord[i].symbols[m]);
          layout2(layout ? 0 : 1);
          release_mods();
          result = true;
        }
      } else {
        if (! (mods && chord[i].symbols[layout] == ' ')) {
          Keyboard.write(chord[i].symbols[layout]);
        }
        release_mods();
        result = true;
      }
      break;
    }
  }
  return result;
}

bool chord_control(byte k) {
  bool result = false;
  byte multiple = 0;
  if (! (touched.list[k ? 0 : 1].chord & (~ 0b111))) {
    multiple = touched.list[k ? 0 : 1].chord & 0b111;
  }
  for (uint8_t i = 0; i < CHORDC; i ++) { // Controls
    if (touched.list[k].chord == chordc[i].chord) {
      result = true;
      byte symbol = chordc[i].symbol;
      if (mode == LAYOUT_NUM && symbol == KEY_RETURN) {
        symbol = KEYPAD_ENTER;
      }
      if (multiple) {
        touched.list[k ? 0 : 1].index = 0;
      }
      if (symbol == KEY_LAYOUT_FUNC) {
        mode = (mode == LAYOUT_FUNC) ? LAYOUT : LAYOUT_FUNC;
        led_layout(mode == LAYOUT_FUNC || layout != 0);
      } else if (symbol == KEY_LAYOUT_NUM) {
        mode = (mode == LAYOUT_NUM) ? LAYOUT : LAYOUT_NUM;
        led_layout(mode == LAYOUT_NUM || layout != 0);
      } else if (symbol == KEY_LAYOUT) {
        if (mode == LAYOUT) {
          layout2(layout ? 0 : 1);
        } else {
          mode = LAYOUT;
          led_layout(layout != 0);
        }
      } else if (symbol == KEY_RESET_MODS) {
        release_mods();
      } else if (symbol >= KEY_LEFT_CTRL && symbol <= KEY_RIGHT_GUI) {
        byte mod = (1 << (symbol - KEY_LEFT_CTRL));
        if (mods & mod) {
          mods &= ~ mod;
          Keyboard.release(symbol);
        } else {
          mods |= mod;
          Keyboard.press(symbol);
        }
        led_layout(mods || layout != 0);
      } else if (multiple == 1) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write(symbol);
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
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

bool chord_ccontrol(byte k) {
  bool result = false;
  byte multiple = 0;
  if (! (touched.list[k ? 0 : 1].chord & (~ 0b111))) {
    multiple = touched.list[k ? 0 : 1].chord & 0b111;
  }
  for (uint8_t i = 0; i < CHORDCC; i ++) { // Ctrl + Controls
    if (touched.list[k].chord == chordcc[i].chord) {
      result = true;
      byte symbol = chordcc[i].symbol;
      if (mode == LAYOUT_NUM && symbol == KEY_RETURN) {
        symbol = KEYPAD_ENTER;
      }
      if (multiple) {
        touched.list[k ? 0 : 1].index = 0;
      }
      if (symbol == KEY_TAB) {
        if (mods) {
          Keyboard.write(' ');
          release_mods();
        } else {
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.write(symbol);
          mode = LAYOUT_TAB;
          led_layout(true);
        }
      } else {
        Keyboard.press(KEY_LEFT_CTRL);
        if (multiple) {
          for (byte m = 0; m < multiple; m ++) {
            Keyboard.write(symbol);
          }
        } else {
          Keyboard.write(symbol);
        }
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
      }
      break;
    }
  }
  return result;
}

bool chord_func(byte k) {
  bool result = false;
  for (uint8_t i = 0; i < CHORDF; i ++) {
    if (touched.list[k].chord == chordf[i].chord) {
      result = true;
      Keyboard.write(chordf[i].symbol);
      mode = LAYOUT;
      release_mods();
      break;
    }
  }
  return result;
}

bool chord_num(byte k) {
  bool result = false;
  for (uint8_t i = 0; i < CHORDN; i ++) {
    if (touched.list[k].chord == chordn[i].chord) {
      result = true;
      Keyboard.write(chordn[i].symbol);
      break;
    }
  }
  return result;
}

void chord_tab(byte k) {
  if (touched.list[k].index != 3) {
    for (uint8_t i = 0; i < CHORDT; i ++) {
      if (touched.list[k].chord == chordt[i].chord) {
        if (chordt[i].symbol == KEY_TAB_SHIFT) {
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_SHIFT);
          mods &= ~ (KEY_LEFT_SHIFT - 0x80);
        } else {
          Keyboard.write(chordt[i].symbol);
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
  if (mode == LAYOUT_FUNC) {
    chord_func(k) & chord_control(k);
  } else if (mode == LAYOUT_NUM) {
    chord_num(k) & chord_control(k);
  } else if (mode == LAYOUT_TAB) {
    chord_tab(k);
  } else {
    chord_symbol(k) & chord_control(k) & chord_ccontrol(k);
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
  
  for (uint8_t i=0; i<12; i++) {
    uint8_t k = 0; // Side left or right
    uint8_t j = i; // Section convert to 1-6 value on each side
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
        touched.list[k].chord |= (j << touched.list[k].index);
        touched.list[k].index = touched.list[k].index + 3;
      } else {
        touched.list[k].chord = 0;
      }
    }
    if (! (touched.curr & (1 << i)) && (touched.last & (1 << i)) ) { // Release
      if (! (touched.curr & (0b1100110011 << (k + k)))) { // All release
        if (touched.list[k].index && touched.list[k].chord) {
          press(k);
        }
        touched.list[k].index = 0;
        touched.list[k].chord = 0;
      }
    }
  }

  touched.last = touched.curr;
}
