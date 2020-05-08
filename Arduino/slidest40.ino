/*
 * Touch slide keyboard Slidest40 (сенсорная слайдовая клавиатура Слайдость40)
 * Version: 0.2 beta
 * Date: 2020-05-08
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

struct Chord4 {
  uint16_t chord;
  byte symbols[4];
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
#define CHORD sizeof(chord) / 6
Chord4 chord[] = {
  {S1(1), {'a', 'A', 'f', 'F'}},
  {S2(1,2), {'t', 'T', 'n', 'N'}},
  {S2(1,3), {'c', 'C', 'r', 'R'}}, // к
  {S3(1,3,5), {'d', 'D', 'l', 'L'}},
  {S3(1,2,4), {'g', 'G', 'u', 'U'}},
  {S3(1,3,4), {0, 0, 'z', 'Z'}}, // я
  {S1(2), {'e', 'E', 't', 'T'}},
  {S3(2,1,3), {'k', 'K', 'x', 'X'}}, // ч
  {S3(2,4,3), {0, 0, '\'', '"'}}, // э
  {S3(2,4,6), {0, 0, 'm', 'M'}}, // ь
  {S1(3), {'i', 'I', 'b', 'B'}},
  {S2(3,1), {'n', 'N', 'y', 'Y'}},
  {S2(3,5), {'s', 'S', 'c', 'C'}},
  {S2(3,4), {'h', 'H', 'd', 'D'}},
  {S3(3,1,2), {'b', 'B', ',', '<'}},
  {S3(3,4,2), {'j', 'J', 'q', 'Q'}},
  {S3(3,5,6), {'z', 'z', 'p', 'P'}},
  {S3(3,4,6), {'q', 'Q', 's', 'S'}},
  {S1(4), {' ', ' ', ' ', ' '}},
  {S2(4,3), {'r', 'R', 'h', 'H'}},
  {S3(4,2,1), {'y', 'Y', 'm', 'M'}},
  {S3(4,3,1), {'v', 'V', ';', ':'}},
  {S3(4,6,5), {'f', 'F', 'o', 'O'}}, // щ
  {S3(4,3,5), {'w', 'W', 'i', 'I'}},
  {S1(5), {'o', 'O', 'j', 'J'}},
  {S2(5,3), {'l', 'L', 'k', 'K'}},
  {S3(5,3,1), {'m', 'M', 'v', 'V'}},
  {S3(5,3,4), {0, 0, '`', '~'}}, // ё
  {S3(5,6,4), {'p', 'P', 'g', 'G'}},
  {S1(6), {'u', 'U', 'e', 'E'}},
  {S3(6,5,3), {'x', 'X', '[', '{'}}, // х
  {S3(6,4,3), {0, 0, '.', '>'}}, // ю
  {S3(6,4,2), {0, 0, 'w', 'W'}}, // ц
  {S4(1,2,4,3), {'1', '!', '1', '!'}},
  {S4(2,1,3,4), {'2', '@', '2', 0}},
  {S4(1,3,4,2), {'3', '#', '3', 0}},
  {S4(2,4,3,1), {'4', '$', '4', 0}},
  {S4(3,1,2,4), {'5', '%', '5', '%'}},
  {S4(4,2,1,3), {'6', '^', '6', 0}},
  {S4(3,4,2,1), {'7', '&', '7', 0}},
  {S4(4,3,1,2), {'8', '*', '8', '*'}},
  {S4(3,4,6,5), {'9', '(', '9', '('}},
  {S4(4,3,5,6), {'0', ')', '0', ')'}},
  {S4(3,5,6,4), {',', ';', '?', '$'}},
  {S4(4,6,5,3), {'.', ':', '/', '^'}},
  {S4(5,3,4,6), {'[', '{', 0, 0}},
  {S4(6,4,3,5), {']', '}', 0, 0}},
  {S4(5,6,4,3), {'/', '<', '|', 0}},
  {S4(6,5,3,4), {'\\', '>', '\\', 0}},
  {S3(1,3,1), {'?', '|', '&', 0}},
  {S4(5,3,1,3), {'`', '~', 0, 0}},
  {S4(1,3,5,3), {'#', '#', '#', '#'}}, // № and reserv
  {S3(3,1,3), {'-', '_', '-', '_'}},
  {S3(3,5,3), {'+', '=', '+', '='}},
  {S3(5,3,5), {'"', '\'', '"', 0}},
};

// Select layout 1
#define CHORDL sizeof(chordl) / 3
Chord2 chordl[] = {
  {S4(2,1,3,4), {0, '@'}},
  {S4(1,3,4,2), {0, '#'}},
  {S4(2,4,3,1), {0, '$'}},
  {S4(4,2,1,3), {0, '^'}},
  {S4(3,4,2,1), {0, '&'}},
  {S4(5,3,4,6), {'[', '{'}},
  {S4(6,4,3,5), {']', '}'}},
  {S4(5,3,1,3), {'`', '~'}},
  {S3(5,3,5), {0, '\''}},
  {S3(1,3,1), {0, '|'}},
  {S4(5,6,4,3), {0, '<'}},
  {S4(6,5,3,4), {0, '>'}},
};

#define KEY_LAYOUT 255
#define KEY_LAYOUT_FUNC 254
#define KEY_RESET_MODS 253
byte layout = 0;
byte mods = 0;
bool layout_func = false;

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
  {S4(1,2,1,2), KEY_LAYOUT_FUNC},
  {S2(4,2), KEY_LEFT_SHIFT},
  {S3(4,3,4), KEY_TAB},
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
};

// Macros
#define CHORDM sizeof(chordm) / 8
ChordM chordm[] = {
  {S3(1,3,4), "m1"},
  {S3(2,4,3), "m2"},
  {S3(5,3,4), "m3"},
  {S3(6,4,3), "m4"},
};

#define LED_LAYOUT LED_BUILTIN_RX
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
  digitalWrite(LED_LAYOUT, layout == 0 ? HIGH : LOW); // LOW = light
}

void releaseMods() {
    Keyboard.releaseAll();
    mods = 0;
    digitalWrite(LED_LAYOUT, layout == 0 ? HIGH : LOW); // LOW = light
}

void press(byte k) { // Press on release
  byte l = layout_func ? 4 : layout * 2;
  if (l < 3 && (mods & (1 << (KEY_LEFT_SHIFT - KEY_LEFT_CTRL)))) {
    l ++;
  }
  for (uint8_t i = 0; i < CHORD; i ++) {
    if (touched.list[k].chord == chord[i].chord) {
      if (chord[i].symbols[l] == 0) { // Macros
        for (byte j = 0; j < CHORDM; j ++) {
          if (touched.list[k].chord == chordm[j].chord) {
            releaseMods();
            Keyboard.print(chordm[j].macros);
            break;
          }
        }
        for (byte j = 0; j < CHORDL; j ++) {
          if (touched.list[k].chord == chordl[j].chord) {
            byte m = (mods & (1 << (KEY_LEFT_SHIFT - KEY_LEFT_CTRL))) ? 1 : 0;
            if (chordl[j].symbols[m]) {
              layout2(layout ? 0 : 1);
              Keyboard.release(KEY_LEFT_SHIFT);
              Keyboard.write(chordl[j].symbols[m]);
              layout2(layout ? 0 : 1);
              releaseMods();
            }
            break;
          }
        }
      } else {
        Keyboard.release(KEY_LEFT_SHIFT);
        Keyboard.write(chord[i].symbols[l]);
        releaseMods();
      }
      break;
    }
  }
  if (l == 4) {
    layout_func = false;
    digitalWrite(LED_LAYOUT, layout != 0 ? LOW : HIGH); // LOW = light
  } else {
    for (uint8_t i = 0; i < CHORDC; i ++) { // Controls
      if (touched.list[k].chord == chordc[i].chord) {
        if (chordc[i].symbol == KEY_LAYOUT_FUNC) {
          layout_func = ! layout_func;
          digitalWrite(LED_LAYOUT, layout_func || layout != 0 ? LOW : HIGH); // LOW = light
        } else if (chordc[i].symbol == KEY_LAYOUT) {
          layout2(layout ? 0 : 1);
        } else if (chordc[i].symbol == KEY_RESET_MODS) {
          releaseMods();
        } else if (chordc[i].symbol >= KEY_LEFT_CTRL && chordc[i].symbol <= KEY_RIGHT_GUI) {
          byte mod = (1 << (chordc[i].symbol - KEY_LEFT_CTRL));
          if (mods & mod) {
            mods &= ~ mod;
            Keyboard.release(chordc[i].symbol);
          } else {
            mods |= mod;
            Keyboard.press(chordc[i].symbol);
          }
          digitalWrite(LED_LAYOUT, mods || layout != 0 ? LOW : HIGH); // LOW = light
        } else {
          Keyboard.write(chordc[i].symbol);
        }
        break;
      }
    }
    for (uint8_t i = 0; i < CHORDCC; i ++) { // Ctrl + Controls
      if (touched.list[k].chord == chordcc[i].chord) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write(chordcc[i].symbol);
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
        break;
      }
    }
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
        if (touched.list[k].chord) {
          press(k);
        }
        touched.list[k].index = 0;
        touched.list[k].chord = 0;
      }
    }
  }

  touched.last = touched.curr;
}
