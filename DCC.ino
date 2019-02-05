void setupTimer2()
{
  //Timer2 settings: Timer Prescale /8, mode 8
  //Timer clock = 16Mhz/8 = 2Mhz or 0,5 uS
  TCCR2A = 0;
  TCCR2B = 0 << CS22 | 1 << CS21 | 0 << CS20;
  //Timer2 overflow interrupt enable
  TIMSK2 = 1 << TOIE2;
  //Load the time for the first cycle
  TCNT2 = TIMER_SHORT;
}

/* DCC Commands*/

void writeIdle()
{
  struct Frame *newFrame = (struct Frame*)malloc(sizeof(struct Frame));

  newFrame->data[0] = 0xFF;
  newFrame->data[1] = 0x00;
  newFrame->data[2] = newFrame->data[0] ^ newFrame->data[1];
  newFrame->len = 3;
  queue(newFrame);
}

//SPEED
//TODO: Rewrite to speed + direction or define speed consts
void writeSpeed(uint8_t adr, uint8_t speed)
{
  Serial.print("Speed: ");
  Serial.print(adr);
  Serial.print(" ");
  Serial.println(speed);
  //TODO: Move down
  struct Frame *newFrame = (struct Frame*)malloc(sizeof(struct Frame));

  newFrame->data[0] = adr;

  if (speed == 14)
    newFrame->data[1] = 0x60;
  else
  {
    if (speed > 14)
      newFrame->data[1] = 0x60 | (speed - 13);
    else
      newFrame->data[1] = 0x40 | (28 - speed - 13);
  }
  newFrame->data[2] = newFrame->data[0] ^ newFrame->data[1];
  newFrame->len = 3;

  //noInterrupts();
  queue(newFrame);
  //interrupts();
}

void writeFuncRegister(uint8_t adr, uint8_t reg)
{
  struct Frame *newFrame = (struct Frame*)malloc(sizeof(struct Frame));

  newFrame->data[0] = adr;
  newFrame->data[1] = FUNCR_BASE | reg;
  newFrame->data[2] = newFrame->data[0] ^ newFrame->data[1];
  newFrame->len = 3;
  queue(newFrame);
}


