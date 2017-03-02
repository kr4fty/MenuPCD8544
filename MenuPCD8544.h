/*
 * MenuPCD8544 - Simple menus for LCD4884 shield
 *               (Nokia 5110 display + 5-way analog button)
 *
 * Copyright (c) 2014 oryshed <oryshed@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef MenuPCD8544_h
#define MenuPCD8544_h

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <avr/pgmspace.h>

//#include <AnalogKeyPad.h>
#include <Adafruit_GFX.h>
//#include <PCD8544.h>
#include <Adafruit_PCD8544.h>

#define FS(x) (__FlashStringHelper*)(x)

typedef void (*menuFunc_t)() PROGMEM;

struct menuItem_t
{ const char *item;
  menuFunc_t doit;
} PROGMEM;

struct menu_t
{ int8_t numItems;
  const struct menuItem_t *items;
} PROGMEM;

/** Simple menus for the LCD4884 shield
 *
 * Beware: maximum number of menu items in the menu is 5.
 * You can have only one instance of this class.
 *
 */
class MenuPCD8544
{
  public:
    MenuPCD8544(const menu_t * menu);
//    void begin(PCD8544 *lcd, AnalogKeyPad *keypad);
    void begin(Adafruit_PCD8544 *lcd);
    void stop();
    int8_t navigate(char keyCode, menuFunc_t *func);
    void display();
    void clear();
    inline static void canceled() { };
  
  private:
    static const int8_t STATUS_LINE = 5;
    static const uint8_t MAX_MENU_DEPTH = 3;

    int8_t navigateSubmenu(int8_t level, char keyCode, menuFunc_t *func);
    void clearSubmenuTrail();
    void displayItem(int8_t i);
    PGM_P getItemText(int8_t i);
    static boolean isSubmenu(PGM_P menuItemText);
    menuItem_t *menu;
    int8_t numItems;
    //PCD8544 *lcd;
    Adafruit_PCD8544 *lcd;
    //AnalogKeyPad *keypad;
    int8_t currentItem;
    static int8_t submenuTrail[MAX_MENU_DEPTH];

};

#endif
