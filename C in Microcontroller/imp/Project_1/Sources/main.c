#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


//-----------nastavime koncovy cas  
unsigned char DECOUNT = 0;  
unsigned char YRR_fin   =0;
unsigned char MTHR_fin  =1;
unsigned char DAYR_fin  =0;
unsigned char HRR_fin   =15;
unsigned char MINR_fin  =3;
unsigned char SECR_fin  =12;

 int YRR_to =0; 
 int MTHR_to =0;
 int DAYR_to=0;
 int HRR_to =0;
 int MINR_to =0; 
 int SECR_to =0;

volatile unsigned char test1;

void init() {              // inicializacia 
  RTCCR1 = 0b00000100; // interupt po sekunde
  RTCCR2 = 0b00010000; // pustime RTC
 //------------nastavime aktualny cas do RTC
  SECR = 38;
  MINR = 5;
  HRR = 0;
  DAYR = 0;
  MTHR = 1;
  YRR = 0;
 //------------------------------------------ 
  
  // nastavime uvodny casovy rozdiel 
  if((SECR_fin - SECR) < 0)   {
     MINR_to++;
     SECR_to = (60-SECR)+SECR_fin;
  }
  else
    SECR_to = SECR_fin -SECR;
  
  if(MINR>0)
  MINR_to = MINR_fin - (MINR+1);
  else
  MINR_to = MINR_fin - MINR;
  if(MINR_to < 0){
    HRR_to++;
    MINR_to = (60-(MINR+1))+MINR_fin;
  }
  
  if(HRR_to>0)
  HRR_to = HRR_fin - (HRR+1);
  else
  HRR_to = HRR_fin - HRR;
    if(HRR_to < 0){
    DAYR_to++;
    HRR_to = (24-(HRR+1))+HRR_fin;
  }
  
  if(DAYR_to>0)
  DAYR_to = DAYR_fin - (DAYR+1);
  else
  DAYR_to = DAYR_fin - DAYR;
    if(DAYR_to < 0){
    MTHR_to++;
    if((MTHR%2) == 0)
      if(MTHR == 2)
    DAYR_to = (28-(DAYR+1))+DAYR_fin;
      else
      DAYR_to = (30-(DAYR+1))+DAYR_fin;
    else
      DAYR_to = (31-(DAYR+1))+DAYR_fin;  
  }
  
  if(MTHR_to>0)
  MTHR_to = MTHR_fin - (MTHR+1);
  else
  MTHR_to = MTHR_fin - MTHR;
    if(MTHR_to < 0){
    YRR_to++;
    MTHR_to = (12-(MTHR+1))+MTHR_fin;
  }
  
  if(YRR_to>0)
  YRR_to = YRR_fin - (YRR+1);
  else
  YRR_to = YRR_fin - YRR;
  
  
 
 
  DECOUNT = 1;     // zaciname odpocitavat

}
void odcitanie() {         // po sekunde podpocitavame
 //--------------koniec odpocitavania-----------------------------------------------------
 if(YRR_to <0 || MTHR_to <0 || DAYR_to <0 || HRR_to < 0 || MINR_to <0 || SECR_to <0)
   DECOUNT=0;
  if(YRR_to ==0 && MTHR_to ==0 && DAYR_to ==0 && HRR_to == 0 && MINR_to ==0 && SECR_to ==0)
   DECOUNT=0; 
//----------------------------------------------------------------------------------------
  if(SECR_to == 0){     // odpocet sekundy
    MINR_to--;
    SECR_to = 59;
  } else
      SECR_to--;
  
  if(MINR_to < 0){               //odpocet minuty
    HRR_to--;
    MINR_to = 59;
  }
      
  
  if(HRR_to < 0){
    DAYR_to--;             // odpocet hodiny
    HRR_to = 23;
  }
                            
  if(DAYR_to < 0){          // odpocet dna
    MTHR_to--;
    if((MTHR % 2) == 0)
      if(MTHR == 2)
        DAYR_to = 28;
      else
        DAYR_to = 30;
    else
      DAYR_to = 31;  
  }  
   
                         
   if(MTHR_to < 0){        // odpocet mesiaca
    YRR_to--;
    MTHR_to = 11;
  }
  
  
  
  
}

interrupt 18 void sekunda(){       // presla sekunda 

    odcitanie();
    
    test1 = SECR;
    test1 = RTCSR;
    
    __RESET_WATCHDOG();   
}

void main(void) {

  EnableInterrupts; 

  init();


  for(;;) {
  
  if(DECOUNT == 0) {
    RTCCR1 = 0b00000000;   // vypneme RTC
  RTCCR2 = 0b00000000;
  }
  
   __RESET_WATCHDOG();
  }

}
