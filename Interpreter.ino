void runCommands()
{
  uint16_t *command_ptr = commands;
  uint16_t cCommand;
  uint16_t counter = 0;
  uint16_t arg1 = 0;
  uint16_t arg2 = 0;
  
    while (counter < COMMAND_LENGTH)
    {
      cCommand = *command_ptr;
      //NO ARGUMENTS
      if (cCommand == INTR_IDLE)
      {
        writeIdle();
      }
      else //ONE ARGUMENT
      {
        command_ptr++;
        counter++;
        arg1 = *command_ptr;
        if (cCommand == INTR_DELAY)
        {
          wait(arg1);
        }
        else if(cCommand == INTR_BLOCK)
        {
          waitForBlock(arg1);
        }
        else //TWO ARGUMENTS
        {
          command_ptr++;
          counter++;
          arg2 = *command_ptr;
          
          if (cCommand == INTR_SPEED)
          {
            writeSpeed(arg1, arg2);
          }
          else if(cCommand == INTR_FUNC)
          {
            writeFuncRegister(arg1, arg2);
            writeFuncRegister(arg1, arg2);
            writeFuncRegister(arg1, arg2);
            writeIdle();
          }
          else if(cCommand == INTR_ACC)
          {
            writeAcc(arg1, arg2);
            writeAcc(arg1, arg2);
            writeAcc(arg1, arg2);
            writeIdle();
          }
        } 
      }
      command_ptr++;
      counter++;
    }
}

void interpreterSetup()
{
  pinMode(BLOCK_1, INPUT_PULLUP);
  pinMode(BLOCK_2, INPUT_PULLUP);
  pinMode(BLOCK_3, INPUT_PULLUP);
  pinMode(BLOCK_4, INPUT_PULLUP);
  pinMode(BLOCK_5, INPUT_PULLUP);
  pinMode(BLOCK_6, INPUT_PULLUP);
  pinMode(BLOCK_8, INPUT_PULLUP);
  pinMode(BLOCK_9, INPUT_PULLUP);
  pinMode(BLOCK_10, INPUT_PULLUP);
  pinMode(BLOCK_11, INPUT_PULLUP);
  pinMode(BLOCK_12, INPUT_PULLUP);
  pinMode(BLOCK_13, INPUT_PULLUP);
  pinMode(BLOCK_14, INPUT_PULLUP);
  pinMode(BLOCK_15, INPUT_PULLUP);
}

void wait(uint16_t delayPeriod)
{
  unsigned long start = millis();
  while (millis() - start < delayPeriod)
  {
    ;
  }
}

void waitForBlock(uint8_t block)
{
  Serial.print("waiting for block: ");
  Serial.println(block);
  Serial.println(digitalRead(block));
  uint8_t kill = digitalRead(block);
  while ( kill != LOW)
  {
    kill = digitalRead(block);
  }
  Serial.print("block done");
}

