#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  uint32_t Capacity;
  uint32_t Length;
  uint32_t Cursor;
  char*Data;
} string;

/*    Assembler    */
char Opcodes[][5] = {
  "add", "sub", "and", "or", "xor", "lsl", "lsr", "asr",
  "lw", "lh", "lb", "INV", "sw", "sh", "sb", "INV",
  "jeq", "jne", "jslt", "jlt", "jsle", "jle", "jsge", "jge",
  "swm", "stu", "uts", "hlt"
};

uint8_t Encoded[] = {
  0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
  0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71,
  0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72,
  0x03, 0x13, 0x23, 0x33
};

uint32_t AssembleALInstruction(uint32_t Instruction)
{
  uint16_t RD, RS1, Extend, RS2, Offset;
  
  Instruction |= (RD&15) << 8;
  Instruction |= (RS1&15) << 12;
  Instruction |= ((0-Extend)&1) << 7;

  if(Extend) {
    Instruction |= (RS2&15) << 16;
  } else {
    Instruction |= (RS2&15) << 16;
    Instruction |= Offset << 20;
  }

  return Instruction;
}

uint32_t AssembleLSInstruction(uint32_t Instruction)
{
  uint16_t RDS, RS1, Extend, RS2, Offset;

  if(Instruction < 12) {
    Instruction |= (RDS&15) << 8;
  } else {
    Instruction |= (RDS&15) << 8;
  }

  Instruction |= (RS1&15) << 12;
  Instruction |= ((0-Extend)&1) << 7;

  uint8_t AddrShift = 2-((Instruction&0x30)>>4);
  if(Extend) {
    Instruction |= (RS2&15) << 16;
  } else {
    Instruction |= (RS2&15) << 16;
    Instruction |= Offset << 20;
  }

  return Instruction;
}

uint32_t AssembleCJInstruction(uint32_t Instruction)
{
  uint16_t RDS, RS1, Extend, RS2, Offset;

  Instruction |= (RS1&15) << 12;
  Instruction |= (RDS&15) << 8;
  Instruction |= ((0-Extend)&1) << 7;

  if(Extend) {
    Instruction |= RS2 << 16;
  } else {
    Instruction |= (RS2&15) << 16;
    Instruction |= Offset << 20;
  }

  return Instruction;
}

uint32_t AssembleCCInstruction(uint32_t Instruction)
{
  if(Instruction == 19 || Instruction == 35) {
    uint16_t RD, RS1;
    Instruction |= (RD&15) << 8;
    Instruction |= (RS1&15) << 12;
  }

  return Instruction;
}

uint32_t GetInteger(string*Assembly) {
  uint32_t Integer = 0;
  while(Assembly->Cursor < Assembly->Length && Assembly->Data[Assembly->Cursor] == ' ') { Assembly->Cursor++; }
  while(Assembly->Cursor < Assembly->Length && isdigit(Assembly->Data[Assembly->Cursor])) {
    Integer *= 10;
    Integer += Assembly->Data[Assembly->Cursor] - '0';
    Assembly->Cursor++;
  }
  return Integer;
}

uint32_t GetInstruction(string*Assembly) {
  for(int i = 0; i < 28; i++) {
    if(strncmp(Assembly->Data, Opcodes[i], strlen(Opcodes[i])) == 0) {
      Assembly->Cursor += strlen(Opcodes[i]);
      return Encoded[i];
    }
  }

  return 0;
}

uint32_t Assemble(string Assembly) {
  uint32_t Instruction = 0;
  
  Assembly.Cursor = 0;
  if(Assembly.Length == 0) {
    return 0;
  }

  Instruction |= GetInstruction(&Assembly);
  uint32_t RD = GetInteger(&Assembly);
  uint32_t RS1 = GetInteger(&Assembly);
  uint32_t RS2 = GetInteger(&Assembly);
  uint32_t Offset = GetInteger(&Assembly);

  Instruction |= (RD&15) << 8;
  Instruction |= (RS1&15) << 12;
  if(RS2 != 0) {
    Instruction |= (RS2&15) << 16;
    Instruction |= Offset << 20;
  } else {
    Instruction |= 1 << 7;
    Instruction |= Offset << 16;
  }

  return Instruction;
}