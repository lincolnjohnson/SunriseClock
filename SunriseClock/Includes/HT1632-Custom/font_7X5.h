/* 
 * 7-high FONT FOR RENDERING TO THE LED SCREEN.
 * Includes kerning support
 * Joeri Gantois, 2011
 */

#ifndef __FONT7X5_H
#define __FONT7X5_H

#define FONT_7X5_HEIGHT  8

const unsigned char FONT_7X5 [] PROGMEM = {
0b00000000, 0b00000000, 0b00000000,  //  
  0b01111001,  // !
  0b01110000, 0b00000000, 0b01110000,  // "
  0b00101000, 0b01111100, 0b00101000, 0b01111100, 0b00101000,  // #
  0b00010010, 0b00101010, 0b01111111, 0b00101010, 0b00100100,  // $
  0b01100010, 0b01100100, 0b00001000, 0b00010011, 0b00100011,  // %
  0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b00000101,  // &
  0b01010000, 0b01100000,  // '
  0b00011100, 0b00100010, 0b01000001,  // (
  0b01000001, 0b00100010, 0b00011100,  // )
  0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,  // *
  0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,  // +
  0b00000101, 0b00000110,  // ,
  0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,  // -
  0b00000011, 0b00000011,  // .
  0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000,  // /
  0b01111111, 0b01000001, 0b01111111,  // 0
  0b00000000, 0b01111111, 0b00000000,  // 1
  0b01001111, 0b01001001, 0b01111001,  // 2
  0b01001001, 0b01001001, 0b01111111,  // 3
  0b01111000, 0b00001000, 0b01111111,  // 4
  0b01111001, 0b01001001, 0b01001111,  // 5
  0b01111111, 0b01001001, 0b01001111,  // 6
  0b01000000, 0b01000000, 0b01111111,  // 7
  0b01111111, 0b01001001, 0b01111111,  // 8
  0b01111001, 0b01001001, 0b01111111,  // 9
  0b00110110, 0b00110110, 0b00000000,  // :
  0b00110101, 0b00110110, 0b00000000,  // ;
  0b00001000, 0b00010100, 0b00100010, 0b00000000,  // <
  0b00010100, 0b00010100, 0b00010100, 0b00010100,  // =
  0b00100010, 0b00010100, 0b00001000, 0b00000000,  // >
  0b00100000, 0b01000000, 0b01000101, 0b01001000, 0b00110000,  // ?
  0b00100110, 0b01001001, 0b01001111, 0b01000001, 0b00111110,  // @
  0b00111111, 0b01001000, 0b01001000, 0b00111111,  // A
  0b01111111, 0b01001001, 0b01001001, 0b00110110,  // B
  0b00111110, 0b01000001, 0b01000001, 0b00100010,  // C
  0b01111111, 0b01000001, 0b01000001, 0b00111110,  // D
  0b01111111, 0b01001001, 0b01001001, 0b01000001,  // E
  0b01111111, 0b01001000, 0b01001000, 0b01000000,  // F
  0b00111110, 0b01000001, 0b01001001, 0b00101110,  // G
  0b01111111, 0b00001000, 0b00001000, 0b01111111,  // H
  0b01000001, 0b01111111, 0b01000001, 0b00000000,  // I
  0b00000010, 0b00000001, 0b01000001, 0b01111110,  // J
  0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,  // K
  0b01111111, 0b00000001, 0b00000001, 0b00000001,  // L
  0b01111111, 0b00100000, 0b00011000, 0b00100000, 0b01111111,  // M
  0b01111111, 0b00010000, 0b00001000, 0b00000100, 0b01111111,  // N
  0b00111110, 0b01000001, 0b01000001, 0b00111110,  // O
  0b01111111, 0b01001000, 0b01001000, 0b00110000,  // P
  0b00111110, 0b01000001, 0b01000101, 0b01000010, 0b00111101,  // Q
  0b01111111, 0b01001000, 0b01001000, 0b00110111,  // R
  0b00110001, 0b01001001, 0b01001001, 0b01000110,  // S
  0b01000000, 0b01000000, 0b01111111, 0b01000000, 0b01000000,  // T
  0b01111110, 0b00000001, 0b00000001, 0b01111110,  // U
  0b01111100, 0b00000010, 0b00000001, 0b00000010, 0b01111100,  // V
  0b01111110, 0b00000001, 0b00001110, 0b00000001, 0b01111110,  // W
  0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,  // X
  0b01110000, 0b00001000, 0b00000111, 0b00001000, 0b01110000,  // Y
  0b01000011, 0b01000101, 0b01001001, 0b01010001, 0b01100001,  // Z
  0b01111111, 0b01000001, 0b01000001,  // [
  0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,  // backslash
  0b01000001, 0b01000001, 0b01111111,  // ]
  0b00010000, 0b00100000, 0b01000000, 0b00100000, 0b00010000,  // ^
  0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001,  // _
};

int FONT_7X5_END [] = {
     3,   4,   7,  12,  17,  22,  27,  29,
    32,  35,  40,  45,  47,  52,  54,  59,
    62,  65,  68,  71,  74,  77,  80,  83,
    86,  89,  92,  95,  99, 103, 107, 112,
   117, 121, 125, 129, 133, 137, 141, 145,
   149, 153, 157, 162, 166, 171, 176, 180,
   184, 189, 193, 197, 202, 206, 211, 216,
   221, 226, 231, 234, 239, 242, 247, 252
};

#endif