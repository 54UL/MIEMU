# gbdev.io/pandocs reference
- link:
https://gbdev.io/pandocs

# GB SOC manual:
https://gekkio.fi/files/gb-docs/gbctr.pdf

## HARDWARE REGISTERS
| Address | Name        | Description                                              | Readable / Writable | Models |
| ------- | ----------- | -------------------------------------------------------- | ------------------- | ------ |
| $FF00   | P1/JOYP     | Joypad                                                   | Mixed               | All    |
| $FF01   | SB          | Serial transfer data                                     | R/W                 | All    |
| $FF02   | SC          | Serial transfer control                                  | R/W                 | Mixed  |
| $FF04   | DIV         | Divider register                                         | R/W                 | All    |
| $FF05   | TIMA        | Timer counter                                            | R/W                 | All    |
| $FF06   | TMA         | Timer modulo                                             | R/W                 | All    |
| $FF07   | TAC         | Timer control                                            | R/W                 | All    |
| $FF0F   | IF          | Interrupt flag                                           | R/W                 | All    |
| $FF10   | NR10        | Sound channel 1 sweep                                    | R/W                 | All    |
| $FF11   | NR11        | Sound channel 1 length timer & duty cycle               | Mixed               | All    |
| $FF12   | NR12        | Sound channel 1 volume & envelope                        | R/W                 | All    |
| $FF13   | NR13        | Sound channel 1 period low                               | W                   | All    |
| $FF14   | NR14        | Sound channel 1 period high & control                    | Mixed               | All    |
| $FF16   | NR21        | Sound channel 2 length timer & duty cycle               | Mixed               | All    |
| $FF17   | NR22        | Sound channel 2 volume & envelope                        | R/W                 | All    |
| $FF18   | NR23        | Sound channel 2 period low                               | W                   | All    |
| $FF19   | NR24        | Sound channel 2 period high & control                    | Mixed               | All    |
| $FF1A   | NR30        | Sound channel 3 DAC enable                               | R/W                 | All    |
| $FF1B   | NR31        | Sound channel 3 length timer                             | W                   | All    |
| $FF1C   | NR32        | Sound channel 3 output level                             | R/W                 | All    |
| $FF1D   | NR33        | Sound channel 3 period low                               | W                   | All    |
| $FF1E   | NR34        | Sound channel 3 period high & control                    | Mixed               | All    |
| $FF20   | NR41        | Sound channel 4 length timer                             | W                   | All    |
| $FF21   | NR42        | Sound channel 4 volume & envelope                        | R/W                 | All    |
| $FF22   | NR43        | Sound channel 4 frequency & randomness                   | R/W                 | All    |
| $FF23   | NR44        | Sound channel 4 control                                  | Mixed               | All    |
| $FF24   | NR50        | Master volume & VIN panning                              | R/W                 | All    |
| $FF25   | NR51        | Sound panning                                            | R/W                 | All    |
| $FF26   | NR52        | Sound on/off                                             | Mixed               | All    |
| $FF30-FF3F | Wave RAM  | Storage for one of the sound channels’ waveform         | R/W                 | All    |
| $FF40   | LCDC        | LCD control                                              | R/W                 | All    |
| $FF41   | STAT        | LCD status                                               | Mixed               | All    |
| $FF42   | SCY         | Viewport Y position                                      | R/W                 | All    |
| $FF43   | SCX         | Viewport X position                                      | R/W                 | All    |
| $FF44   | LY          | LCD Y coordinate                                         | R                   | All    |
| $FF45   | LYC         | LY compare                                               | R/W                 | All    |
| $FF46   | DMA         | OAM DMA source address & start                           | R/W                 | All    |
| $FF47   | BGP         | BG palette data                                          | R/W                 | DMG    |
| $FF48   | OBP0        | OBJ palette 0 data                                       | R/W                 | DMG    |
| $FF49   | OBP1        | OBJ palette 1 data                                       | R/W                 | DMG    |
| $FF4A   | WY          | Window Y position                                        | R/W                 | All    |
| $FF4B   | WX          | Window X position plus 7                                 | R/W                 | All    |
| $FF4D   | KEY1        | Prepare speed switch                                     | Mixed               | CGB    |
| $FF4F   | VBK         | VRAM bank                                                | R/W                 | CGB    |
| $FF51   | HDMA1       | VRAM DMA source high                                     | W                   | CGB    |
| $FF52   | HDMA2       | VRAM DMA source low                                      | W                   | CGB    |
| $FF53   | HDMA3       | VRAM DMA destination high                                | W                   | CGB    |
| $FF54   | HDMA4       | VRAM DMA destination low                                 | W                   | CGB    |
| $FF55   | HDMA5       | VRAM DMA length/mode/start                               | R/W                 | CGB    |
| $FF56   | RP          | Infrared communications port                             | Mixed               | CGB    |
| $FF68   | BCPS/BGPI   | Background color palette specification / Background palette index | R/W          | CGB    |
| $FF69   | BCPD/BGPD   | Background color palette data / Background palette data  | R/W                 | CGB    |
| $FF6A   | OCPS/OBPI   | OBJ color palette specification / OBJ palette index      | R/W                 | CGB    |
| $FF6B   | OCPD/OBPD   | OBJ color palette data / OBJ palette data                | R/W                 | CGB    |
| $FF6C   | OPRI        | Object priority mode                                     | R/W                 | CGB    |
| $FF70   | SVBK        | WRAM bank                                                | R/W                 | CGB    |
| $FF76   | PCM12       | Audio digital outputs 1 & 2                             | R                   | CGB    |
| $FF77   | PCM34       | Audio digital outputs 3 & 4                             | R                   | CGB    |
| $FFFF   | IE          | Interrupt enable                                         | R/W                 | All    |
## Interrupts

