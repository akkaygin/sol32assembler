#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t GetX(uint32_t Instruction)
{
  return (Instruction >> 7)&1;
}

uint32_t SignExtendXEmbedded(uint32_t Instruction)
{
  return Instruction>>31? (-1<<16)|(Instruction>>16) : (Instruction>>16);
}

uint32_t SignExtendOffset(uint32_t Instruction)
{
  return Instruction>>31? (-1<<12)|(Instruction>>20) : (Instruction>>20);
}

/*    Disassembler    */
// see: https://twitter.com/nikitonsky/status/1615754231687921666
void DisassembleALInstruction(uint32_t Instruction, char*Buffer)
{
  if(Instruction == 0 || Instruction == 0x80) { sprintf(Buffer, "noop"); return Buffer; }

  switch((Instruction>>4)&7)
  {
  case 0:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X + R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 1:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X - %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X - R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 2:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X & %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X & R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 3:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X | %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X | R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 4:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X ^ %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X ^ R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;
  
  case 5:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X << %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X << R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 6:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X >> %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X >> R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 7:
    if(GetX(Instruction)) {
      sprintf(Buffer, "R%X = R%X a>> %X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "R%X = R%X a>> R%X + %X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;
  }
}

void DisassembleLSInstruction(uint32_t Instruction, char*Buffer)
{
  uint32_t Offset = GetX(Instruction)? SignExtendXEmbedded(Instruction) : SignExtendOffset(Instruction);

  switch((Instruction>>4)&7)
  {
  case 0:
    if(GetX(Instruction)) {
      sprintf(Buffer, "load w to R%X from BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "load w to R%X from BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 1:
    if(GetX(Instruction)) {
      sprintf(Buffer, "load hw to R%X from BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "load hw to R%X from BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 2:
    if(GetX(Instruction)) {
      sprintf(Buffer, "load b to R%X from BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "load b to R%X from BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 4:
    if(GetX(Instruction)) {
      sprintf(Buffer, "store w from R%X to BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "store w from R%X to BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 5:
    if(GetX(Instruction)) {
      sprintf(Buffer, "store hw from R%X to BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "store hw from R%X to BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  case 6:
    if(GetX(Instruction)) {
      sprintf(Buffer, "store b from R%X to BR%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
    } else {
      sprintf(Buffer, "store b from R%X to BR%X+R%X+%X", (Instruction>>8)&15, (Instruction>>12)&15, (Instruction>>16)&15, SignExtendOffset(Instruction));
    }
  break;

  default:
    sprintf(Buffer, "invalid instruction");
  break;
  }
}

void DisassembleCJInstruction(uint32_t Instruction, char*Buffer)
{
  uint32_t Offset = GetX(Instruction)? SignExtendXEmbedded(Instruction) : SignExtendOffset(Instruction);
  
  switch((Instruction>>4)&7)
  {
  case 0:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X == R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X == R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 1:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X != R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X != R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;
  
  case 2:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X s< R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X s< R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 3:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X < R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X < R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 4:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X s<= R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X s<= R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 5:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X <= R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X <= R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 6:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X s>= R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X s>= R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;

  case 7:
    if(GetX(Instruction)) {
      sprintf(Buffer, "goto R15+%X if R%X >= R%X", Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    } else {
      sprintf(Buffer, "goto R15+R%X+%X if R%X >= R%X", (Instruction>>16)&15, Offset, (Instruction>>12)&15, (Instruction>>8)&15);
    }
  break;
  }
}

void DisassembleCCInstruction(uint32_t Instruction, char*Buffer)
{
  switch((Instruction>>4)&27)
  {
  case 0:
    sprintf(Buffer, "handoff\n");
  break;

  case 1:
    sprintf(Buffer, "SR%X->UR%X\n", (Instruction>>16)&15, (Instruction>>8)&15, (Instruction>>12)&15, SignExtendXEmbedded(Instruction));
  break;

  case 2:
    sprintf(Buffer, "UR%X->SR%X\n", (Instruction>>16)&15);
  break;

  case 3:
    sprintf(Buffer, "Halt Core\n");
  break;
  }
}

void Disassemble(uint32_t Instruction, char*Buffer)
{
  switch (Instruction&3)
  {
  case 0:
    DisassembleALInstruction(Instruction, Buffer);
  break;

  case 1:
    DisassembleLSInstruction(Instruction, Buffer);
  break;

  case 2:
    DisassembleCJInstruction(Instruction, Buffer);
  break;

  case 3:
    DisassembleCCInstruction(Instruction, Buffer);
  break;
  }
}