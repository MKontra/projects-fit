
; -----------------------------------------------------------------------------
; export symbols
            XDEF Entry        ; export 'Entry' symbol
            ABSENTRY Entry    ; for absolute assembly: mark this as application entry point

; include derivative specific macros
            Include 'derivative.inc'

; -----------------------------------------------------------------------------
VectorStart EQU  $FFDA  	; start address of interrupt vectors table

; -----------------------------------------------------------------------------
            ORG Z_RAMStart	;vars in Page Zero RAM for direct addressing
LOCALB              ds.b 1
            ORG RAMStart	; variable/data section

BTNS                ds.b 1     ;tlacitka

MEASURE_0           ds.b 1     ;low byte of time in miliseconds            
MEASURE_1           ds.b 1     ;middle byte of time in miliseconds
MEASURE_2           ds.b 1     ;high byte of time in miliseconds
MEASURE_padding_0   ds.b 1     ;pad to 4 bytes
            
tmp0                ds.b 1     ;low byte of time in miliseconds            
tmp1                ds.b 1     ;middle byte of time in miliseconds
tmp2                ds.b 1     ;high byte of time in miliseconds
tmp_padding_0       ds.b 1     ;pad to 4 bytes
              
MEASURE_H           ds.b 1     ;measured amount of hours           
MEASURE_M           ds.b 1     ;measured amount of minutes
MEASURE_S           ds.b 1     ;measured amount of seconds
MEASURE_S100        ds.b 1     ;measured amount of 1/100secs   
            
USR_H               ds.b 1     ;user stored amount of hours           
USR_M               ds.b 1     ;user stored amount of minutes
USR_S               ds.b 1     ;user stored amount of seconds
USR_S100            ds.b 1     ;user stored amount of 1/100secs
            
CLOCK_RUNNING       ds.b 1     ;start pressed... 
ADD_PERFORMED       ds.b 1     ;not first press of add button... 

hourdiv_0             ds.b 1
hourdiv_1             ds.b 1 
hourdiv_2             ds.b 1 
hourdiv_3             ds.b 1 

mindiv_0             ds.b 1
mindiv_1             ds.b 1 
mindiv_2             ds.b 1 
mindiv_3             ds.b 1

secdiv_0             ds.b 1
secdiv_1             ds.b 1 
secdiv_2             ds.b 1 
secdiv_3             ds.b 1                         
            ORG ROMStart	; code section

; -----------------------------------------------------------------------------
; application starts here after HC08 is reseted

;mov32
movs32: MACRO
      ;uklada sp+1-3 na h:x, pouzite pri kopirovani na zasobniku
      lda 1,SP
      sta 1,X
      
      lda 2,SP
      sta 2,X
      
      lda 3,SP
      sta 3,X
      
      lda 4,SP
      sta 4,X
      ENDM

;push32
push32: MACRO
      lda 3,X
      psha
      lda 2,X
      psha
      lda 1,X
      psha
      lda ,X
      psha
      ENDM  

;pop32      
pop32: MACRO
      ;H:X obsahuje adresu kam data dat
      pula
      sta 0,X
      pula
      sta 1,X
      pula
      sta 2,X
      pula
      sta 3,X
      ENDM

inc32: MACRO
      ;sp+1->sp+3 = j
      ;j = j++
      ;carry if overflow
      clc
      lda 1,SP
      add #1
      sta 1,SP
      
      lda 2,SP
      adc #0
      sta 2,SP
      
      lda 2,SP
      adc #0
      sta 2,SP
      
      lda 3,SP
      adc #0
      sta 3,SP
      ENDM

;add32
add32: MACRO
      ;sp+1->sp+3 = j
      ;sp+5->sp+8 = k
      ;j = j + k
      ;carry if overflow
      clc
      lda 1,SP
      add 5,SP
      sta 1,SP
      
      lda 2,SP
      adc 6,SP
      sta 2,SP
      
      lda 3,SP
      adc 7,SP
      sta 3,SP
      
      lda 4,SP
      adc 8,SP
      sta 4,SP
      ENDM
      
sub32: MACRO
      ;sp+3->sp+6 = j
      ;sp+7->sp+10 = k
      ;j = j - k
      ;carry if borrow
      clc
      lda 1,SP
      sub 5,SP
      sta 1,SP
      
      lda 2,SP
      sbc 6,SP
      sta 2,SP
      
      lda 3,SP
      sbc 7,SP
      sta 3,SP
      
      lda 4,SP
      sbc 8,SP
      sta 4,SP
      ENDM
      
divmod32: MACRO
      ;sp+3->sp+6 = j
      ;sp+7->sp+10= k
      ;j = j % k; k = j / k
      ;NEMAMCOBYSOMDODAL 

      ais #-4 ;local cntr
      lda #0
      sta 1,SP
      sta 2,SP
      sta 3,SP
      sta 4,SP
      ais #4
