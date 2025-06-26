// Data acquasition system using stm32F1 MCU
// Purpose : to take readings from multiple channels and print onto the computer for analysis and other purpose

#define PWM_TIMER   TIM3
uint16_t count_prev,count,del_count = 0;
uint32_t TimePeriod = 10000000;
uint32_t timestamp=0;
uint16_t freq = 0;
uint8_t skip_count =1;
uint8_t skip_update = 0;
int d[] = {0, 0, 0, 0};
uint8_t DigitIndex = 0;
uint8_t DecimalPlaceArray[10] = {      4,       2,      2,      1,      1,      1,       4,          4,        4,        4            };  // Either two decimal place or one decimal place 4 means no decimal
uint16_t DigitSift[10] =         {      1,      10,     10,     10,     10,     100,     1000,       1,        1,       1         };
                                //   time     20v     20v     20v     20v      40v     100v      analog   speed     count         // 4 means no decimal
uint8_t MeasurementIndex = 0;

uint8_t DecimalPlace = 4;       // from right  0 means no decimal
int32_t value = 0;            // Number that will be displayed
uint32_t n =0;                 // Stores numerical Value of float to segmented for 7 segment display
uint32_t ccr = 1;



int MenueIndex = 1;
int FunctionIndex = 2;      // Function selector // 1: button triggeredx, 2:time triggered
int DisplayIndex = 8;        // which channel will be displayed minimum 1
uint16_t TimeBase[12] = {10,20,30,50,100,500,1000,2000,5000,10000,30000,60000};           // data capture time gap in ms 
int TimeBaseIndex = 3;         // 



bool FunctionChange = false;       //  Controls whether up or down key can change function
bool DisplayChange = false;       //  Controls whether up or down key can change what is being displayed on the 7 segment display 
bool TimeBaseChange = false;      //  Controls whether up or down key can change timebase
bool acquare = false;             //  enables acquaring the analog reading
bool print = false;               //  enables the print sequence as printing takes several milliseconds
bool FreezDisplay = false;        //  Keep display to stick showing the current setting
bool AutoAcquare = false;         //  boolean to enable auto acquare based on timebase
bool NegativeValue = false;
bool CountReset = false;
bool buzz = false;
bool sweep = false;



unsigned long previousMillis1 = 0, previousMillis2 = 0;
unsigned long Timer_print = 0;                             // controls the printing in the cells
unsigned long Timer_send = 0;                              // controls the start of the print sequence in case of automatic printing
unsigned long Timer_DisplayUpdate = 0;
unsigned long currentMillis2 =0;
unsigned long Timer_buzz = 0;



const int segmentMap[22] = {      // Binary for controlling the segment
      0b1111111000000111,   // 0
      0b1111111111001111,   // 1
      0b1111110100100111,   // 2
      0b1111110110000111,   // 3
      0b1111110011001111,   // 4
      0b1111110010010111,   // 5
      0b1111110000010111,   // 6
      0b1111111111000111,   // 7
      0b1111110000000111,   // 8
      0b1111110010000111,   // 9
      0b1111110111111111,   // 10(-)
      0b1111110001000111,   // 11(A)
      0b1111110000011111,   // 12(b)
      0b1111111000110111,   // 13(c)
      0b1111110100001111,   // 14(d)
      0b1111110000110111,   // 15(e)
      0b1111110001110111,   // 16(F)
      0b1111110001100111,   // 17(P)     
      0b0000011111111000,   // 18 (Blank)
      0b1111110111111111,   // 19 (-)
      0b1111111000111111,   // 20(L)
      0b1111111000001111    // 21(U)

};


const int digitMap[4] = {
                0b0100000000000000,       // 0        
                0b0010000000000000,       // 1       
                0b0001000000000000,       // 2        
                0b0000100000000000,       // 3                        
} ;


// How it works?
// In the void loop, measure() is started by an interupt, ones measurement is finished print boolian becomes true starting the print sequence,
// display is controlled by a non blocking code.


