/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"

/* Private function prototypes -----------------------------------------------*/
void drawSetup(void);

void drawSprite(void);
void drawItem(void);
void drawHP(void);

void deleteItem(void);
void deleteHP(void);

void drawHearts(int16_t x0, int16_t y0);
uint8_t deleteHearts(void);
void addHeart(void);

void drawBatteries(int16_t x0, int16_t y0);
uint8_t deleteBatteries(void);
void addBattery(void);

void base_animation(int16_t x0, int16_t y0, int16_t offsetX, int16_t offsetY, uint8_t *flag); /*l'offset serve per muoversi all'interno della matrice grande*/
void animations(int16_t x0, int16_t y0, uint8_t flag);
void menu_anim(void);

void setDirection(int selectButton);
int getDirection(void);

int checkDistanceL(void);
int checkDistanceR(void);

int isDead(void);
int outsideScreen(void);

void drawMute(void);
void drawVolume(void);

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