IME: Interrupt master enable flag [write only]
```
0 - Disable all interrupts
1 - Enable all interrupts that are enabled in the IE register (FFFF)
```
The IME flag is used to disable all interrupts, overriding any enabled bits in the IE register. It isn’t possible to access the IME flag by using an I/O address. IME can be modified by the following instructions/events only:

```
EI     ; Enables interrupts  (that is, IME=1)
DI     ; Disables interrupts (that is, IME=0)
RETI   ; Enables interrupts and returns (same as the instruction sequence EI, RET)
<ISR>  ; Disables interrupts and calls the interrupt handler
Where <ISR> is the Interrupt Service Routine that is automatically executed by the CPU when it services an interrupt request.
```

The effect of ei is delayed by one instruction. This means that ei followed immediately by di does not allow any interrupts between them. This interacts with the halt bug in an interesting way.

### FFFF — IE: Interrupt enable
```
Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)
```

### FF0F — IF: Interrupt flag
```
Bit 0: VBlank   Interrupt Request (INT $40)  (1=Request)
Bit 1: LCD STAT Interrupt Request (INT $48)  (1=Request)
Bit 2: Timer    Interrupt Request (INT $50)  (1=Request)
Bit 3: Serial   Interrupt Request (INT $58)  (1=Request)
Bit 4: Joypad   Interrupt Request (INT $60)  (1=Request)
```
When an interrupt request signal changes from low to high, the corresponding bit in the IF register is set. For example, Bit 0 becomes set when the LCD controller enters the VBlank period.

Any set bits in the IF register are only requesting an interrupt. The actual execution of the interrupt handler happens only if both the IME flag and the corresponding bit in the IE register are set; otherwise the interrupt “waits” until both IME and IE allow it to be serviced.

Since the CPU automatically sets and clears the bits in the IF register, it is usually not necessary to write to the IF register. However, the user may still do that in order to manually request (or discard) interrupts. Just like real interrupts, a manually requested interrupt isn’t serviced unless/until IME and IE allow it.

### Interrupt Handling
1) The IF bit corresponding to this interrupt and the IME flag are reset by the CPU. The former “acknowledges” the interrupt, while the latter prevents any further interrupts from being handled until the program re-enables them, typically by using the reti instruction.

2) The corresponding interrupt handler (see the IE and IF register descriptions above) is called by the CPU. This is a regular call, exactly like what would be performed by a call <address> instruction (the current PC is pushed onto the stack and then set to the address of the interrupt handler).

The following interrupt service routine is executed when control is being transferred to an interrupt handler:

1) Two wait states are executed (2 M-cycles pass while nothing happens; presumably the CPU is executing nops during this time).
2) The current value of the PC register is pushed onto the stack, consuming 2 more M-cycles.
3) The PC register is set to the address of the handler (one of: $40, $48, $50, $58, $60). This consumes one last M-cycle.

The entire routine should last a total of 5 M-cycles. This has yet to be tested, but is what the Z80 datasheet implies.

### Interrupt Priorities
In the following circumstances it is possible that more than one bit in the IF register is set, requesting more than one interrupt at once:

1) More than one interrupt request signal changed from low to high at the same time.
2) Several interrupts have been requested while IME/IE didn’t allow them to be serviced.
3) The user has written a value with several bits set (for example binary 00011111) to the IF register.

If IME and IE allow the servicing of more than one of the requested interrupts, the interrupt with the highest priority is serviced first. The priorities follow the order of the bits in the IE and IF registers: Bit 0 (VBlank) has the highest priority, and Bit 4 (Joypad) has the lowest priority.

