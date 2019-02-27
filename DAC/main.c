#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <math.h>

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06

#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 20 nsec for 50 MHz clock)
void SysTick_Wait(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(160000);  // wait 10ms (assumes 16 MHz clock)
  }
}
//from index 0 to index 255 the voltage output is defined as (1.65*sin((2pi/255)*x)+ 1.65) which ranges from 0 to 3.3v
//following the voltage from 0 to 255 would create a sine wave centered at 1.65V with an amplitude of 1.65V
//We want the output to be sent out in binary to our DAC so we have to translate the voltage from Voltage to a binary number
//Since the voltage from 0 to 3.3v corresponds to 0 to 255 we can take the proportion of out current voltage to the maximum
//and the muliply it by 255 to get out corresponging decimal number. 
//For example at index 255 the sin wave contents becomes 2pi, so the left side of the equation becomes 0 leaving 1.65
//1.65/3.3 is 1/2 or .5 there for we want to output our maximum binary value devided by 2
//255/2 is roughly 128 which will be represented on our DAC circuit as 1.65 volts. 
float sine_out[256] = {//sine_out[x] = NUMBER;   NUMBER = ((1.65*sin((2pi/255)*x)+ 1.65)/3.3v)*255
127.5
,130.6412748 
,133.7806425
,136.9161973
,140.0460355
,143.1682571
,146.2809665
,149.3822741
,152.470297
,155.5431606
,158.5989993
,161.6359579
,164.6521927
,167.6458726
,170.61518
,173.5583124
,176.473483
,179.358922
,182.2128776
,185.0336173
,187.8194285
,190.56862
,193.2795229
,195.9504912
,198.5799035
,201.1661635
,203.7077011
,206.2029732
,208.6504651
,211.0486909
,213.3961946
,215.691551
,217.9333667
,220.1202807
,222.2509652
,224.3241269
,226.338507
,228.2928826
,230.1860672
,232.0169116
,233.7843041
,235.4871719
,237.1244811
,238.6952377
,240.1984881
,241.6333196
,242.9988614
,244.2942842
,245.5188016
,246.6716703
,247.7521904
,248.7597058
,249.693605
,250.5533209
,251.3383316
,252.0481605
,252.6823768
,253.2405953
,253.7224772
,254.1277299
,254.4561075
,254.7074105
,254.8814864
,254.9782294
,254.997581
,254.9395293
,254.8041095
,254.5914039
,254.3015417
,253.9346987
,253.4910977
,252.971008
,252.3747454
,251.7026718
,250.9551952
,250.1327695
,249.2358939
,248.265113
,247.221016
,246.1042368
,244.9154535
,243.6553878
,242.3248045
,240.9245116
,239.4553591
,237.918239
,236.3140844
,234.6438691
,232.9086073
,231.1093523
,229.2471965
,227.3232703
,225.3387419
,223.2948159
,221.1927333
,219.0337702
,216.8192373
,214.550479
,212.2288728
,209.855828
,207.4327853
,204.9612157
,202.4426197
,199.8785263
,197.2704922
,194.6201008
,191.928961
,189.1987066
,186.4309952
,183.627507
,180.789944
,177.9200289
,175.019504
,172.0901302
,169.1336859
,166.1519659
,163.1467806
,160.1199541
,157.0733243
,154.0087406
,150.9280635
,147.8331634
,144.7259191
,141.6082169
,138.4819498
,135.3490155
,132.2113162
,129.0707566
,125.9292434
,122.7886838
,119.6509845
,116.5180502
,113.3917831
,110.2740809
,107.1668366
,104.0719365
,100.9912594
,97.92667571
,94.88004586
,91.85321944
,88.84803405
,85.86631411
,82.90986982
,79.98049602
,77.07997112
,74.21005603
,71.37249304
,68.56900485
,65.80129343
,63.07103905
,60.37989923
,57.72950777
,55.12147369
,52.55738033
,50.03878434
,47.56721475
,45.14417203
,42.77112721
,40.44952096
,38.18076271
,35.96622981
,33.8072667
,31.70518408
,29.66125812
,27.67672966
,25.75280352
,23.8906477
,22.09139271
,20.35613087
,18.68591564
,17.08176102
,15.54464087
,14.07548838
,12.67519546
,11.34461222
,10.08454646
,8.89576316
,7.778984022
,6.734887038
,5.764106077
,4.867230495
,4.044804783
,3.297328232
,2.625254631
,2.028991994
,1.50890231
,1.065301322
,0.6984583405
,0.4085960731
,0.1958904942
,0.06047073655
,0.002419013037
,0.0217705666
,0.118513649
,0.2925895278
,0.5438925224
,0.8722700676
,1.277522807
,1.759404712
,2.317623236
,2.951839486
,3.661668431
,4.446679137
,5.306395028
,6.240294172
,7.247809605
,8.328329667
,9.481198378
,10.70571584
,12.00113864
,13.36668035
,14.80151195
,16.30476235
,17.87551894
,19.51282812
,21.21569588
,22.98308843
,24.81393278
,26.70711743
,28.66149305
,30.67587313
,32.74903475
,34.87971931
,37.06663328
,39.30844899
,41.60380544
,43.95130912
,46.34953489
,48.79702679
,51.29229894
,53.8338365
,56.42009648
,59.0495088
,61.72047714
,64.43137997
,67.1805715
,69.96638273
,72.78712238
,75.64107801
,78.52651698
,81.44168756
,84.38481996
,87.35412742
,90.34780727
,93.36404208
,96.40100069
,99.45683938
,102.529703
,105.6177259
,108.7190335
,111.8317429
,114.9539645
,118.0838027
,121.2193575
,124.3587252
,127.5
};

