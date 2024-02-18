
#include "keyboard.h"

#define ROM

#ifdef ROM
#define IRQ_ADDRLO $0245
#define IRQ_ADDRHI $0246
#else
#define IRQ_ADDRLO $fffe
#define IRQ_ADDRHI $ffff
#endif

#define        via_portb               $0300 
#define		   via_ddrb				   $0302	
#define		   via_ddra				   $0303
#define        via_t1cl                $0304 
#define        via_t1ch                $0305 
#define        via_t1ll                $0306 
#define        via_t1lh                $0307 
#define        via_t2ll                $0308 
#define        via_t2ch                $0309 
#define        via_sr                  $030A 
#define        via_acr                 $030b 
#define        via_pcr                 $030c 
#define        via_ifr                 $030D 
#define        via_ier                 $030E 
#define        via_porta               $030f 

#define KEY_FIRST_ASCII 6      // Values before that one are the modifier keys

    .zero

irq_A               .dsb 1
irq_X               .dsb 1
irq_Y               .dsb 1
zpTemp01			.dsb 1
zpTemp02			.dsb 1
tmprow				.dsb 1

    .text 

_KeyMatrix            .dsb 4     ; The virtual Key Matrix (top half)
_KeyRowArrows         .dsb 4     ; The virtual Key Matrix (bottom half starting on the row with the arrows and the space bar)
_KeyCapsLock          .byt 1     ; By default we use CAPS letter

KeyShiftPressed       .byt 0     ; Status of any of the SHIFT keys
KeyLastPressed        .dsb 1     ; For the debouncing
KeyCurrentPressed     .dsb 1     ; For the debouncing

; Regarding SHIFT and CAPS LOCK:
; - SHIFT does impact all the keys (letters, symbols, numbers)
; - CAPS LOCK only impacts the actual letters

; Some more routines, not actualy needed, but quite useful
; for reading a single key (get the first active bit in 
; the virtual matrix) and returning his ASCII value.
; Should serve as an example about how to handle the keymap.
; Both _ReadKey and _ReadKeyNoBounce can be used directly from 
; C, declared as:


_KeyAsciiUpper
    .asc "7","N","5","V",KEY_RCTRL,"1","X","3"
    .asc "J","T","R","F",0,KEY_ESCAPE,"Q","D"
    .asc "M","6","B","4",KEY_LCTRL,"Z","2","C"
    .asc "K","9",59,"-",0,0,92,39
    .asc " ",",",".",KEY_UP,KEY_LSHIFT,KEY_LEFT,KEY_DOWN,KEY_RIGHT
    .asc "U","I","O","P",KEY_FUNCT,KEY_DELETE,"]","["
    .asc "Y","H","G","E",0,"A","S","W"
    .asc "8","L","0","\",KEY_RSHIFT,KEY_RETURN,0,"="

_KeyAsciiLower
    .asc "&","n","%","v",KEY_RCTRL,"!","x","#"
    .asc "j","t","r","f",0,KEY_ESCAPE,"q","d"
    .asc "m","^","b","$",KEY_LCTRL,"z","@","c"
    .asc "k","(",59,95,0,0,92,39
    .asc " ","<",">",KEY_UP,KEY_LSHIFT,KEY_LEFT,KEY_DOWN,KEY_RIGHT
    .asc "u","i","o","p",KEY_FUNCT,KEY_DELETE,"}","{"
    .asc "y","h","g","e",0,"a","s","w"
    .asc "*","l",")","|",KEY_RSHIFT,KEY_RETURN,0,"="


_InitIRQ
.(
    ; Since we are starting from when the standard irq has already been 
    ; setup, we need not worry about ensuring one irq event and/or right 
    ; timer period, only redirecting irq vector to our own irq handler. 
    sei
    ; Setup DDRA, DDRB and ACR
    lda #%11111111
    sta via_ddra
    lda #%11110111 ; PB0-2 outputs, PB3 input.
    sta via_ddrb
    lda #%1000000
    sta via_acr

    ; Since this is an slow process, we set the VIA timer to 
    ; issue interrupts at 25Hz, instead of 100 Hz. This is 
    ; not necessary -- it depends on your needs
    lda #<40000
    sta via_t1ll 
    lda #>40000
    sta via_t1lh

    ; Patch IRQ vector
    lda #<irq_routine 
    sta IRQ_ADDRLO
    lda #>irq_routine 
    sta IRQ_ADDRHI
    cli 
    rts 
.)