### Nested Interrupts
The CPU automatically disables all the other interrupts by setting IME=0 when it services an interrupt. Usually IME remains zero until the interrupt handler returns (and sets IME=1 by means of the reti instruction). However, if you want to allow the servicing of other interrupts (of any priority) during the execution of an interrupt handler, you may do so by using the ei instruction in the handler.

## 8-bit Load instructions
|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|ld r,r     |xx       |4            |––    |r=r                           |
|ld r,n     |xx nn    |8            |––    |r=n                           |
|ld r,(HL)  |xx       |8            |––    |r=(HL)                        |
|ld (HL),r  |7x       |8            |––    |(HL)=r                        |
|ld (HL),n  |36 nn    |12           |––    |(HL)=n                        |
|ld A,(BC)  |0A       |8            |––    |A=(BC)                        |
|ld A,(DE)  |1A       |8            |––    |A=(DE)                        |
|ld A,(nn)  |FA nn nn |16           |––    |A=(nn)                        |
|ld (BC),A  |02       |8            |––    |(BC)=A                        |
|ld (DE),A  |12       |8            |––    |(DE)=A                        |
|ld (nn),A  |EA nn nn |16           |––    |(nn)=A                        |
|ld A,(FF00+n)|F0 nn  |12           |––    |read from io-port n (memory FF00+n)|
|ld (FF00+n),A|E0 nn  |12           |––    |write to io-port n (memory FF00+n) |
|ld A,(FF00+C)|F2     |8            |––    |read from io-port C (memory FF00+C)|
|ld (FF00+C),A|E2     |8            |––    |write to io-port C (memory FF00+C) |
|ldi (HL),A  |22       |8            |––    |(HL)=A, HL=HL+1               |
|ldi A,(HL)  |2A       |8            |––    |A=(HL), HL=HL+1               |
|ldd (HL),A  |32       |8            |––    |(HL)=A, HL=HL-1               |
|ldd A,(HL)  |3A       |8            |––    |A=(HL), HL=HL-1               |

## 16-bit Load instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|ld rr,nn   |x1 nn nn |12           |––    |rr=nn (rr may be BC,DE,HL or SP)|
|ld (nn),SP |08 nn nn |20           |––    |(nn)=SP                       |
|ld SP,HL   |F9       |8            |––    |SP=HL                         |
|push rr    |x5       |16           |––    |SP=SP-2 (SP)=rr ; rr may be BC,DE,HL,AF|
|pop rr     |x1       |12           |(AF)  |rr=(SP) SP=SP+2 ; rr may be BC,DE,HL,AF|

## 8-bit Arithmetic/Logic instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|add A,r    |8x       |4            |z0hc  |A=A+r                         |
|add A,n    |C6 nn    |8            |z0hc  |A=A+n                         |
|add A,(HL) |86       |8            |z0hc  |A=A+(HL)                      |
|adc A,r    |8x       |4            |z0hc  |A=A+r+cy                      |
|adc A,n    |CE nn    |8            |z0hc  |A=A+n+cy                      |
|adc A,(HL) |8E       |8            |z0hc  |A=A+(HL)+cy                   |
|sub r      |9x       |4            |z1hc  |A=A-r                         |
|sub n      |D6 nn    |8            |z1hc  |A=A-n                         |
|sub (HL)   |96       |8            |z1hc  |A=A-(HL)                      |
|sbc A,r    |9x       |4            |z1hc  |A=A-r-cy                      |
|sbc A,n    |DE nn    |8            |z1hc  |A=A-n-cy                      |
|sbc A,(HL) |9E       |8            |z1hc  |A=A-(HL)-cy                   |
|and r      |Ax       |4            |z010  |A=A & r                       |
|and n      |E6 nn    |8            |z010  |A=A & n                       |
|and (HL)   |A6       |8            |z010  |A=A & (HL)                    |
|xor r      |Ax       |4            |z000  |A=A xor r                     |
|xor n      |EE nn    |8            |z000  |A=A xor n                     |
|xor (HL)   |AE       |8            |z000  |A=A xor (HL)                  |
|or r       |Bx       |4            |z000  |A=A | r                       |
|or n       |F6 nn    |8            |z000  |A=A | n                       |
|or (HL)    |B6       |8            |z000  |A=A | (HL)                    |
|cp r       |Bx       |4            |z1hc  |compare A-r                   |
|cp n       |FE nn    |8            |z1hc  |compare A-n                   |
|cp (HL)    |BE       |8            |z1hc  |compare A-(HL)                |
|inc r      |xx       |4            |z0h-  |r=r+1                         |
|inc (HL)   |34       |12           |z0h-  |(HL)=(HL)+1                   |
|dec r      |xx       |4            |z1h-  |r=r-1                         |
|dec (HL)   |35       |12           |z1h-  |(HL)=(HL)-1                   |
|daa        |27       |4            |z-0c  |decimal adjust A              |
|cpl        |2F       |4            |-11-  |A = A xor FF                  |

