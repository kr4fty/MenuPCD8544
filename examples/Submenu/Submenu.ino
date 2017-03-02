#include <PCD8544.h>
#include <AnalogKeyPad.h>
#include <MenuPCD8544.h>

/*******************************************************************
 * Definition of the "Speed" submenu
 *******************************************************************/

// number of entries in the menu
// maximum number of menu items in a menu is 5
static const int8_t NUM_ITEMS_SUBMENU_SPEED = 3;

// text for menu items, located in the program memory
const char menuSpeedItem1[] PROGMEM = "Slow";
const char menuSpeedItem2[] PROGMEM = "Medium";
const char menuSpeedItem3[] PROGMEM = "Fast";

// forward declaration of menu items' functions
void slowFunc();
void mediumFunc();
void fastFunc();

// associate menu items with their corresponding functions
const menuItem_t menuSpeedItems[NUM_ITEMS_SUBMENU_SPEED] PROGMEM =
{
    { menuSpeedItem1, &slowFunc   },
    { menuSpeedItem2, &mediumFunc },
    { menuSpeedItem3, &fastFunc   }   };

const menu_t submenuRoot PROGMEM =
{ NUM_ITEMS_SUBMENU_SPEED, menuSpeedItems };


/*******************************************************************
 * Main menu definition
 *******************************************************************/

// maximum number of menu items in a menu is 5
// if you want more you need to split your menu into submenus
const int8_t NUM_MAIN_MENU_ITEMS = 4;

// menu entries shown to the user, they are located in the program memory
const char menuMainItem1[] PROGMEM = "On";
const char menuMainItem2[] PROGMEM = "Blink";
const char menuMainItem3[] PROGMEM = "Off";
const char menuMainItem4[] PROGMEM = ":Speed..."; // the ':' indicates a submenu, it is not displayed as part of the menu item


// functions corresponding to the menu entries
void onFunc();
void blinkFunc();
void offFunc();

// menu definition: a menu is an array of tuples: (menu text, menu function)
const menuItem_t menuMainItems[NUM_MAIN_MENU_ITEMS] PROGMEM =
{
    { menuMainItem1, &onFunc },
    { menuMainItem2, &blinkFunc },
    { menuMainItem3, &offFunc },
    { menuMainItem4, (menuFunc_t) &submenuRoot } // submenus must be cast to menuFunc_t

};

// top level menu node: it consist of the number of items in the menu
//                      and a pointer to the array of menu items 
const menu_t menuRoot PROGMEM =
{ NUM_MAIN_MENU_ITEMS, menuMainItems };
    

PCD8544 lcd;
AnalogKeyPad keypad(A0);
MenuPCD8544 menuMain(&menuRoot);

const uint8_t LEDpin = 13;

void setup()
{
	Serial.begin(57600);
	Serial.println(F("Simple Menu for LCD4884 shield v1.01"));
	lcd.begin();
    menuMain.begin(&lcd);                               // show the menu
    pinMode(LEDpin, OUTPUT);
}

void loop()
{
	menuFunc_t selection;

    taskBlinkLED();                                 // run the blinking LED task
    
    selection = taskMenu();                         // run the menu task
    if ( selection != NULL)                         // menu item selected?
        (*selection)();                             // then execute it
}


// handle the menu
menuFunc_t taskMenu()
{
	menuFunc_t doit = NULL;
	int8_t keyCode;

    if ( (keyCode = keypad.keyUpEvent()) != AnalogKeyPad::NO_KEY )       // was key pressed?
    {
        lcd.clearStatus();                     // clear the status line
        // valid input keycodes: U,D,L,R,S - Up, Down, Left, Right, Select
        // returns true if menu item was selected and *doit holds the associated function pointer
        int8_t selected = menuMain.navigate(keyCode, &doit);
        
        if (selected)               		        // user selected the menu item
        {   if (doit == MenuPCD8544::canceled)      // check that the menu was not canceled
            {   doit = NULL;
                lcd.setStatus(F("menu canceled"));
                delay(2000);                        // bad, this will prevent other tasks from running
                lcd.clearStatus();
            }
            menuMain.begin(&lcd);                   // show the menu again
        }
    } // end process keycode
    return doit;
}


uint8_t blinking = 0;
uint16_t blinkInterval = 500;

void taskBlinkLED()
{   static uint32_t lastTime = 0;
    uint32_t now;
    
    if (blinking)
    {   now = millis();
        if (now > lastTime + blinkInterval)         // toggle LED
        {   digitalWrite(LEDpin, !digitalRead(LEDpin));
            lastTime = now;
        }
    }
}

void onFunc()
{
	Serial.println(F("do On"));
    digitalWrite(LEDpin, 1);
    blinking = 0;
	lcd.setStatus(F("LED on"));
}

void blinkFunc()
{
	Serial.println(F("do Blink"));
    blinking = 1;                            // LED blink interval in msec, used by manageBlinking() 
	lcd.setStatus(F("LED blink"));
}


void offFunc()
{
	Serial.println(F("do Off"));
    digitalWrite(LEDpin, 0);
    blinking = 0;
	lcd.setStatus(F("LED off"));
}

void slowFunc()
{
	Serial.println(F("do Slow"));
    blinkInterval = 750;
    lcd.setStatus(F("speed slow"));
}

void mediumFunc()
{
	Serial.println(F("do Medium"));
    blinkInterval = 300;
    lcd.setStatus(F("speed medium"));
}

void fastFunc()
{
	Serial.println(F("do Fast"));
    blinkInterval = 100;
    lcd.setStatus(F("speed fast"));
}



