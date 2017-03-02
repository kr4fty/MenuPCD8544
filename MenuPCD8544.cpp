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
 * 
 * Changelog:
 * 2014-08-27:  
 *      corrected bug where the display was not properly refreshed if the 
 *      submenu contained more items then the parent menu
 */

#ifndef MenuPCD8544_h
#include "MenuPCD8544.h"
#endif

//#define DEBUG_MENU_PCD8544

int8_t MenuPCD8544::submenuTrail[] =
{ -1, -1, -1 };

MenuPCD8544::MenuPCD8544(const menu_t *menu)
{
	this->numItems = pgm_read_byte(&(menu->numItems));
	this->menu = (menuItem_t *) pgm_read_word(&(menu->items));
	this->currentItem = 0;

}

//void MenuPCD8544::begin(PCD8544 *lcd)
void MenuPCD8544::begin(Adafruit_PCD8544 *lcd)
{
#ifdef DEBUG_MENU_PCD8544
	Serial.println(F("begin:entered"));
#endif
	this->lcd = lcd;
	display();
}

void MenuPCD8544::stop()
{
    clearSubmenuTrail();
	clear();
}

int8_t MenuPCD8544::navigate(char keyCode, menuFunc_t *func )
{
	if (submenuTrail[0] == -1)                        // first call to menu?
	{
		submenuTrail[0] = 0;			// set first menu item as current item
		currentItem = 0;
		display();									// show the top menu level
#ifdef DEBUG_MENU_PCD8544
		Serial.println(F("navigate: first call"));
#endif
	}
#ifdef DEBUG_MENU_PCD8544
	Serial.print(F("navigate:keyCode=")); Serial.println(keyCode);
#endif

	int8_t result = navigateSubmenu(0, keyCode, func);
	if (submenuTrail[0] == -1)
	{	*func = canceled;
		result = 1;
#ifdef DEBUG_MENU_PCD8544
		Serial.println(F("navigate: canceled"));
#endif
	}
	if (result == 1)
	{	clear();								// remove main menu from the screen
        clearSubmenuTrail();
	}
#ifdef DEBUG_MENU_PCD8544
	Serial.print(F("navigate: result=")); Serial.println(result);
#endif
	return result;
}

int8_t MenuPCD8544::navigateSubmenu(int8_t level, char keyCode, menuFunc_t *func)
{
	int8_t oldItem;
	int8_t result = 0;

#ifdef DEBUG_MENU_PCD8544
	Serial.print(F("navigateSub:entered"));
	Serial.print(F(", level="));
	Serial.print(level);
	Serial.print(F(", item="));
	Serial.println(currentItem);
#endif

	currentItem = submenuTrail[level];

	if ((level < (MAX_MENU_DEPTH - 1)) && (submenuTrail[level + 1] != -1))
	{
		MenuPCD8544 submenu = MenuPCD8544(
				(menu_t *) pgm_read_word( &(menu[currentItem].doit) ));
		submenu.currentItem = submenuTrail[level+1];
		submenu.begin(lcd);
		result = submenu.navigateSubmenu(level + 1, keyCode, func);
		if (submenuTrail[level+1] == -1)				// was submenu canceled?
			display();
	}
	else
		switch (keyCode)
		{
		case 'U':                                               // key UP
			oldItem = currentItem;
			currentItem--;
			if (currentItem < 0)
				currentItem = numItems - 1;
			submenuTrail[level] = currentItem;					// save the current item
			displayItem(oldItem);
			displayItem(currentItem);
			result = 0;
			break;

		case 'D':                                               // key DOWN
			oldItem = currentItem;
			currentItem = (currentItem + 1) % numItems;
			submenuTrail[level] = currentItem;					// save the current item
			displayItem(oldItem);
			displayItem(currentItem);
			result = 0;
			break;

		case 'L':                                               // key LEFT
            clear();
			submenuTrail[level] = -1;					        // return from submenu
			result = 0;
			break;

		case 'R':                                               // key RIGHT
		case 'S':                                               // key SELECT
		{
			PGM_P menuItemText = getItemText(currentItem);
			if (isSubmenu(menuItemText))
			{	submenuTrail[level+1] = 0;                      // remember we're in the submenu, select the first item
				clear();                                        // now display the submenu
				MenuPCD8544 submenu = MenuPCD8544((menu_t *) pgm_read_word( &(menu[currentItem].doit) ));
				submenu.begin(lcd);
				submenu.display();
#ifdef DEBUG_MENU_PCD8544
				Serial.println(F("submenu displayed"));
#endif
				result = 0;// no selection done yet
			}
			else
			{	*func = (menuFunc_t) pgm_read_word( &(menu[currentItem].doit) );
                clear();
				result = 1;
			}
			break;
		}
		default:
			result = 0;							// unknown key, no selection is made
			break;
	} // end SELECT
#ifdef DEBUG_MENU_PCD8544
	Serial.print(F("navigateSub:exit"));
	Serial.print(F(", level="));
	Serial.print(level);
	Serial.print(F(", item="));
	Serial.print(currentItem);
	Serial.print(F(", result="));
	Serial.println(result);
#endif
	return result;
}

void MenuPCD8544::display()
{
	for (int8_t i = 0; i < numItems; i++)
	{
		this->lcd->setCursor(0, i*8+1);
		///lcd->clearLine();
                this->lcd->print("             ");
		displayItem(i);
	}
}

void MenuPCD8544::clear()
{
	for (int8_t i = 0; i < numItems; i++)
	{	this->lcd->setCursor(0, i*8+1);
		//lcd->clearLine();
                this->lcd->print("             ");
	}
}


void MenuPCD8544::clearSubmenuTrail()
{
    currentItem = 0;
    for (uint8_t i=0; i<MAX_MENU_DEPTH; i++)
        submenuTrail[i] = -1; 				// no items selected at any level
}

void MenuPCD8544::displayItem(int8_t i)
{
	PGM_P menuItemText;
	
	//lcd->setInverse(i == currentItem);
        if(i == currentItem)
            lcd->setTextColor(WHITE, BLACK);
        
        this->lcd->setCursor(0,i*8+1);
        this->lcd->print("             ");
        this->lcd->setCursor(0,i*8+1);

	menuItemText = getItemText(i);
	if (isSubmenu(menuItemText)) menuItemText++;
	this->lcd->print(FS(menuItemText));
#ifdef DEBUG_MENU_PCD8544
	Serial.write(i == currentItem ? '*' : ' ');
	Serial.println(FS(menuItemText));
#endif
	//lcd->setInverse(false); //TODO: restore the previous state instead of forcing the normal mode
        this->lcd->setTextColor(BLACK, WHITE);
        
        this->lcd->drawLine(0, (i+1)*8, lcd->width()-7, (i+1)*8, BLACK);
        this->lcd->drawLine(lcd->width()-7, i*8, lcd->width()-7, (i+1)*8, BLACK);
        
        this->lcd->display();
}

PGM_P MenuPCD8544::getItemText(int8_t i)
{
	return (PGM_P)pgm_read_word( &(menu[i].item));
}

boolean MenuPCD8544::isSubmenu(PGM_P menuItemText)
{
	if (pgm_read_byte(menuItemText) == ':')
	    return true;
	else
	    return false;
}