void writeAcc(unsigned int adr, uint8_t command)
{
  uint8_t realAdr;
  uint8_t reg;

  if (adr % 4 == 0)
  {
    realAdr = adr / 4;
    reg = 3;
  }
  else
  {
    realAdr = (adr / 4) + 1;
    reg = (adr % 4) - 1;
  }
  uint8_t byte1 = realAdr & 0x3f;     //63  0011 1111
  byte1 += 0x80;                      //128 1000 0000
  uint8_t byte2 = 0x80;               //128 1000 0000

  unsigned int x = 0;
  uint8_t y = realAdr & 0x40;         //64  0100 0000

  if (y == 0)
  {
    x += 0x40;                        //64  0100 0000
  }

  y = realAdr & 0x80;                 //128 1000 0000
  if (y == 0)
  {
    x = x + 0x80;                     //128 1000 0000
  }

  y = realAdr & 0x100;                //256 0001 0000 0000
  if (y == 0)
  {
    x = x + 0x100;                    //256 0001 0000 0000
  }
  x = x >> 2;

  byte2 += x;
  byte2 += reg << 1;

  if (command == LIGHT_RED || command == SWITCH_TURN || command == CROSS_SWITCH_CROSS)
  {
    struct Frame *newFrame1 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame2 = (struct Frame*)malloc(sizeof(struct Frame));
    newFrame1->len = 3;
    newFrame2->len = 3;

    newFrame1->data[0] = byte1;
    newFrame2->data[0] = byte1;

    byte2 += 8;                 //OUTPUT 0 ON
    newFrame1->data[1] = byte2;
    byte2 -= 8;                 //OUTPUT 0 OFF
    newFrame2->data[1] = byte2;

    newFrame1->data[2] = newFrame1->data[0] ^ newFrame1->data[1];
    newFrame2->data[2] = newFrame2->data[0] ^ newFrame2->data[1];

    queue(newFrame1);
    queue(newFrame2);
  } 
  else if (command == LIGHT_GREEN || command == SWITCH_STRAIGHT || command == CROSS_SWITCH_STRAIGHT)
  {
    struct Frame *newFrame1 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame2 = (struct Frame*)malloc(sizeof(struct Frame));
    newFrame1->len = 3;
    newFrame2->len = 3;

    newFrame1->data[0] = byte1;
    newFrame2->data[0] = byte1;

    byte2 += 9;                 //OUTPUT 1 ON
    newFrame1->data[1] = byte2;
    byte2 -= 8;                 //OUTPUT 1 OFF
    newFrame2->data[1] = byte2;

    newFrame1->data[2] = newFrame1->data[0] ^ newFrame1->data[1];
    newFrame2->data[2] = newFrame2->data[0] ^ newFrame2->data[1];
  
    queue(newFrame1);
    queue(newFrame2);
  }
  else if (command == TRIPPLE_SWITCH_STRAIGHT)
  {
    struct Frame *newFrame1 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame2 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame3 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame4 = (struct Frame*)malloc(sizeof(struct Frame));

    newFrame1->len = 3;
    newFrame2->len = 3;
    newFrame3->len = 3;
    newFrame4->len = 3;

    //Fejlen
    newFrame1->data[0] = byte1;
    newFrame2->data[0] = byte1;
    newFrame3->data[0] = byte1;
    newFrame4->data[0] = byte1;

    byte2 += 9;               //OUTPUT 1 ON
    newFrame1->data[1] = byte2;
    byte2 -= 8;               //OUTPUT 1 OFF
    newFrame2->data[1] = byte2;

    reg = reg + 1;
    byte2 = 0x80 + x;
    byte2 += reg << 1;

    byte2 += 9;                //OUTPUT 1 ON
    newFrame3->data[1] = byte2;
    byte2 -= 8;               //OUTPUT 1 OFF
    newFrame4->data[1] = byte2;

    newFrame1->data[2] = newFrame1->data[0] ^ newFrame1->data[1];
    newFrame2->data[2] = newFrame2->data[0] ^ newFrame2->data[1];
    newFrame3->data[2] = newFrame3->data[0] ^ newFrame3->data[1];
    newFrame4->data[2] = newFrame4->data[0] ^ newFrame4->data[1];

    queue(newFrame1);
    queue(newFrame2);
    queue(newFrame3);
    queue(newFrame4);
  }
  else if (command == TRIPPLE_SWITCH_RIGHT)
  {
    struct Frame *newFrame1 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame2 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame3 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame4 = (struct Frame*)malloc(sizeof(struct Frame));

    newFrame1->len = 3;
    newFrame2->len = 3;
    newFrame3->len = 3;
    newFrame4->len = 3;

    newFrame1->data[0] = byte1;
    newFrame2->data[0] = byte1;
    newFrame3->data[0] = byte1;
    newFrame4->data[0] = byte1;

    byte2 += 8; //OUTPUT 0 ON
    newFrame1->data[1] = byte2;
    byte2 -= 8; //OUTPUT 0 OFF
    newFrame2->data[1] = byte2;

    reg++;
    byte2 = 0x80 + x;
    byte2 += reg << 1;

    byte2 += 9; //OUTPUT 1 ON
    newFrame3->data[1] = byte2;
    byte2 -= 8; //OUTPUT 1 OFF
    newFrame4->data[1] = byte2;

    newFrame1->data[2] = newFrame1->data[0] ^ newFrame1->data[1];
    newFrame2->data[2] = newFrame2->data[0] ^ newFrame2->data[1];
    newFrame3->data[2] = newFrame3->data[0] ^ newFrame3->data[1];
    newFrame4->data[2] = newFrame4->data[0] ^ newFrame4->data[1];

    queue(newFrame1);
    queue(newFrame2);
    queue(newFrame3);
    queue(newFrame4);
  }
  else if (command == TRIPPLE_SWITCH_LEFT)
  {
    struct Frame *newFrame1 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame2 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame3 = (struct Frame*)malloc(sizeof(struct Frame));
    struct Frame *newFrame4 = (struct Frame*)malloc(sizeof(struct Frame));

    newFrame1->len = 3;
    newFrame2->len = 3;
    newFrame3->len = 3;
    newFrame4->len = 3;

    newFrame1->data[0] = byte1;
    newFrame2->data[0] = byte1;
    newFrame3->data[0] = byte1;
    newFrame4->data[0] = byte1;

    byte2 += 9; //OUTPUT 1 ON
    newFrame1->data[1] = byte2;
    byte2 -= 8; //OUTPUT 1 OFF
    newFrame2->data[1] = byte2;

    reg++;
    byte2 = 0x80 + x;
    byte2 += reg << 1;

    byte2 += 8; //OUTPUT 0 ON
    newFrame3->data[1] = byte2;
    byte2 -= 8; //OUTPUT 0 OFF
    newFrame4->data[1] = byte2;

    newFrame1->data[2] = newFrame1->data[0] ^ newFrame1->data[1];
    newFrame2->data[2] = newFrame2->data[0] ^ newFrame2->data[1];
    newFrame3->data[2] = newFrame3->data[0] ^ newFrame3->data[1];
    newFrame4->data[2] = newFrame4->data[0] ^ newFrame4->data[1];

    queue(newFrame1);
    queue(newFrame2);
    queue(newFrame3);
    queue(newFrame4);
  }
}

