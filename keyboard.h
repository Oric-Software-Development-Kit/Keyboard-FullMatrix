
#define ROM

#ifdef ASSEMBLER
// Included from an assembler module
#else
// Included from a C module

// Initialization of ISR
extern void InitIRQ();

// Virtual keyboard matrix organized as follows:                     
// - byte is row number (selected through AY)  
// - bit in byte indicates the row (selected through the VIA port B) numbered 76543210 
// For mapping actual keys see:              
// http://wiki.defence-force.org/doku.php?id=oric:hardware:oric_keyboard 

extern unsigned char KeyMatrix[8];
extern unsigned char KeyRowArrows;         // One bit per key pressed (actual part of KeyMatrix)

extern unsigned char KeyAsciiUpper[8][8];
extern unsigned char KeyAsciiLower[8][8];
extern unsigned char KeyCapsLock;           // Used to force the SHIFTed status of letters (only acceptable values are 0 and 1)

extern unsigned char ReadKey();             // Reads a key (single press, but repeating) and returns his ASCII value
extern unsigned char ReadKeyNoBounce();     // Read a single key, same as before but no repeating.
#endif


// Usually it is a good idea to keep 0 all the unused
// entries, as it speeds up things. Z=1 means no key
// pressed and there is no need to look in tables later on. 
// This keys don't have an ASCII code assigned, so we will
// use consecutive values outside the usual alphanumeric 
// space.

#define KEY_LCTRL       1
#define KEY_RCTRL       2
#define KEY_LSHIFT      3
#define KEY_RSHIFT      4
#define KEY_FUNCT       5

#define KEY_LEFT        8
#define KEY_RIGHT       9
#define KEY_DOWN        10
#define KEY_UP			11

#define KEY_RETURN      13
#define KEY_ESCAPE      27
#define KEY_SPACE       32
#define KEY_DELETE      127


// Using these defines, you can easily check the content of KeyRowArrows
#define KEY_MASK_SPACE        1
#define KEY_MASK_LESS_THAN    2
#define KEY_MASK_GREATER_THAN 4
#define KEY_MASK_UP_ARROW     8
#define KEY_MASK_LEFT_SHIFT   16
#define KEY_MASK_LEFT_ARROW   32
#define KEY_MASK_DOWN_ARROW   64
#define KEY_MASK_RIGHT_ARROW  128

#define VKEY_LEFT_CONTROL     20        // Control key (left one on PC)
#define VKEY_LEFT_SHIFT       36        // Left Shift key
#define VKEY_RIGHT_SHIFT      60        // Right Shift key
#define VKEY_FUNCTION          4        // Function key (right Control key on PC)