\@divstart:
      sub32


      bcs \@divnegative
      
      ais #-4 ;local cntr
      inc32
      ais #4
      
      bra \@divstart

\@divnegative: 

      add32 ;kj je zvysok chcelo by to do k dat podiel

      ais #3 
      tsx
      ais #-3
      
      ais #-4
      movs32
      ais #4
      
      

      ENDM
      
Entry:
       cli                		; enable interrupts
       mov  #$01,CONFIG1   		; disable Computer Operating Properly (COP) Module
       rsp
       clrh
       clrx
       
       lda #$0
       sta CLOCK_RUNNING      ;stopped by default
       sta ADD_PERFORMED      ;no data stored
       sta BTNS
 
      ;konstanty na delenie pre hodiny
       lda #$40
       sta hourdiv_0
       lda #$7E
       sta hourdiv_1
       lda #$05
       sta hourdiv_2
       lda #$00
       sta hourdiv_3

      ;konstanty na delenie pre minuty
       lda #$70
       sta mindiv_0
       lda #$17
       sta mindiv_1
       lda #$00
       sta mindiv_2
       lda #$00
       sta mindiv_3       

      ;konstanty na delenie pre sekundy
       lda #$64
       sta secdiv_0
       lda #$00
       sta secdiv_1
       lda #$00
       sta secdiv_2
       lda #$00
       sta secdiv_3        
        
       lda #$00
       sta MEASURE_0
       lda #$00
       sta MEASURE_1
       lda #$00
       sta MEASURE_2
       lda #$00
       sta MEASURE_padding_0
 

main_loop:

       lda BTNS
       sta LOCALB
       brset 0, LOCALB, dostart       
       brset 1, LOCALB, dosplit
       brset 2, LOCALB, doadd
       brset 3, LOCALB, dostop
       brset 4, LOCALB, doreset
       jmp main_loop
       
dostart:
       lda #$0
       cmp CLOCK_RUNNING      ;if clock is started - do nothing
       bne dostartend
       
       jsr RTCInterruptEnable ;clock is stopped - start them
       lda #$01
       sta CLOCK_RUNNING
       
dostartend:
       jmp zeroBTNS
       
dosplit:
      sei
       lda #$0
       cmp CLOCK_RUNNING
       beq dosplitend         ;clock is stopped - do nothing

       jsr Clock012toHMS      ;recalclate 1/100s to appropriate values

dosplitend:
      cli
       jmp zeroBTNS
       
doadd:
      sei
       lda #$0
       cmp CLOCK_RUNNING
       beq dosplitend         ;clock is stopped - do nothing
       cmp ADD_PERFORMED
       beq firstadd           ;first add = only set usr vals
       
       jsr Clock012toHMS      ;transform 1/100secs
       jsr CalcMeUsrdiff      ;store difference

       jmp doaddend
firstadd:

       jsr Clock012toHMS
       jsr StoreToUSRvars
       
       lda #$01
       sta ADD_PERFORMED      
       
doaddend:
      cli
       jmp zeroBTNS
       
dostop:
       lda #$0
       cmp CLOCK_RUNNING      ;if clock is stopped - do nothing
       beq dostartend
       
       jsr RTCInterruptDisble ;clock is running - stop it
       sta CLOCK_RUNNING      ;set running flag to 0
       
dostopend:
       jmp zeroBTNS
       
doreset:
       sei
       jsr ClockReset
       cli
       jmp zeroBTNS              
       
zeroBTNS:
       lda #$0
       sta BTNS
       jmp main_loop       

;---------------------------------------------------------------------
;subroutines
;---------------------------------------------------------------------
           
      
;clock reset = zeroes all measure vars
ClockReset:
      lda #$0
      sta MEASURE_2
      sta MEASURE_1
      sta MEASURE_0
      
      ;sta MEASURE_H
      ;sta MEASURE_M
      ;sta MEASURE_S
      ;sta MEASURE_S100       
      rts


;calculate hours, minutes and seconds from 1/100sec value
Clock012toHMS:
      ;h:x address of time
      ldhx #hourdiv_0
      push32
      ldhx #MEASURE_0
      push32
      divmod32
      
      ;vrch - modulo
      ldhx #tmp0
      pop32
      pula
      sta MEASURE_H
      pula
      pula
      pula
      ;vrch podiel
      ; nic + tmp = podiel po hodinach
      ldhx #mindiv_0
      push32
      ldhx #tmp0
      push32
      divmod32     
      ldhx #tmp0
      pop32
      pula
      sta MEASURE_M
      pula
      pula
      pula
      ;vrch modulo po minutach     
      ; nic + tmp = modulo po minutach
      ldhx #secdiv_0
      push32
      ldhx #tmp0
      push32
      divmod32     
      ldhx #tmp0
      pop32
      pula
      sta MEASURE_S
      pula
      pula
      pula
      ;vrch modulo po sekundach
      push32     
      pula
      sta MEASURE_S100
      pula
      pula
      pula

      rts
      
