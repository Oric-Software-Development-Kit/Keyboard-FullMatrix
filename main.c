// --------------------------------------
// Complete Keyboard Matrix Read
// All keys are scanned and a  virtual matrix 
// of 8 bytes is updated at each IRQ.     
// --------------------------------------
// (c) 2010-2024 Defence Force
// Authors: Twilighte, Chema and Dbug
// --------------------------------------
// This code is provided as-is:
// We do not assume any responsability concerning the fact this is a bug-free software !!!
// You can use this example code without any limitation, but if you do, it would be nice
// if you could mention the origin somewhere and inform us :)
// --------------------------------------
// For more information, please contact us on internet:
// e-mail: mike@defence-force.org
// URL: http://www.defence-force.org
// --------------------------------------
// Note: This text was typed with a Win32 editor. So perhaps the text will not be
// displayed correctly with other OS.

#include <stdio.h>
#include <lib.h>
#include "keyboard.h"



// Given a position in the 8x8 matrix, returns if the key at this location is pressed or not.
// This is not 100% accuracte and depends of how many keys are pressed, and which rows they are on.
// Don't test only on PC and expect that to work on the Oric, beyond two key pressed, you are in dangerous waters.
unsigned char IsPressed(unsigned char matrix_position)
{
    unsigned char bitfield;
    unsigned char bitkey;

    bitkey=1 << (matrix_position&7);
    bitfield=KeyMatrix[matrix_position>>3];
    return bitfield & bitkey;    
}

// Given an ASCII code, returns a displayable string representing the key
char* GetKeyName(unsigned char asciiCode)
{
    if (!asciiCode)
    {
        // Nothing pressed
        return "<none>";
    }
    else
    if ( (asciiCode>32) && (asciiCode<127) )
    {
        // Displayable keys (letters, numbers, ...)
        static char buffer[2];
        buffer[0]=asciiCode;
        buffer[1]=0;
        return buffer;
    }
    else
    {
        switch (asciiCode)
        {
        // Modifier keys
        case KEY_LCTRL:     return "LEFT CTRL";
        case KEY_RCTRL:     return "RIGHT CTRL";
        case KEY_LSHIFT:    return "LEFT SHIFT";
        case KEY_RSHIFT:    return "RIGHT SHIFT";
        case KEY_FUNCT:     return "FUNCTION";

        // Arrow keys
        case KEY_LEFT:      return "LEFT ARROW";
        case KEY_RIGHT:     return "RIGHT ARROW";
        case KEY_DOWN:      return "DOWN ARROW";
        case KEY_UP:        return "UP ARROW";

        // Special keys
        case KEY_RETURN:    return "RETURN";
        case KEY_ESCAPE:    return "ESCAPE";
        case KEY_SPACE:     return "SPACE";
        case KEY_DELETE:    return "DELETE";
        }
    }
    return "<unknown>";
}

// Update a line at the bottom of the screen showing the status of the various modifier keys, CAPS Lock, etc...
void ShowCurrentKeyStatus()
{    
    unsigned char key=ReadKey();
    sprintf((char*)(0xbb80+40*12),"%c%cReadKey: %c'%s' (%d)    ",16+6,4,5,GetKeyName(key),key);

    sprintf((char*)(0xbb80+40*12+36),"%s",KeyCapsLock?"CAPS":"caps");

    // Show the status of the Control, Shift and Function key at the bottom of the screen
    sprintf((char*)(0xbb80+40*12+30),"%c%c%c%c"
        ,IsPressed(VKEY_LEFT_CONTROL)?'C':'-'
        ,IsPressed(VKEY_LEFT_SHIFT)?'S':'-'
        ,IsPressed(VKEY_RIGHT_SHIFT)?'S':'-'
        ,IsPressed(VKEY_FUNCTION)?'F':'-');        

}