/* End DCC Commands*/

/* Interrupt vectors*/
unsigned char flip = 0;                
unsigned char shortDelay = 0;           
unsigned char last_timer = TIMER_SHORT;

unsigned char state = PREAMBLE;
unsigned char preambleCount = 16;

unsigned char currentBit = 0x80;    // 1000 0000
uint8_t *nextbyte = NULL;
int currentMsg = 0;
int byteIndex = 0;

int nextMsg = 1;
int changeMsg = 0;


ISR(TIMER2_OVF_vect)
{
  uint8_t latency;
  //Toggle the signal every other interrupt
  if (flip)
  {
    digitalWrite(OUTPIN, HIGH);
    flip = 0;
    latency = TCNT2;
    TCNT2 = latency + last_timer;
  }
  else
  {
    digitalWrite(OUTPIN, LOW);
    flip = 1;
    switch (state)
    {
    case PREAMBLE:
      shortDelay = 1;
      preambleCount--;
      if (preambleCount == 0)
      {
        state = SEPARATOR;
        byteIndex = 0;
        nextbyte = &(getFront()->data[0]);
      }
      break;
    case SEPARATOR:
      shortDelay = 0;
      state = SENDBYTE;
      currentBit = 0x80;
      nextbyte = &(getFront()->data[byteIndex]);
      break;
    case SENDBYTE:
      if (*nextbyte & currentBit)
      {
        shortDelay = 1;
      }
      else
      {
        shortDelay = 0;
      }
      currentBit = currentBit >> 1;
      if (currentBit == 0)
      {
        byteIndex++;
        if (byteIndex >= getFront()->len)
        {
          state = PREAMBLE;
          preambleCount = 16;
          if (hasNext())
            dequeue();
        }
        else
        {
          state = SEPARATOR;
        }
      }
      break;
    }
    if (shortDelay)
    {
      latency = TCNT2;
      TCNT2 = latency + TIMER_SHORT;
      last_timer = TIMER_SHORT;
    }
    else
    {
      latency = TCNT2;
      TCNT2 = latency + TIMER_LONG;
      last_timer = TIMER_LONG;
    }
  }
}