irq_routine 
.(
    ; Preserve registers 
    sta irq_A
    stx irq_X
    sty irq_Y

    ; Clear IRQ event 
    lda via_t1cl 

    ; Process keyboard 
    jsr ReadKeyboard 

    ; Restore Registers 
    lda irq_A
    ldx irq_X
    ldy irq_Y

    ; End of IRQ 
    rti 
.)



ReadKeyboard
.(
    ; Write Column Register Number to PortA 
    lda #$0E 
    sta via_porta 

    ; Tell AY this is Register Number 
    lda #$FF 
    sta via_pcr 

    ; Clear CB2, as keeping it high hangs on some orics.
    ; Pitty, as all this code could be run only once, otherwise
    ldy #$dd 
    sty via_pcr 

    ldx #7 

loop_row   ;Clear relevant bank 
    lda #00 
    sta _KeyMatrix,x 

    ; Write 0 to Column Register 

    sta via_porta 
    lda #$fd 
    sta via_pcr 
    lda #$dd
    sta via_pcr


    lda via_portb 
    and #%11111000
    stx zpTemp02
    ora zpTemp02 
    sta via_portb 


    ; Wait 10 cycles for circuit to settle on new row 
    ; Use time to load inner loop counter and load Bit 

    ; CHEMA: Fabrice Broche uses 4 cycles (lda #8:inx) plus
    ; the four cycles of the and absolute. That is 8 cycles.
    ; So I guess that I could do the same here (ldy,lda)

    ldy #$80
    lda #8 

    ; Sense Row activity 
    and via_portb 
    beq skip2 

    ; Store Column 
    tya
loop_column   
    eor #$FF 

    sta via_porta 
    lda #$fd 
    sta via_pcr 
    lda #$dd
    sta via_pcr

    lda via_portb 
    and #%11111000
    stx zpTemp02
    ora zpTemp02 
    sta via_portb 


    ; Use delay(10 cycles) for setting up bit in _KeyMatrix and loading Bit 
    tya 
    ora _KeyMatrix,x 
    sta zpTemp01 
    lda #8 

    ; Sense key activity 
    and via_portb 
    beq skip1 

    ; Store key 
    lda zpTemp01 
    sta _KeyMatrix,x 

skip1   ;Proceed to next column 
    tya 
    lsr 
    tay 
    bcc loop_column 

skip2   ;Proceed to next row 
    dex 
    bpl loop_row 

    rts 
.)  



; Reads a key (single press, but repeating) and returns his ASCII value in reg X. 
; Z=1 if no keypress detected.
_ReadKey
.(
    ; Start by checking modifiers... because that modifies the keys...    
    ldx #1

    lda _KeyMatrix+(VKEY_LEFT_SHIFT/8)
    and #1 << (VKEY_LEFT_SHIFT & 7)
    bne shift_pressed
    lda _KeyMatrix+(VKEY_RIGHT_SHIFT/8)
    and #1 << (VKEY_RIGHT_SHIFT & 7)
    bne shift_pressed

    ldx #0

+shift_pressed
    txa
    sta KeyShiftPressed

    ; Then we do the proper matrix scan
    ldx #7
loop_row
    lda _KeyMatrix,x
    beq next_row

    sta tmprow

    txa
    asl
    asl
    asl
    tay 

    lda tmprow
loop_column
    iny
    lsr tmprow
    bcc loop_column

    lda _KeyAsciiLower-1,y
    cmp #KEY_FIRST_ASCII
    bcs ascii_key

not_ascii_key
    lda tmprow
    bne loop_column

next_row
    dex
    bpl loop_row

    ldx #0
    stx KeyCurrentPressed
    rts

ascii_key
    sta KeyCurrentPressed
    cmp #97              // 'a'
    bcc not_letter
    cmp #122+1           // 'z'
    bcs not_letter
    ; For actual letters, we need to take into consideration both CAPS LOCK and SHIFT keys
    pha
    lda KeyShiftPressed
    eor _KeyCapsLock
    sta KeyShiftPressed
    pla

not_letter
    ; For non letter character, we only use the SHIFT Status
    asl KeyShiftPressed
    beq not_shifted
    lda _KeyAsciiUpper-1,y
not_shifted

    tax
    rts
.)

; Read a single key, same as before but no repeating.
_ReadKeyNoBounce
.(
    jsr _ReadKey
    lda KeyCurrentPressed
    cmp KeyLastPressed
    beq retz
    sta KeyLastPressed
    rts
retz
    ldx #0
    rts
.)