int32_t reading[11] = {0,0,0,0,0,0,0,0,0};

uint16_t measurement[9] = {0,0,0,0,0,0,0,0};   // array to store measured value


void botton_trigger(){                           // Interupt function for button1
            if(FunctionChange || DisplayChange || TimeBaseChange == true){       // used as ok button
              FunctionChange = false; 
              DisplayChange = false;
              TimeBaseChange = false;
              FreezDisplay = false;
            }

            if(FunctionIndex==1){                                          // used as trigger to read one set of data 
                  acquare = true;
            }
            if(FunctionIndex==2){                                          // used to toggle autoacquare on or off 
              AutoAcquare = !AutoAcquare;
             
            }
 
            if(FunctionIndex==3){        
                  AutoAcquare = true;                                  // used as trigger to read one set of data 
                  sweep = true;
                  
            }
                  buzz = true;      
}


void digit_calculator(uint32_t n){
              for(uint8_t i = 0; i<4 ;i++ ){
              d[i] = n%10;
              n = n/10;   
               }

}

void botton_menue(){       // Interupt function for button1
    FreezDisplay = true;
    DecimalPlace = 4;
    d[0] = 18;
    d[1] = 18;
    d[2] = 18;
    d[3] = 18;     // earase the digit
    acquare = false;
    FunctionChange = false;
    DisplayChange = false;
    TimeBaseChange = false;
      if(MenueIndex==1){
          d[3] = 16;       // display F  for function
          d[2] = FunctionIndex; 
          FunctionChange = true;
      }
      
      if(MenueIndex==2){
          d[3] = 12;        // display b  basetime
          n = TimeBase[TimeBaseIndex]/10;
          DecimalPlace = 2;
          digit_calculator(n);
          TimeBaseChange = true;
      }

      if(MenueIndex==3){
          d[3] = 14;        // display d  for display
          d[2] = DisplayIndex;
          DisplayChange = true;
      }
      if(MenueIndex==4){
        CountReset = true;
          d[3] = 13;        // display c for count reset
          d[1] = 0;
      }
      MenueIndex++;
      if(MenueIndex>4){MenueIndex=1;}

}

void botton_up(){       // Interupt function for button1
        if(FunctionChange){
          FunctionIndex++;
          if(FunctionIndex>3){FunctionIndex=3;}
          d[2] = FunctionIndex;          
        }
        else if(DisplayChange){
          DisplayIndex++;
          if(DisplayIndex>9){DisplayIndex=9;}
          d[2] = DisplayIndex;
        }
        else if(TimeBaseChange){
          TimeBaseIndex++;
          if(TimeBaseIndex>11){TimeBaseIndex=11;}
          n = TimeBase[TimeBaseIndex]/10;
          DecimalPlace = 2;
          digit_calculator(n);
        }
}

void botton_down(){       // Interupt function for button1
        if(FunctionChange){
          FunctionIndex--;
          if(FunctionIndex<1){FunctionIndex=1;}
          d[2] = FunctionIndex;          
        }
        else if(DisplayChange){
          DisplayIndex--;
          if(DisplayIndex<1){DisplayIndex=1;}
          d[2] = DisplayIndex;
        }
        else if(TimeBaseChange){
          TimeBaseIndex--;
          if(TimeBaseIndex<0){TimeBaseIndex=0;}
          n = TimeBase[TimeBaseIndex]/10;
          DecimalPlace = 2;
          digit_calculator(n);
        }
}

void button_alternate(){
  if(CountReset){
            count = 0;
            CountReset = false;
            d[0] = 19;
            d[1] = 19;
            d[2] = 19;
          }

          buzz = true;
}




// writing segments
void Write_digit(int d) {
  GPIOB->ODR |= 0b0000011111111000;
  GPIOB->ODR &= segmentMap[d];
}



