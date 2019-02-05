#define TIMER_SHORT 0x8D //58 uS
#define TIMER_LONG 0x1B //116 uS

#define OUTPIN 11

#define FUNCR_LIGHT 0x10
#define FUNCR_SOUND 0x01
#define FUNCR_HORN1 0x02
#define FUNCR_HORN2 0x04
#define FUNCR_BELL 0x08
#define FUNCR_BASE 0x80

#define PREAMBLE 0
#define SEPARATOR 1
#define SENDBYTE 2

#define LIGHT_RED 0
#define LIGHT_GREEN 1

#define SWITCH_STRAIGHT 1
#define SWITCH_TURN 0

#define CROSS_SWITCH_STRAIGHT 1
#define CROSS_SWITCH_CROSS 0

#define TRIPPLE_SWITCH_STRAIGHT 2
#define TRIPPLE_SWITCH_RIGHT 3
#define TRIPPLE_SWITCH_LEFT 4


#define BLOCK_1 2
#define BLOCK_2 3
#define BLOCK_3 4
#define BLOCK_4 5
#define BLOCK_5 6
#define BLOCK_6 7
#define BLOCK_8 8
#define BLOCK_9 9
#define BLOCK_10 10
#define BLOCK_11 12
#define BLOCK_12 13
#define BLOCK_13 15
#define BLOCK_14 16
#define BLOCK_15 5



/*
Command Struct
*/
struct Frame
{
  uint8_t data[4];
  uint8_t len;
};
