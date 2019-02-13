

#include <stdint.h>
#include "tm4c123gh6pm.h"

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06

void sawtooth(void){
	
	GPIO_PORTB_DATA_R =(GPIO_PORTB_DATA_R == 255)? 0: GPIO_PORTB_DATA_R + 1;
	
}

void PortB_Init(void){volatile unsigned long delay;

	SYSCTL_RCGC2_R |= 0x02;//activating port b clock
	delay = SYSCTL_RCGC2_R;//delay
	GPIO_PORTB_CR_R |= 0xFF;//allows changes to all bits of port B
	GPIO_PORTB_AMSEL_R &= ~0xFF;////Disables Analog functionality on all of port b
	GPIO_PORTB_PCTL_R &= 0x00000000;//All pins are set up for GPIO
	GPIO_PORTB_DIR_R |= 0xFF;//All pins are output
	GPIO_PORTB_AFSEL_R &= ~0xFF;//All pins are not using alternate function
	GPIO_PORTB_PUR_R |= 0xFF;//
	GPIO_PORTB_DEN_R |= 0xFF;//All pins are digitaly enabled
	//initializing port b
	GPIO_PORTB_DATA_R &= ~0xFF;//Clears all bit on Port B
	
}

void Button_Init(void){volatile unsigned long delay;//delay
	SYSCTL_RCGC2_R |= 0x20;//clock to portF
	delay = SYSCTL_RCGC2_R;//delay
	GPIO_PORTF_LOCK_R |= 0x4C4F434B;
	GPIO_PORTF_CR_R |= 0x11;//allows changes to PF0 and PF4
	GPIO_PORTF_AMSEL_R &= ~0x11;//disables analog functionality to PF0 and PF4
	GPIO_PORTF_PCTL_R &= ~0x000F000F;//GPIO Functionality to PF0 and PF4
	GPIO_PORTF_DIR_R |= ~0x11;
	GPIO_PORTF_AFSEL_R &= ~0x11;//disables alternate functionalit to PF0 and PF4
	GPIO_PORTF_PUR_R |= 0x11;
	GPIO_PORTF_DEN_R |= 0x11;
	
	GPIO_PORTF_IS_R &= ~0x11;//PF0 and PF4 edge triggerd
	GPIO_PORTF_IBE_R &= ~0x11;//Not triggered on both edges
	GPIO_PORTF_IEV_R &= ~0x11;//faling edge triggered
	GPIO_PORTF_ICR_R = 0x11;//clear flags
	GPIO_PORTF_IM_R = 0x11;//arm interrupt
	
	NVIC_PRI7_R = (NVIC_PRI7_R & ~0x00E00000)|0x00400000;
	NVIC_EN0_R = 0x40000000;
}

void LED_Init(void){volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x20;//activate clock for port F
	delay = SYSCTL_RCGC2_R;//delay
	GPIO_PORTF_LOCK_R |= 0x4C4F434B;//unlock
	GPIO_PORTF_CR_R |= 0x0E;//allow changes
	GPIO_PORTF_AMSEL_R &= ~0x0E;//disable analog functionality
	GPIO_PORTF_PCTL_R &= ~0x0000FFF0;//configure to GPIO
	GPIO_PORTF_DIR_R |= 0x0E;//output
	GPIO_PORTF_AFSEL_R &= ~0x0E;//disable alt function
	GPIO_PORTF_PUR_R |= 0x0E;//
	GPIO_PORTF_DEN_R |= 0x0E;//enable digitle I/O
	
	GPIO_PORTF_DATA_R |= 0x02;//make red initially
	
}

void GPIOPortF_Handler(void){
	

	if(GPIO_PORTF_RIS_R&0x01){//if PF0    SW2	
		GPIO_PORTF_ICR_R = 0x01;
		////////////////
		
	}
	
	if(GPIO_PORTF_RIS_R&0x10){//if PF4		SW1
		GPIO_PORTF_ICR_R = 0x10;
		//////////////////////////
	}

}
void Timer1_Init(unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	sawtooth();
	
	}


int main(void){
	//Button_Init();
	//LED_Init();
	Timer1_Init(8000000);
	PortB_Init();
	
	while(1);
}