## 16-bit Arithmetic/Logic instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|add HL,rr  |x9       |8            |-0hc  |HL = HL+rr ; rr may be BC,DE,HL,SP|
|inc rr     |x3       |8            |––    |rr = rr+1 ; rr may be BC,DE,HL,SP|
|dec rr     |xB       |8            |––    |rr = rr-1 ; rr may be BC,DE,HL,SP|
|add SP,dd  |E8 dd    |16           |00hc  |SP = SP +/- dd ; dd is 8-bit signed number|
|ld HL,SP+dd|F8 dd    |12           |00hc  |HL = SP +/- dd ; dd is 8-bit signed number|

## Rotate and Shift instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|rlca       |07       |4            |000c  |rotate A left                 |
|rla        |17       |4            |000c  |rotate A left through carry   |
|rrca       |0F       |4            |000c  |rotate A right                |
|rra        |1F       |4            |000c  |rotate A right through carry  |
|rlc r      |CB 0x    |8            |z00c  |rotate left                   |
|rlc (HL)   |CB 06    |16           |z00c  |rotate left                   |
|rl r       |CB 1x    |8            |z00c  |rotate left through carry     |
|rl (HL)    |CB 16    |16           |z00c  |rotate left through carry     |
|rrc r      |CB 0x    |8            |z00c  |rotate right                  |
|rrc (HL)   |CB 0E    |16           |z00c  |rotate right                  |
|rr r       |CB 1x    |8            |z00c  |rotate right through carry    |
|rr (HL)    |CB 1E    |16           |z00c  |rotate right through carry    |
|sla r      |CB 2x    |8            |z00c  |shift left arithmetic (b0=0)  |
|sla (HL)   |CB 26    |16           |z00c  |shift left arithmetic (b0=0)  |
|swap r     |CB 3x    |8            |z000  |exchange low/hi-nibble        |
|swap (HL)  |CB 36    |16           |z000  |exchange low/hi-nibble        |
|sra r      |CB 2x    |8            |z00c  |shift right arithmetic (b7=b7)|
|sra (HL)   |CB 2E    |16           |z00c  |shift right arithmetic (b7=b7)|
|srl r      |CB 3x    |8            |z00c  |shift right logical (b7=0)    |
|srl (HL)   |CB 3E    |16           |z00c  |shift right logical (b7=0)    |

## Single-bit Operation instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|bit n,r    |CB xx    |8            |z01-  |test bit n                    |
|bit n,(HL) |CB xx    |12           |z01-  |test bit n                    |
|set n,r    |CB xx    |8            |––    |set bit n                     |
|set n,(HL) |CB xx    |16           |––    |set bit n                     |
|res n,r    |CB xx    |8            |––    |reset bit n                   |
|res n,(HL) |CB xx    |16           |––    |reset bit n                   |

## CPU Control instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|ccf        |3F       |4            |-00c  |cy=cy xor 1                   |
|scf        |37       |4            |-001  |cy=1                          |
|nop        |00       |4            |––    |no operation                  |
|halt       |76       |N*4          |––    |halt until interrupt occurs (low power)|
|stop       |10 00    |?            |––    |low power standby mode (VERY low power)|
|di         |F3       |4            |––    |disable interrupts, IME=0      |
|ei         |FB       |4            |––    |enable interrupts, IME=1       |

## Jump instructions

|Mnemonic   |Encoding |Clock cycles |Flags |Description                    |
|-----------|---------|-------------|------|------------------------------|
|jp nn      |C3 nn nn |16           |––    |jump to nn, PC=nn              |
|jp HL      |E9       |4            |––    |jump to HL, PC=HL              |
|jp f,nn    |xx nn nn |16/12        |––    |conditional jump if nz,z,nc,c  |
|jr PC+dd   |18 dd    |12           |––    |relative jump to nn (PC=PC+8-bit signed)|
|jr f,PC+dd |xx dd    |12/8         |––    |conditional relative jump if nz,z,nc,c|
|call nn    |CD nn nn |24           |––    |call to nn, SP=SP-2, (SP)=PC, PC=nn|
|call f,nn  |xx nn nn |24/12        |––    |conditional call if nz,z,nc,c  |
|ret        |C9       |16           |––    |return, PC=(SP), SP=SP+2       |
|ret f      |xx       |20/8         |––    |conditional return if nz,z,nc,c|
|reti       |D9       |16           |––    |return and enable interrupts (IME=1)|
|rst n      |xx       |16           |––    |call to 00,08,10,18,20,28,30,38|