void measure(){
      measurement[0] = millis()/1000;
      measurement[1] = adc_read_value(PA_0, 12);       // 5v range   adcRaw = readADC(0)
      measurement[1] = adc_read_value(PA_0, 12);
      measurement[2] = adc_read_value(PA_1, 12);       // 5v range
      measurement[3] = adc_read_value(PA_2, 12);       // 20v range
      measurement[4] = adc_read_value(PA_3, 12);       // 20v range 
      measurement[5] = adc_read_value(PA_4, 12);       // 40v range
      measurement[6] = adc_read_value(PA_5, 12);       // 100v range
      measurement[7] = adc_read_value(PA_6, 12);;

                                                      

      // All readings will be multiplied by 1000 including speed and count                                                
      reading[0] = measurement[0];
      reading[1] = (89047*(measurement[1]-2037))/10000;     // 20v range CH1                      // Voltages will be recorded as mV
      reading[2] = (88653*(measurement[2]-2037))/10000;     // 20v range CH2
      reading[3] = (89446*(measurement[3]-2037))/10000;     // 20v range CH3
      reading[4] = (90662*(measurement[4]-2037))/10000;     // 20v range CH4
      reading[5] = (213675*(measurement[5]-2037))/10000;
      reading[6] = (215818*(measurement[5]-2037))/10000;
      reading[7] = measurement[7];
      if(TimePeriod>10000000){
          TimePeriod=10000000;
      }
      if(TimePeriod<100){
          TimePeriod=100;
      }
     
      reading[8] = 1000000/TimePeriod;                      // speed
      reading[9] = count;                                   // Counting
      reading[10] = ccr;
     
      

}

void SpeedMeasurment(){
  TimePeriod = micros()-timestamp;
  timestamp = micros();
  count++;
}

void PWM_init(){
  GPIOA->CRL &= ~(0xF << (7 * 4));
  //    set MODE = 11 (50 MHz), CNF = 10 (AF PP) → 0b1011 = 0xB
  GPIOA->CRL |=  (0xB << (7 * 4));

  // 2) Enable TIM3 clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // 3) PWM timing: 72 MHz / (PSC+1) → 1 MHz; 1 MHz / (ARR+1) → 1 kHz
  const uint32_t pwm_freq = 50000;
  uint32_t prescaler = 5;                             // 72 MHz / (5+1) = 12 MHz of timer frequency
  uint32_t arr       = (12'000'000 / pwm_freq) - 1;      // 12 MHz / 50 kHz = 240 ticks → ARR=239

  PWM_TIMER->PSC = prescaler;
  PWM_TIMER->ARR = arr;

  // 4) PWM Mode 1 on CH1, preload enable
  PWM_TIMER->CCMR1 &= ~TIM_CCMR1_CC2S;                  // CC2 as output
  PWM_TIMER->CCMR1 |= (6 << 12)    /* OC1M = 110 */ 
                    | TIM_CCMR1_OC2PE;

  // 5) Enable CH1 output
  PWM_TIMER->CCER |= TIM_CCER_CC2E;

  // 6) ARPE + counter enable
  PWM_TIMER->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;

}