// Shows the 8x8 keyboard matrix, in either UPPER or LOWER case depending of the Shift keys status
void ShowKeyboardMatrix()
{
    unsigned char* key;
    unsigned char display;
    char* start;
    char i,row;
    char mask;
     
    if (IsPressed(VKEY_LEFT_SHIFT) || IsPressed(VKEY_RIGHT_SHIFT))
    {
        // Shifted
        key = &KeyAsciiLower[0][0];
    }
    else
    {
        // Unshifted
        key = &KeyAsciiUpper[0][0];
    }
    start=(char*)(0xbb80+40*4-11);
    for (row=0;row<8;row++)
    {
        *start++ = 2;
        *start++ = '0'+row;
        mask=KeyMatrix[row];
        for (i=0;i<8;i++)
        {
            display=*key++;
            if ( (display<32) || (display>=128) )
            {
                display='.';
            }
            if (!(mask & 1))
            {
                display |=128;
            }
            *start++ = display;
            mask >>=1;
        }
        
        start+=40-10;
    }
}


char EditField[36*4];
char EditFieldSize=0;

void DisplayInputField()
{
    int y;
    char* screen;
    char* buffer;
    buffer=EditField;
    screen=(char*)(0xbb80+40*24);
    for (y=0;y<4;y++)
    {
        screen[0] = 16+7;
        screen[1] = 0;
        memcpy(screen+2,buffer,36);
        buffer+=36;
        screen+=40;
    }
}

void UpdateInputField()
{
    char key;

    // Check if we have new keys, and do something about it
    key = ReadKeyNoBounce();
    if (key)
    {
        if (key==KEY_DELETE)
        {
            if (EditFieldSize>0)
            {
                EditFieldSize--;
                EditField[EditFieldSize]=' ';
            }
        }
        else
        if ( (key>=32) && (key<128) )            
        {
            if ( ( (key=='T') || (key=='t') ) && IsPressed(VKEY_LEFT_CONTROL) )
            {
                KeyCapsLock=!KeyCapsLock;
            }
            else
            {                
                EditField[EditFieldSize]=key;
                EditFieldSize++;
            }
        }
        DisplayInputField();
    }
}


void main()
{
    setflags(SCREEN); // So we don't get the blinking cursor frozen after we disabled the IRQ
    InitIRQ();
    paper(4);
    ink(6);
    cls();
    sprintf((char*)(0xbb80),"%c%c Complete Keyboard Matrix Read Demo%c",16+3,1,3);

    printf("\n");
    printf("                             01234567\n");
    printf("Space       \033B\174001\174          0\n");
    printf("< ,         \033B\174002\174          1\n");
    printf("> .         \033B\174004\174          2\n");
    printf("Up Arrow    \033B\174008\174          3\n");
    printf("Left Shift  \033B\174016\174<---      4\n");
    printf("Left Arrow  \033B\174032\174          5\n");
    printf("Down Arrow  \033B\174064\174          6\n");
    printf("Right Arrow \033B\174128\174          7\n");


    printf("\n");
    printf("\n");
    printf("\n");
	printf("This virtual matrix is updated by\n");
	printf("interrupts 50 times per second.\n\n");

    printf("Each row uses one byte in the matrix,\n");
    printf("and each bit represents a single key.\n\n");

    printf("Use CTRL-T to change CAPS status\n\n");

    printf("Practice Edit Field:\n\n");
    DisplayInputField();
    
    while (1)
    {
        poke(0xbb80+40*3+1,(KeyRowArrows & KEY_MASK_SPACE)?1:3);
        poke(0xbb80+40*4+1,(KeyRowArrows & KEY_MASK_LESS_THAN)?1:3);
        poke(0xbb80+40*5+1,(KeyRowArrows & KEY_MASK_GREATER_THAN)?1:3);
        poke(0xbb80+40*6+1,(KeyRowArrows & KEY_MASK_UP_ARROW)?1:3);
        poke(0xbb80+40*7+1,(KeyRowArrows & KEY_MASK_LEFT_SHIFT)?1:3);
        poke(0xbb80+40*8+1,(KeyRowArrows & KEY_MASK_LEFT_ARROW)?1:3);
        poke(0xbb80+40*9+1,(KeyRowArrows & KEY_MASK_DOWN_ARROW)?1:3);
        poke(0xbb80+40*10+1,(KeyRowArrows & KEY_MASK_RIGHT_ARROW)?1:3);

        sprintf((char *)(0xbb80+40*7+25),"%u  ",KeyRowArrows);        

        ShowKeyboardMatrix();

        ShowCurrentKeyStatus();

        UpdateInputField();

    }   
}

