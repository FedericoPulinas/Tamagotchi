/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../Pet.h"
#include "../RIT.h"
#include "../ADC/adc.h"
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =                                       /*                      */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

uint8_t flag_baseanimation = 1, flag_stopbasic = 0, flag_foot = 0, flag_taken = 0;
uint8_t flag_dead = 0, flag_show = 0, flag_cuddle = 0, flag_endsound = 0;

volatile int counterHP = 5, counterB = 5;

extern uint8_t count_hp;
extern volatile int flag_sound;
extern volatile int flag_animationOn;
extern volatile int flag_TP;
extern const int freqs[8];
extern volatile int song, play;
extern int volumeLevel;

int	one_note = 1;
int	three_note = 3;
int	five_note = 5;
int	eight_note = 8;

void TIMER0_IRQHandler (void)
{
	static int count = 0, hh = 0, mm = 0, ss = 0;
	char time_in_char[18] = "";
	static int tmpH = 5, tmpB = 5;
	
	if (LPC_TIM0->IR & 01){
		
		if(flag_dead == 0 && LPC_TIM0->TCR == 1){
			count++;
			hh = count/3600;
			mm = count/60;
			ss = count%60;
			sprintf(time_in_char, "%02d:%02d:%02d", hh, mm, ss);
			GUI_Text(136, 10, (uint8_t*)time_in_char, Black, Grigio_medio);
		}
		
		if(flag_stopbasic == 0 && !isDead()){ //quando fa le animazioni di HP o Item fermo lo sbattere degli occhi
			base_animation(6, 22,	 22, 6, &flag_baseanimation);		
		}
		else if (isDead() && flag_dead == 1){
			disable_timer(0);
			count = 0;
			flag_dead = 0;
			flag_stopbasic = 0;
			flag_animationOn = 0;
			LCD_DrawRectangle(89, 130, 80, 20, Marroncino);
			MyDrawRectangleLine(1, 250, 237, 250, 1, 318, 237, 318, Grigio_medio, Black);
			GUI_Text(105, 277, "RESET", Black, Grigio_medio);
			GUI_Text(90, 159, "PLAY AGAIN", Black, Marroncino);
			tmpH = 5;
			tmpB = 5;
			enable_RIT();
		}
		
		if(!isDead() && flag_animationOn == 0){
			//HP
			if(counterHP == 0){
				tmpH = deleteHearts();
				counterHP = 5;
			}
			counterHP--;
		
			if (counterB == 0){
				tmpB = deleteBatteries();
				counterB = 5;
			}
			counterB--;
			if(tmpH == 0){
				flag_stopbasic = 1;
				enable_timer(3);
			}
			
		//Batteries
			if(tmpB == 0){
				flag_stopbasic = 1;
				enable_timer(3);
			}
		}
		
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
	}
	
	/* Match register 1 interrupt service routine */
	else if(LPC_TIM0->IR & 02){
		LPC_TIM0->IR =  2 ;			/* clear interrupt flag */	
	}
	
	/* Match register 2 interrupt service routine*/
	else if(LPC_TIM0->IR & 4){
		LPC_TIM0->IR =  4 ;			/* clear interrupt flag */	
	}
	
	/* Match register 3 interrupt service routine */
	else if(LPC_TIM0->IR & 8){
	 
		LPC_TIM0->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void) //timer feed
{
	/* Match register 0 interrupt service routine */
	if (LPC_TIM1->IR & 01){
		static int ticks = 0;
		
		//click sound
		LPC_DAC->DACR = (SinTable[ticks]*volumeLevel/8)<<6;
		ticks++;
		if(ticks == 45){
			ticks = 0;
		}
		
		LPC_TIM1->IR = 1;			/* clear interrupt flag */
	}
	
	/* Match register 1 interrupt service routine */
	else if(LPC_TIM1->IR & 02)
  {		
		LPC_TIM1->IR =  2 ;			/* clear interrupt flag */	
	}
	
	/* Match register 2 interrupt service routine */
	else if(LPC_TIM1->IR & 4){
		LPC_TIM1->IR =  4 ;			/* clear interrupt flag */	
	}
	
	/* Match register 3 interrupt service routine */
	else if(LPC_TIM1->IR & 8)
  {
		LPC_TIM1->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	
	/* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01){	
		LPC_TIM2->IR = 1;			/* clear interrupt flag */
	}
	
	/* Match register 1 interrupt service routine */
	else if(LPC_TIM2->IR & 02)
  {
		LPC_TIM2->IR =  2 ;			/* clear interrupt flag */	
	}
	
	else if(LPC_TIM2->IR & 4){
		LPC_TIM2->IR =  4 ;			/* clear interrupt flag */	
	}
	
	/* Match register 3 interrupt service routine */
	else if(LPC_TIM2->IR & 8)
  {
		LPC_TIM2->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

void TIMER3_IRQHandler (void)
{
	static int count = 0;
	
	flag_stopbasic = 1;
	/* Match register 0 interrupt service routine */
 	if (LPC_TIM3->IR & 01){
		/*menu animations:
			0 - right foot
			1 - left foot
			2 - rise arms
			3 - hp in hands
			4 - item in hands*/
		if(flag_taken == 0 && flag_TP == 0){
				
			
			if(getDirection() == 1 /*going left*/ && checkDistanceL() == 1 && isDead() == 0){ 
				//animations(-20, 0, flag_foot);
				animations(-10, 0, flag_foot);
				flag_foot = !flag_foot;
			}
			else if (getDirection() == 2 /*going right*/ &&  checkDistanceR() == 1 && isDead() == 0){
				//animations(+20, 0, flag_foot);
				animations(+10, 0, flag_foot);
				flag_foot = !flag_foot;
			}
			else if (getDirection() == 1 && checkDistanceL() != 1 && isDead() == 0){ /*it reached the HP since the direction didn't change but the distance is too less*/
				animations(0, 0, 2);
				animations(0, 0, 3);
				flag_taken = 1;
				flag_stopbasic = 0;
			}
			else if (getDirection() == 2 && checkDistanceR() != 1 && isDead() == 0) { /*it reached the Item since the direction didn't change but the distance is too less*/
				animations(0, 0, 2);
				animations(0, 0, 4);
				flag_taken = 1;
				flag_stopbasic = 0;
				flag_endsound = 0;
			}
			//touchScreen animation
			
			if (isDead()) {
				GUI_Text(89, 130, "GAME OVER", Red, Marroncino);
				//animations(30, 0, 5);
				if(flag_animationOn == 0){
					song = 1;
					play = 1;
				}
				flag_animationOn = 1;
				animations(10, 0, 5);
				if(outsideScreen()){
					flag_dead = 1;
					disable_timer(3);
				}	
			}
		}
		else if (flag_cuddle == 0 && flag_TP == 1){
			animations(0, 0, 6);
			flag_cuddle = 1;
			flag_animationOn = 1;
		}
		else if (flag_taken == 0 && flag_TP == 1 && flag_show == 0 && count != 4){
			animations(0, 0, 7);
			flag_show = !flag_show;
			count++;
		}
		else if (flag_taken == 0 && flag_TP == 1 && flag_show == 1 && count != 4){
			animations(0, 0, 8);
			flag_show = !flag_show;
			count++;
		}
		else if (count == 4){
			if (count_hp < 5 && count_hp > 0){
				addHeart();
				counterHP = 5; //reset of the timer
			}
			count = 0;
			flag_taken = 1;
		}
		else if (flag_taken == 1){
				disable_timer(3);
				flag_taken = 0;
				animations(0, 0, 10);
				//animations(0, 0, 20);
				flag_endsound = 0;
				flag_stopbasic = 0;
				flag_animationOn = 0;
				flag_cuddle = 0;
				flag_TP = 0;
		}
		
		
		LPC_TIM3->IR = 1;			/* clear interrupt flag */
	}
	
	/* Match register 1 interrupt service routine */
	else if(LPC_TIM3->IR & 02){
		
		LPC_TIM3->IR =  2 ;			/* clear interrupt flag */	
	}
	
	else if(LPC_TIM3->IR & 4){
		LPC_TIM3->IR =  4 ;			/* clear interrupt flag */	
	}
	
	/* Match register 3 interrupt service routine */
	else if(LPC_TIM3->IR & 8)
  {
		LPC_TIM3->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
