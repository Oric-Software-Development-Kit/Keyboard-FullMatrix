
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
extern unsigned char KeyAscii[8][8];

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
#define KET_RCTRL       2
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