void setup() {
      // interupt pins
  //pinMode(PC13, OUTPUT);
  pinMode(PB0, INPUT_PULLDOWN);   // for button
  pinMode(PA8, INPUT_PULLDOWN);   // for button
  pinMode(PB15, INPUT_PULLDOWN);  // for button
  pinMode(PB1, INPUT_PULLDOWN);   // Speed measurement
  pinMode(PA9, INPUT_PULLDOWN);   // for button
  pinMode(PA10, INPUT_PULLDOWN);


  //pinMode(PA10, OUTPUT);
      // analog reding pins
  pinMode(PA0, INPUT);
  pinMode(PA1, INPUT);
  pinMode(PA2, INPUT);
  pinMode(PA3, INPUT);
  pinMode(PA4, INPUT);
  pinMode(PA5, INPUT);
  pinMode(PA6, INPUT);


  pinMode(PA7, OUTPUT);   // generating PWM on PA7
  pinMode(PA15, OUTPUT);

      // Set PB3 to PB14 as output that is for display driving
  pinMode(PB3, OUTPUT);
  pinMode(PB4, OUTPUT);
  pinMode(PB5, OUTPUT);
  pinMode(PB6, OUTPUT);
  pinMode(PB7, OUTPUT);
  pinMode(PB8, OUTPUT);
  pinMode(PB9, OUTPUT);
  pinMode(PB10, OUTPUT);
  pinMode(PB11, OUTPUT);
  pinMode(PB12, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB14, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PB0), botton_trigger, RISING);
  attachInterrupt(digitalPinToInterrupt(PB15), botton_menue, RISING);
  attachInterrupt(digitalPinToInterrupt(PA9), botton_up, RISING);
  attachInterrupt(digitalPinToInterrupt(PA10), botton_down, RISING);
  attachInterrupt(digitalPinToInterrupt(PA8), button_alternate, RISING);
  attachInterrupt(digitalPinToInterrupt(PB1), SpeedMeasurment, RISING);
  Serial.begin(115200);
  PWM_init();
  GPIOA->ODR &= ~(0b1000000000000000);
}            

void loop(){


  ////// Acquare //////

  if(acquare){                // when aquare is enabled it measures the voltages and starts printing by enabling 
    measure();                // the print variable, ones printing is finished print variable becomes false
    print=true;
    acquare = false;
  }
            if(print){       // Print values
                      for(int i=0; i<11; i++){
                                Serial.print(reading[i]);
                                Serial.print(",");
                              }
                              Serial.println(); 
                              print = false;
                              ccr++;
                              if(ccr>238){
                                ccr=1;
                                  if(sweep){
                                      AutoAcquare = false;
                                  }
                                }       // arr is going upto 239
                              PWM_TIMER->CCR2=ccr;      // directly comparing the ccr value to the arr ramp.
            }

  if(AutoAcquare==true){                                          // makes variable acquare true at every TimeBase interval to start measuring and printing sequence
    if(millis()-Timer_send>TimeBase[TimeBaseIndex])
    {
      Timer_send = millis();
      acquare = true;
    }
  }

  

      //////  Buzzer /////

  if(buzz){
    GPIOA->ODR |= 0b1000000000000000;
    Timer_buzz = millis();
    buzz = false;
  }
  if(millis()-Timer_buzz>50){
      GPIOA->ODR &= ~(0b1000000000000000);
  }

      
      
      
      ///// Display update /////

  if((millis()-Timer_DisplayUpdate>500)&&(!FreezDisplay)){       // update the display digit with measured value only when display is not freezed at every 0.5s
    Timer_DisplayUpdate = millis();
    measure();
    value = reading[DisplayIndex]/DigitSift[DisplayIndex];       // Updating display
    DecimalPlace = DecimalPlaceArray[DisplayIndex];
    if(value<0){                                                 //  Checking for negative value
      NegativeValue = true;
      value=-value;
    }
    else{NegativeValue = false;}
    n = value;
    digit_calculator(n);
  }

    currentMillis2 = millis();                        // update display every 2ms
    if(currentMillis2 - previousMillis2 >= 2) {
          previousMillis2 = currentMillis2;
          Write_digit(d[DigitIndex]);
          GPIOB->ODR &= 0b1000011111111111;           // Clear all digit control lines
          GPIOB->ODR |= digitMap[DigitIndex];
          GPIOB->ODR |= 0b0000010000000000;
          if(DigitIndex==DecimalPlace){
            GPIOB->ODR &= 0b1111101111111111;
          }
          if((DigitIndex==0)&&NegativeValue){
            GPIOB->ODR &= 0b1111101111111111;
          }
    DigitIndex++;
    if (DigitIndex > 3) {
      DigitIndex = 0;
      } 
  }

      ///// Serial listning /////

   if(Serial.available()){
      char ch = Serial.read();
      if(ch == 't'){
        botton_trigger();
      }
   } 

}