int twinkle[42] = {262,262,392,392,440,440,392,349,349,330,330,294,294,262,392,392,349,349,330,330,294,392,392,349,349,330,330,294,262,262,392,392,440,440,392,349,349,330,330,294,294,262};
		
char d0 = 0, d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0, d6 = 0;
int sine_check = 0;
int i = 0;//to iterate through waves is timer isr
unsigned int y = 0;//to itterate through twinkle in mainloop
int twink = 0;//flag to activate twinkle little star song

void sine_wave(void){
	
	sine_check = (sine_check == 255)? 0: sine_check + 1;
	GPIO_PORTB_DATA_R  = sine_out[sine_check];
	
}


int square_check = 0, UPHDL = 0, triangle_check = 0;

void sawtooth(void){
	
	GPIO_PORTB_DATA_R =(GPIO_PORTB_DATA_R == 255)? 0: GPIO_PORTB_DATA_R + 1;
	//GPIO_PORTB_DATA_R |= 0xFF;
}

void squarewave(void){
	square_check += 1;
	square_check = (square_check > 255)? 0: square_check;
	GPIO_PORTB_DATA_R =(square_check < 128)? 0x00: 0xFF;	
}



void trianglewave(void){
	UPHDL = ((GPIO_PORTB_DATA_R == 0) || (GPIO_PORTB_DATA_R == 254))?  UPHDL^1: UPHDL;
	GPIO_PORTB_DATA_R =(UPHDL)? GPIO_PORTB_DATA_R + 2: GPIO_PORTB_DATA_R - 2;
	}

	
	
