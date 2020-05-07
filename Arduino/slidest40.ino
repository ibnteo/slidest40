/*
 * Touch slide keyboard Slidest40 (сенсорная слайдовая клавиатура Слайдость40)
 * Version: 0.1 beta
 * Date: 2020-05-07
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

struct Chord3 {
  uint16_t chord;
  byte symbols[5];
};

struct ChordM {
  uint16_t chord;
  String macros;
};

#define S1 0
#define S2 3
#define S3 6
#define S4 9
#define S5 12

// Chords
#define CHORD sizeof(chord) / 7
Chord3 chord[] = {
  {(1<<S1), {'a', 'A', 'f', 'F', 0}},
  {(1<<S1)|(2<<S2), {'t', 'T', 'n', 'N', 0}},
  {(1<<S1)|(3<<S2), {'c', 'C', 'r', 'R', 0}},
  {(1<<S1)|(3<<S2)|(5<<S3), {'d', 'D', 'l', 'L', 0}},
  {(1<<S1)|(2<<S2)|(4<<S3), {'g', 'G', 'u', 'U', 0}},
  {(1<<S1)|(3<<S2)|(4<<S3), {0, 0, 'z', 'Z', 0}},
  {(2<<S1), {'e', 'E', 't', 'T', 0}},
  {(2<<S1)|(1<<S2)|(3<<S3), {'k', 'K', 'x', 'X', 0}},
  {(2<<S1)|(4<<S2)|(3<<S3), {0, 0, '\'', '"', 0}},
  {(2<<S1)|(4<<S2)|(6<<S3), {'-', '-', '-', '-', 0}},
  {(2<<S1)|(4<<S2)|(6<<S3)|(4<<S4), {'"', '"', '@', '@', 0}},
  {(3<<S1), {'i', 'I', 'b', 'B', 0}},
  {(3<<S1)|(1<<S2), {'n', 'N', 'y', 'Y', 0}},
  {(3<<S1)|(5<<S2), {'s', 'S', 'c', 'C', 0}},
  {(3<<S1)|(4<<S2), {'h', 'H', 'd', 'D', 0}},
  {(3<<S1)|(1<<S2)|(2<<S3), {'b', 'B', ',', '<', 0}},
  {(3<<S1)|(4<<S2)|(2<<S3), {'j', 'J', 'q', 'Q', 0}},
  {(3<<S1)|(5<<S2)|(6<<S3), {'z', 'z', 'p', 'P', 0}},
  {(3<<S1)|(4<<S2)|(6<<S3), {'q', 'Q', 's', 'S', 0}},
  {(4<<S1), {' ', ' ', ' ', ' ', 0}},
  {(4<<S1)|(3<<S2), {'r', 'R', 'h', 'H', 0}},
  {(4<<S1)|(2<<S2)|(1<<S3), {'y', 'Y', 'm', 'M', 0}},
  {(4<<S1)|(3<<S2)|(1<<S3), {'v', 'V', ';', ':', 0}},
  {(4<<S1)|(6<<S2)|(5<<S3), {'f', 'F', 'w', 'W', 0}},
  {(4<<S1)|(3<<S2)|(5<<S3), {'w', 'W', 'i', 'I', 0}},
  {(5<<S1), {'o', 'O', 'j', 'J', 0}},
  {(5<<S1)|(3<<S2), {'l', 'L', 'k', 'K', 0}},
  {(5<<S1)|(3<<S2)|(1<<S3), {'m', 'M', 'v', 'V', 0}},
  {(5<<S1)|(3<<S2)|(4<<S3), {0, 0, '`', '~', 0}},
  {(5<<S1)|(6<<S2)|(4<<S3), {'p', 'P', 'g', 'G', 0}},
  {(6<<S1), {'u', 'U', 'e', 'E', 0}},
  {(6<<S1)|(5<<S2)|(3<<S3), {'x', 'X', '[', '{', 0}},
  {(6<<S1)|(4<<S2)|(3<<S3), {0, 0, '.', '>', 0}},
  {(6<<S1)|(4<<S2)|(2<<S3), {',', ',', '?', '?', 0}},
  {(6<<S1)|(4<<S2)|(2<<S3)|(4<<S4), {'.', '.', '/', '/', 0}},
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
  {(2<<S1)|(1<<S2), KEY_BACKSPACE},
  {(2<<S1)|(4<<S2), KEY_RETURN},
  {(4<<S1)|(6<<S2), KEY_DOWN_ARROW},
  {(1<<S1)|(2<<S2)|(1<<S3), KEY_LAYOUT},
  {(1<<S1)|(2<<S2)|(1<<S3)|(2<<S4), KEY_LAYOUT_FUNC},
  {(4<<S1)|(2<<S2), KEY_LEFT_SHIFT},
  {(4<<S1)|(2<<S2)|(4<<S3), KEY_LEFT_CTRL},
  {(4<<S1)|(2<<S2)|(4<<S3)|(6<<S4), KEY_LEFT_ALT},
  {(4<<S1)|(2<<S2)|(4<<S3)|(3<<S4), KEY_LEFT_GUI},
  {(4<<S1)|(2<<S2)|(4<<S3)|(2<<S4), KEY_RESET_MODS},
};

// Ctrl + Controls
#define CHORDCC sizeof(chordcc) / 3
Chord chordcc[] = {
  {(2<<S1)|(1<<S2)|(2<<S3), KEY_BACKSPACE},
  {(1<<S1)|(3<<S2)|(1<<S3), KEY_RETURN},
  {(5<<S1)|(6<<S2)|(5<<S3), KEY_RIGHT_ARROW},
  {(6<<S1)|(5<<S2)|(6<<S3), KEY_LEFT_ARROW},
};

// Macros
#define CHORDM sizeof(chordm) / 8
ChordM chordm[] = {
  {(1<<S1)|(3<<S2)|(4<<S3), "m1"},
  {(2<<S1)|(4<<S2)|(3<<S3), "m2"},
  {(5<<S1)|(3<<S2)|(4<<S3), "m3"},
  {(6<<S1)|(4<<S2)|(3<<S3), "m4"},
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
      } else {
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
