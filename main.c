// --------------------------------------
// Complete Keyboard Matrix Read
// All keys are scanned and a  virtual matrix 
// of 8 bytes is updated at each IRQ.     
// --------------------------------------
// (c) 2010-2020 Defence Force
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
        case KET_RCTRL:     return "RIGHT CTRL";
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


void ShowCurrentKeyStatus()
{    
    unsigned char key=ReadKey();
    sprintf((char*)(0xbb80+40*14),"%c%cReadKey: %c'%s' (%d)        ",16+6,4,5,GetKeyName(key),key);
}


void ShowKeyboardMatrix()
{
    unsigned char* key;
    unsigned char display;
    char* start;
    char i,row;
    char mask;
     
    key = &KeyAscii[0][0];
    start=(char*)(0xbb80+40*3-10);
    for (row=0;row<8;row++)
    {
        *start++ = 2;
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
        
        start+=40-9;
    }
}


char EditField[36*4];
unsigned char EditFieldSize=0;

void UpdateInputField()
{
    int y;
    char* screen;
    char* buffer;
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
                EditField[EditFieldSize]='.';
            }
        }
        else
        if ( (key>=32) && (key<128) )            
        {
            EditField[EditFieldSize]=key;
            EditFieldSize++;
        }
    }

    // Display the buffer
    buffer=EditField;
    screen=(char*)(0xbb80+40*18);
    for (y=0;y<4;y++)
    {
        screen[0] = 16+6;
        screen[1] = 4;
        memcpy(screen+2,buffer,36);
        buffer+=36;
        screen+=40;
    }
}

void main()
{
    setflags(SCREEN); // So we don't get the blinking cursor frozen after we disabled the IRQ
    paper(4);
    ink(6);
    cls();
    sprintf((char*)(0xbb80),"%c%c Complete Keyboard Matrix Read Demo%c",16+3,1,3);

    printf("\n");
    printf("This virtual matrix is\n");
    printf("updated by interrupts 50\n");
    printf("times per second.\n\n");

    printf("Each of the 8 rows uses one\n");
    printf("byte in the KeyMatrix. \n\n");

    printf("Each of the 8 bit represents\n");
    printf("activity in a given column\n\n");

    printf("Multiple keypresses are supported.\n");

    printf("\n\n\nPractice Edit Field:\n\n");

    InitIRQ();

    memset(EditField,'.',sizeof(EditField));
    while (1)
    {
        ShowKeyboardMatrix();

        ShowCurrentKeyStatus();

        UpdateInputField();
    }   
}