void PortE_Init(void){volatile unsigned long delay;

	SYSCTL_RCGC2_R |= 0x10;//activating port E clock
	delay = SYSCTL_RCGC2_R;//delay
	GPIO_PORTE_CR_R |= 0x1E;//allows change to port E1
	GPIO_PORTE_AMSEL_R &= ~0x1E;////Disables Analog functionality on all of port E
	GPIO_PORTE_PCTL_R &= 0x00000000;//All pins are set up for GPIO
	GPIO_PORTE_DIR_R &= ~0x1E;//All pins are output
	GPIO_PORTE_AFSEL_R &= ~0x1E;//All pins are not using alternate function
	//GPIO_PORTE_PUR_R |= 0x1E;//
	GPIO_PORTE_DEN_R |= 0x1E;//All pins are digitaly enabled
	//initializing port D
	GPIO_PORTE_DATA_R &= ~0x1E;//Clears PE1 on Port E
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

void PortD_Init(void){volatile unsigned long delay;

	SYSCTL_RCGC2_R |= 0x08;//activating port D clock
	delay = SYSCTL_RCGC2_R;//delay
	GPIO_PORTD_CR_R |= 0xCC;//allows changes to all bits of port D
	GPIO_PORTD_AMSEL_R &= ~0xCC;////Disables Analog functionality on all of port D
	GPIO_PORTD_PCTL_R &= 0x00000000;//All pins are set up for GPIO
	GPIO_PORTD_DIR_R &= ~0xCC;//All pins are output
	GPIO_PORTD_AFSEL_R &= ~0xCC;//All pins are not using alternate function
	//GPIO_PORTD_PUR_R |= 0xCC;//
	GPIO_PORTD_DEN_R |= 0xCC;//All pins are digitaly enabled
	//initializing port D
	GPIO_PORTD_DATA_R &= ~0xCC;//Clears all bit on Port D
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
 // TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_TailerLoad(unsigned long period){
	//TIMER1_CTL_R = 0x00000000;
	TIMER1_TAILR_R = period-1;
	
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	//squarewave();
	//sawtooth();
	//trianglewave();
	//sine_wave();
	GPIO_PORTF_DATA_R = 0x02;
	if(i == 0){
		sawtooth();
	}
	else if(i == 1){
		trianglewave();
		
	}
	else if(i == 2){
		squarewave();
		
	}
	else if(i == 3){
		
		sine_wave();
	}
	
	
	}

	
	void GPIOPortF_Handler(void){
		

	if(GPIO_PORTF_RIS_R&0x01){//if PF0    SW2	
		GPIO_PORTF_ICR_R = 0x01;
		////////////////
		i+= 1;
		i%=4;
	}
	
	if(GPIO_PORTF_RIS_R&0x10){//if PF4		SW1
		GPIO_PORTF_ICR_R = 0x10;
		//////////////////////////
		twink = 1;
	}

}
	

int main(void){ unsigned long frequency = 142;
	SysTick_Init();
	Button_Init();
	LED_Init();
	PortB_Init();
	PortD_Init();
	PortE_Init();
	Timer1_Init(frequency);///16000000/440/256          440 hz default but doesnt play automaticaly because timer1 is intially disabled
	while(1){
		
		
		GPIO_PORTF_DATA_R = (GPIO_PORTD_DATA_R&0x04)? 0x02: 
												(GPIO_PORTD_DATA_R&0x08)? 0x04: 
												(GPIO_PORTD_DATA_R&0x40)? 0x06: 
												(GPIO_PORTE_DATA_R&0x02)? 0x08: 
												(GPIO_PORTE_DATA_R&0x04)? 0x0A: 
												(GPIO_PORTE_DATA_R&0x08)? 0x0C: 
												(GPIO_PORTE_DATA_R&0x10)? 0x0E: 
											  0x00;
		
		TIMER1_CTL_R =  (GPIO_PORTD_DATA_R||GPIO_PORTE_DATA_R)?   0x00000001:  0x00000000; 
												
		if(((GPIO_PORTD_DATA_R&0x04) == 0x04)&&(d0 == 0x00)){//C
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(240);
			TIMER1_CTL_R =  0x00000001;
			d0 = 0xFF;
		}
		else{
			d0 = (GPIO_PORTD_DATA_R&0x04)? 0xFF: 0x00;
		}
		
		if(((GPIO_PORTD_DATA_R&0x08) == 0x08)&&(d1 == 0x00)){//D
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(213);
			TIMER1_CTL_R =  0x00000001;
			d1 = 0xFF;
		}
		else{
			d1 = (GPIO_PORTD_DATA_R&0x08)? 0xFF: 0x00;
		}
		
		if(((GPIO_PORTD_DATA_R&0x40) == 0x40)&&(d2 == 0x00)){//E
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(190);
			TIMER1_CTL_R =  0x00000001;
			d2 = 0xFF;
		}
		else{
			d2 = (GPIO_PORTD_DATA_R&0x40)? 0xFF: 0x00;
		}
		
		if(((GPIO_PORTE_DATA_R&0x02) == 0x02)&&(d3 == 0x00)){//F
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(180);
			TIMER1_CTL_R =  0x00000001;
			d3 = 0xFF;
		}
		else{
			d3 = (GPIO_PORTE_DATA_R&0x02)? 0xFF: 0x00;
		}
		
		if(((GPIO_PORTE_DATA_R&0x04) == 0x04)&&(d4 == 0x00)){//G
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(160);
			TIMER1_CTL_R =  0x00000001;
			d4 = 0xFF;
		}
		else{
			d4 = (GPIO_PORTE_DATA_R&0x04)? 0xFF: 0x00;
		}
		
		if(((GPIO_PORTE_DATA_R&0x08) == 0x08)&&(d5 == 0x00)){//A
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(143);
			TIMER1_CTL_R =  0x00000001;
			d5 = 0xFF;
		}
		else{
			d5 = (GPIO_PORTE_DATA_R&0x08)? 0xFF: 0x00;
		}

		if(((GPIO_PORTE_DATA_R&0x10) == 0x10)&&(d6 == 0x00)){//B
			TIMER1_CTL_R =  0x00000000;
			Timer1A_TailerLoad(127);
			TIMER1_CTL_R =  0x00000001;
			d6 = 0xFF;
		}
		else{
			d6 = (GPIO_PORTE_DATA_R&0x10)? 0xFF: 0x00;
		}
		
		
		if(twink == 1){//extra twinkle little star funciton
			y = 0;
			while(y < 42){
			GPIO_PORTF_DATA_R ^= 0xC;//testing delays
			TIMER1_CTL_R =  0x00000000;		//disable timer1a	
			Timer1A_TailerLoad(62500/twinkle[y]);//set timer period
			TIMER1_CTL_R = 0x00000001;    // enable TIMER1A			
			SysTick_Wait10ms(10);//quarter second
			if(((y%7)==0)&&(y!=0)){
			SysTick_Wait10ms(25);//quarter second
			}
			TIMER1_CTL_R =  0x00000000;	
			SysTick_Wait10ms(25);//quarter second
			
				
			y +=1;
		}
		twink = 0;
		
	}
		
	
}
}






