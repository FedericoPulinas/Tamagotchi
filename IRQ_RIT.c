/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "Pet.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "TouchPanel/TouchPanel.h"
#include "ADC/adc.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int seq4beats[3][9] = {{-1, 2, 5, 2, 5, 2, 5, -1, -1},
											 {6, 1, 0, 2, 3,  -1, -1, -1, -1},
											 {0, 6, 4, 2, 7, 1, 3, 5, -1}
											 /*{0, 1, 2, 3, 4, 5, 6, 7, -1}*/};


//volatile int down=0;
volatile int flag_sound = 0, flag_animationOn = 0, flag_TP = 0;
extern volatile int16_t x, y;
extern const int freqs[8];
extern volatile int counterHP, counterB;

int busy = 0;
volatile int song = 0, play = 0;
											 
void RIT_IRQHandler (void)
{					
	static int jL = 0, jR = 0, jS = 0, tick, note;
	static int selectButton = 0; /* 1 - left button was selected, 2 - right button was selected*/
	int frq;
	
	ADC_start_conversion();
	
	if(flag_animationOn == 0){
	//joystick
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && !isDead()){ /*joystick left*/
		jL++;
		switch(jL){
			case 1:
				DrawBox(121, 250, 237, 250, 121, 318, 237, 318, Black);
				DrawBox(1, 250, 119, 250, 1, 318, 119, 318, Red);
				selectButton = 1;
			break;
			
			default:
			break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && !isDead()){ /*joystick right*/
		jR++;
		switch(jR){
			case 1:
				DrawBox(1, 250, 119, 250, 1, 318, 119, 318, Black);
				DrawBox(121, 250, 237, 250, 121, 318, 237, 318, Red);
				selectButton = 2;
			break;
			
			default:
			break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && flag_animationOn == 0 && flag_TP == 0){ /*joystick select or down*/
		/*la decisione di impostare il tasto down per selezionare è semplicemente per velocità di realizzazione del video, dato che in emulazione il tasto select non funziona
		a meno di lavorare utilizzando la finestra del GPIO1*/
		
		jS++;
		
		if(isDead()){
			DrawBox(1, 250, 237, 250, 1, 318, 237, 318, Red);
			reset_timer(0);
			reset_timer(3);
			drawSetup();
		}
		else {
			switch(jS){
			case 1:
				switch(selectButton){
					case 1: /*left*/
						//reset_timer(1);
					
						//click sound
						disable_timer(1);
						reset_timer(1);
						init_timer(1, 0, 0, 3, freqs[7]);	
						enable_timer(1);
						
						song = 0;
						play = 1;
						note = 0;
					
						flag_animationOn = 1;	
					
						drawHP();
						counterHP = 5;
						setDirection(selectButton);
						enable_timer(3);
					break;
					
					case 2: /*right*/
						//click sound
						disable_timer(1);
						reset_timer(1);
						init_timer(1, 0, 0, 3, freqs[7]);	
						enable_timer(1);
		
						song = 0;
						play = 1;
						note = 0;
					
						flag_animationOn = 1;
					
						drawItem();
						counterB = 5;
						setDirection(selectButton);
						enable_timer(3);
					break;
				}
			break;
				
			default:
			break;
		}
		}
	}
	else {
		jL = 0;
		jR = 0;
		jS = 0;
	}
	
	//touchpanel
	if(getDisplayPoint(&display, Read_Ads7846(), &matrix ) && flag_animationOn == 0){
		if(display.x <= (x+65) && display.x >= x && display.y <= (y+82) && display.y >= y){
			//cuddle animation in pet.c
			flag_TP = 1;
			enable_timer(3);
			note = 0;
			song = 2;
			play = 1;
		}
	}
}
	if(play){
			tick++;
			if(tick%7 == 0){
			
				if(note == 9){
					disable_timer(1);
					reset_timer(1);
					note = 0;
					play = 0;
					tick = 0;
				}
				else {
					frq = seq4beats[song][note++];
					if(frq != -1){
						init_timer(1, 0, 0, 3, freqs[frq]);
						enable_timer(1);
					}
					else {
						disable_timer(1);
						reset_timer(1);
					}
			}
		}
	}
	
	disable_RIT();
	reset_RIT();
	enable_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