;measure to user difference
CalcMeUsrdiff:
      clc
      lda MEASURE_S100
      sub USR_S100
      bcc nocompss
      ; vypozicka od sekund
      sub #156  ;kompenzacia pola
      sec       
nocompss:
      sta USR_S100
      
      lda MEASURE_S
      sbc USR_S
      bcc nocomps    
      ; vypozicka od minut
      sub #196  ;kompenzacia pola
      sec       
nocomps:
      sta USR_S  
      
      lda MEASURE_M
      sbc USR_M
      bcc nocompm    
      ; vypozicka od hodin
      sub #196  ;kompenzacia pola
      sec       
nocompm:
      sta USR_M 
      
      lda MEASURE_H
      sbc USR_H
      bcc nocomph    
      ; vypozicka od ???   
nocomph:
      sta USR_H      
      
      rts
      
;store measured values to user variabiles
StoreToUSRvars:
      lda MEASURE_H
      sta USR_H

      lda MEASURE_M
      sta USR_M
      
      lda MEASURE_S
      sta USR_S

      lda MEASURE_S100
      sta USR_S100      
      rts

;RTC configuration subroutine
RTCConfig:
      sei ;disable interrupts during initialization to make sure rtc is fully configured before enable
      
      ;enable RTC module
      bset 6, RTCCR1 ; enable chronograph interrupts
      
      ;reference frequency of clock generator
      ;leave default 32.768 kHz
      ;bclr 0, RTCCR2 - by default
      ;bclr 1, RTCCR2 - by default
      ;bclr 2, RTCCR2 - by default
      bset 4, RTCCR2 ;enable rtc module 
      bset 5, RTCCR2 ;enable chronograph      
      cli
      rts

;enable RTC and interrupt generation (start button)      
RTCInterruptEnable:
      sei
      bset 6, RTCCR1 ;enable chronograph interrupts
      bset 4, RTCCR2 ;enable rtc module 
      bset 5, RTCCR2 ;enable chronograph
      cli
      rts
;disable RTC and interrupt generation(stop button)      
RTCInterruptDisble:
      sei
      bclr 6, RTCCR1 ;disable chronograph interrupts
      bclr 4, RTCCR2 ;disable rtc module 
      bclr 5, RTCCR2 ;disable chronograph
      cli
      rts

; -----------------------------------------------------------------------------
;ISRs and IVT
;interrupt service routines and intterupt vector table...

;spurious handler
spurious:
            NOP
            RTI

;rtc tick handler - fired every 1/100 of second
militick:

            lda RTCSR
            lda CHRR
hintr:            
            ;overflow check
            ; 8639999
 
            lda MEASURE_2
            cmp #$83
            bne noow
            lda MEASURE_1
            cmp #$D5
            bne noow
            lda MEASURE_0
            cmp #$FF
            bne noow
            
            jsr ClockReset
            jmp militickend
noow:
            clc ;clear carry
            lda MEASURE_0
            adc #$1 ;+1 1/100sec
            sta MEASURE_0
            lda MEASURE_1
            adc #$0  ;add carry if present
            sta MEASURE_1
            lda MEASURE_2
            adc #$0  ;add carry if present
            sta MEASURE_2
            
militickend:            
            RTI

; -----------------------------------------------------------------------------
        ORG  VectorStart	
;------------------------------------------------------------------
; 	routine	          vector description        vector address
;------------------------------------------------------------------
	fdb  militick	  ; Real-time Clock Vector  ,     $FFDA
	fdb  spurious   ; ADC Conversion Complete ,     $FFDC
	fdb  spurious   ; Keyboard Vector	  ,     $FFDE
	fdb  spurious   ; SCI Transmit Vector	  ,     $FFE0
	fdb  spurious   ; SCI Receive Vector	  ,     $FFE2
	fdb  spurious   ; SCI Error Vector	  ,     $FFE4
	fdb  spurious   ; SPI Receive Vector	  ,     $FFE6
	fdb  spurious   ; SPI Transmit Vector	  ,     $FFE8
	fdb  spurious	  ; TIM2 Overflow Vector    ,     $FFEA
	fdb  spurious   ; TIM2 Channel 1 Vector	  ,     $FFEC
	fdb  spurious   ; TIM2 Channel 0 Vector	  ,     $FFEE
	fdb  spurious   ; TIM1 Overflow Vector	  ,     $FFF0
	fdb  spurious   ; TIM1 Channel 1 Vector	  ,     $FFF2
	fdb  spurious   ; TIM1 Channel 0 Vector	  ,     $FFF4
	fdb  spurious   ; PLL Vector		  ,     $FFF6
	fdb  spurious   ; LVI Vector		  ,     $FFF8
	fdb  spurious   ; ~IRQ1 Vector		  ,     $FFFA
	fdb  spurious   ; SWI Vector		  ,     $FFFC
	fdb  Entry      ; Reset Vector		  ,     $FFFE

