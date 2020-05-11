/*
 * Touch slide keyboard Slidest40 (сенсорная слайдовая клавиатура Слайдость40)
 * Version: 0.5 beta
 * Date: 2020-05-11
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

#define CHORDF sizeof(chordf) / 3
Chord chordf[] {
  {S2(1,2), KEY_F1},
  {S2(2,1), KEY_F2},
  {S2(1,3), KEY_F3},
  {S2(2,4), KEY_F4},
  {S2(3,1), KEY_F5},
  {S2(4,2), KEY_F6},
  {S2(3,4), KEY_F7},
  {S2(4,3), KEY_F8},
  {S2(3,5), KEY_F9},
  {S2(4,6), KEY_F10},
  {S2(5,6), KEY_F11},
  {S2(6,5), KEY_F12},
};

#define CHORDT sizeof(chordt) / 3
Chord chordt[] {
  {S2(3,4), KEY_RIGHT_ARROW},
  {S2(4,3), KEY_LEFT_ARROW},
};

#define KEY_LAYOUT 255
#define KEY_LAYOUT_FUNC 254
#define KEY_RESET_MODS 253
byte layout = 0;
byte mods = 0;
#define LAYOUT 0
#define LAYOUT_FUNC 1
#define LAYOUT_TAB 2
byte mode = LAYOUT;

// Controls
#define CHORDC sizeof(chordc) / 3
Chord chordc[] = {
  {S2(2,1), KEY_BACKSPACE},
  {S2(2,4), KEY_RETURN},
  {S2(4,6), KEY_DOWN_ARROW},
  {S3(4,6,4), KEY_PAGE_DOWN},
  {S2(6,4), KEY_UP_ARROW},
  {S3(6,4,6), KEY_PAGE_UP},
  {S2(6,5), KEY_LEFT_ARROW},
  {S2(5,6), KEY_RIGHT_ARROW},
  {S3(1,2,1), KEY_LAYOUT},
  {S4(1,2,1,3), KEY_LAYOUT_FUNC},
  {S2(4,2), KEY_LEFT_SHIFT},
  {S4(4,3,4,2), KEY_TAB},
  {S3(3,4,3), KEY_LEFT_CTRL},
  {S3(4,2,4), KEY_LEFT_ALT},
  {S4(4,2,4,6), KEY_LEFT_GUI},
  {S4(4,2,4,2), KEY_RESET_MODS},
};

// Ctrl + Controls
#define CHORDCC sizeof(chordcc) / 3
Chord chordcc[] = {
  {S3(2,1,2), KEY_BACKSPACE},
  {S3(1,3,1), KEY_RETURN},
  {S3(5,6,5), KEY_RIGHT_ARROW},
  {S3(6,5,6), KEY_LEFT_ARROW},
  {S3(4,3,4), KEY_TAB},
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

void chord_symbol(byte k) {
  for (uint8_t i = 0; i < CHORD; i ++) {
    if (touched.list[k].chord == chord[i].chord) {
      bool macros = false;
      if (chord[i].symbols[layout] == 0) { // Macros
        for (byte j = 0; j < CHORDM; j ++) {
          if (touched.list[k].chord == chordm[j].chord) {
            release_mods();
            Keyboard.print(chordm[j].macros);
            macros = true;
            break;
          }
        }
        byte m = layout ? 0 : 1;
        if (! macros && chord[i].symbols[m]) {
          // TODO: save and clean mods
          layout2(layout ? 0 : 1);
          // TODO: restore mods
          Keyboard.write(chord[i].symbols[m]);
          layout2(layout ? 0 : 1);
          release_mods();
        }
      } else {
        if (! (mods && chord[i].symbols[layout] == ' ')) {
          Keyboard.write(chord[i].symbols[layout]);
        }
        release_mods();
      }
      break;
    }
  }
}

void chord_control(byte k) {
  byte multiple = 0;
  if (! (touched.list[k ? 0 : 1].chord & (~ 0b111))) {
    multiple = touched.list[k ? 0 : 1].chord & 0b111;
    touched.list[k ? 0 : 1].index = 0;
  }
  for (uint8_t i = 0; i < CHORDC; i ++) { // Controls
    if (touched.list[k].chord == chordc[i].chord) {
      if (chordc[i].symbol == KEY_LAYOUT_FUNC) {
        mode = (mode == LAYOUT_FUNC) ? LAYOUT : LAYOUT_FUNC;
        led_layout(mode == LAYOUT_FUNC || layout != 0);
      } else if (chordc[i].symbol == KEY_LAYOUT) {
        layout2(layout ? 0 : 1);
      } else if (chordc[i].symbol == KEY_RESET_MODS) {
        release_mods();
      } else if (chordc[i].symbol >= KEY_LEFT_CTRL && chordc[i].symbol <= KEY_RIGHT_GUI) {
        byte mod = (1 << (chordc[i].symbol - KEY_LEFT_CTRL));
        if (mods & mod) {
          mods &= ~ mod;
          Keyboard.release(chordc[i].symbol);
        } else {
          mods |= mod;
          Keyboard.press(chordc[i].symbol);
        }
        led_layout(mods || layout != 0);
      } else if (multiple == 1) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write(chordc[i].symbol);
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
      } else if (multiple) {
        for (byte m = 0; m < multiple; m ++) {
          Keyboard.write(chordc[i].symbol);
        }
      } else {
        Keyboard.write(chordc[i].symbol);
      }
      break;
    }
  }
  for (uint8_t i = 0; i < CHORDCC; i ++) { // Ctrl + Controls
    if (touched.list[k].chord == chordcc[i].chord) {
      if (chordcc[i].symbol == KEY_TAB) {
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write(chordcc[i].symbol);
        mode = LAYOUT_TAB;
        led_layout(true);
      } else {
        Keyboard.press(KEY_LEFT_CTRL);
        if (multiple) {
          for (byte m = 0; m < multiple; m ++) {
            Keyboard.write(chordcc[i].symbol);
          }
        } else {
          Keyboard.write(chordcc[i].symbol);
        }
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
      }
      break;
    }
  }
}

void chord_func(byte k) {
  for (uint8_t i = 0; i < CHORDF; i ++) {
    if (touched.list[k].chord == chordf[i].chord) {
      Keyboard.write(chordf[i].symbol);
      break;
    }
  }
  mode = LAYOUT;
  release_mods();
}

void chord_tab(byte k) {
  if (touched.list[k].index != 3) {
    for (uint8_t i = 0; i < CHORDT; i ++) {
      if (touched.list[k].chord == chordt[i].chord) {
        Keyboard.write(chordt[i].symbol);
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
    chord_func(k);
  } else if (mode == LAYOUT_TAB) {
    chord_tab(k);
  } else {
    chord_symbol(k);
    chord_control(k);
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
