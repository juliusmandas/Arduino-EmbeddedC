#define Sender 10  // Output PIN
#define BUTTON1 5
#define BUTTON2 6
#define BUTTON3 7

//Timer frequency is 2MHz for ( /8 prescale from 16MHz )
#define TIMER_SHORT 0x8D  // 58usec pulse length 
#define TIMER_LONG  0x1B  // 116usec pulse length 

unsigned char last_timer=TIMER_SHORT;  // store last timer value
   
unsigned char flag=0;  // used for short or long pulse
unsigned char every_second_isr = 0;  // pulse up or down

// Forskellige stadier i programmet
#define PREAMBLE 0    
#define SEPERATOR 1
#define SENDBYTE  2

//Kommandoer
#define ADR 0x28 // ADDRESSEN
#define BWRD 0x40 // bagud
#define FWRD 0x60 // forud

//Sætter første stadie til at være preamble
unsigned char state = PREAMBLE;
//Preamble er 16 lang
unsigned char preamble_count = 16;

//Nuværende byte i arrayed
unsigned char outbyte = 0;

//Nuværende bit
unsigned char cbit = 0x80;

unsigned int SPEED = 0x00;

// Holder et array med 7 bits i og en længde til at definere hvor mange af bitsene den skal læse
struct Message {
   unsigned char data[7];
   unsigned char len;
} ;


// MAXMSG = Hvor mange forskellige beskeder skal den sende ud
#define MAXMSG 3


/* I vores tilfælde skal len kun være 3 fordi vi kun skal tænke på adress, order, checksum
 preabmle bliver lavet i det første stadie fordi alle pakker skal have en preamble */
struct Message msg[MAXMSG] = { 
    { { 0xFF ,0x00, 0xFF,  0, 0, 0, 0}, 3},
    { { 0xFF ,0x00, 0xFF,  0, 0, 0, 0}, 3},
    { { 0xFF ,0x00, 0xFF,  0, 0, 0, 0}, 3}
  };               

//Start besked og byte indexes
int msgIndex=0;
int byteIndex=0;

//Setup Timer2.
//Configures the 8-Bit Timer2 to generate an interrupt at the specified frequency.
//Returns the time load value which must be loaded into TCNT2 inside your ISR routine.

void SetupTimer2(){
  //Timer2 Settings: Timer Prescaler /8, mode 0 
  //Start count
  TCCR2A = 0;
  //Stop count
  TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20; 
  //Timer2 Overflow Interrupt Enable   
  TIMSK2 = 1<<TOIE2;
  //load the timer for its first cycle
  TCNT2=TIMER_SHORT; 
}

//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect) {
  //Capture the current timer value TCTN2. This is how much error we have
  //due to interrupt latency and the work in this function
  //Reload the timer and correct for latency.  
  // for more info, see http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
  unsigned char latency;
  
  // Første gang send LOW pulse, 
  if (every_second_isr)  {
     digitalWrite(Sender , HIGH);
     every_second_isr = 0;        
     // set timer to last value
     latency=TCNT2;
     TCNT2=latency+last_timer;  
  }  else  {  // != every second interrupt, advance bit or state
     digitalWrite(Sender, LOW);
     every_second_isr = 1; 
     switch(state)  {
       case PREAMBLE:
           flag=1; // Short pulse
           preamble_count--;
           if (preamble_count == 0)  {  // advance to next state
              state = SEPERATOR;
              msgIndex++; // Hent næste besked
              if (msgIndex >= MAXMSG)  {  msgIndex = 0; } //Hvis msgIndex er lig med eller større end vores maxMSG så start forfra 
              byteIndex = 0; //Start altid ved byte 0 / det vil sige den første i vores array
           }
           break;
        case SEPERATOR:
           flag=0; // long pulse
           // Videre til sendbyte stadiet
           state = SENDBYTE;
           // goto next byte ...
           cbit = 0x80;  // send this bit next time first
           //The bit in dat[7] array         
           outbyte = msg[msgIndex].data[byteIndex];
           break;
        case SENDBYTE:
           if (outbyte & cbit)  { // and operator if 1==1 = 1 or 0==0 = 0
              flag = 1;  // Short pulse
           }  else  {
              flag = 0;  // Long pulse
           }
           cbit = cbit >> 1; //bit shift right to check 1 or 0
           if (cbit == 0)  {  // last bit sent, is there a next byte? only stop when currentbyte is traversed
              byteIndex++;
              if (byteIndex >= msg[msgIndex].len)  {
                 // this was already the XOR byte then advance to preamble
                 state = PREAMBLE;
                 preamble_count = 16;
              }  else  {
                 // send separtor and advance to next byte
                 state = SEPERATOR ;
              }
           }
           break;
     }   
     //Short pulse (data == 1)
     if (flag)  {
        latency=TCNT2;
        TCNT2=latency+TIMER_SHORT;
        last_timer=TIMER_SHORT;
     }
     //Long pulse 
     else  {   
        latency=TCNT2;
        TCNT2=latency+TIMER_LONG; 
        last_timer=TIMER_LONG;
     }  
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(Sender, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  SetupTimer2();   
}


/*DEBOUNCER */
int buttonState;
int lastButtonState = HIGH;   
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int SOUND = 0;

void loop(){
 if(!digitalRead(BUTTON1)){
  frw(&msg[0]);
 }
 if(!digitalRead(BUTTON2)){
  brw(&msg[0]);
 }

  int reading = digitalRead(BUTTON3);
  
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        if(SOUND){
          //sOff(msg[1]);
          sOff(&msg[1]);
          
          SOUND = 0;
        }
        else{
          sHorn12(&msg[1]);
          SOUND = 1;
        }
      }
    }
  }
  lastButtonState = reading;
 
}


void frw(Message *x){
  noInterrupts();
    x->data[0] = ADR;
    x->data[1] = FWRD | SPEED;
    x->data[2] = x->data[0] ^ x->data[1];
  interrupts();
}

void brw(Message *x){
  noInterrupts();
    x->data[0] = ADR;
    x->data[1] = BWRD | 0x44;
    x->data[2] = x->data[0] ^ x->data[1];
  interrupts();
}

void sHorn12(Message *x){
  noInterrupts();
    x->data[0] = ADR;
    x->data[1] = 0x90;
    x->data[2] = x->data[0] ^ x->data[1];
  interrupts();
}


void sOff(Message *x){
  noInterrupts();
    x->data[0] = ADR;
    x->data[1] = 0x80;
    x->data[2] = x->data[0] ^ x->data[1];
  interrupts();
}




