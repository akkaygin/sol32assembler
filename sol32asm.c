#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <ncurses.h>

typedef uint32_t instruction;
typedef struct {
  uint32_t Capacity;
  uint32_t Length;
  uint32_t Cursor;
  char*Data;
} string;

typedef struct {
  uint32_t Capacity;
  uint32_t Length;
  uint32_t Cursor;
  instruction*Instructions;
  string*Assembly;
} program;

char*Disassemble(uint32_t Instruction);
uint32_t Assemble(string Assembly);

void ShiftStringLeft(string*String, uint32_t Amount) {
  if(String->Cursor-Amount >= 0) {
    memmove(&String->Data[String->Cursor-Amount], &String->Data[String->Cursor], (String->Length-String->Cursor)*sizeof(char));
    memset(&String->Data[String->Length-Amount], 0, Amount*sizeof(char));
    String->Length -= Amount;
  }
}

void ShiftStringRight(string*String, uint32_t Amount) {
  if(String->Length+Amount < String->Capacity) {
    memmove(&String->Data[String->Cursor+Amount], &String->Data[String->Cursor], (String->Length-String->Cursor)*sizeof(char));
    memset(&String->Data[String->Cursor], 0, Amount*sizeof(char));
    String->Length += Amount;
  }
}

void ResetString(string*String) {
  String->Length = 0;
  String->Cursor = 0;
  memset(String->Data, 0, String->Capacity*sizeof(char));
}

void InitializeString(string*String) {
  String->Capacity = 32;
  String->Length = 0;
  String->Cursor = 0;
  String->Data = (char*)malloc(String->Capacity*sizeof(char));
  memset(String->Data, 0, String->Capacity*sizeof(char));
}

void ShiftProgramLeft(program*Program, uint32_t Amount) {
  if(Program->Cursor-Amount >= 0) {
    memmove(&Program->Instructions[Program->Cursor-Amount], &Program->Instructions[Program->Cursor], (Program->Length-Program->Cursor)*sizeof(instruction));
    memset(&Program->Instructions[Program->Length-Amount], 0, Amount*sizeof(instruction));
    
    for(int i = 0; i < Amount; i++) {
      free(Program->Assembly[Program->Cursor-Amount+i].Data);
    }
    memmove(&Program->Assembly[Program->Cursor-Amount], &Program->Assembly[Program->Cursor], (Program->Length-Program->Cursor)*sizeof(string));
    for(int i = 0; i < Amount; i++) {
      //ResetString(&Program->Assembly[Program->Length-Amount+i]); // this resets the last valid item, also does nothing
    }
    
    Program->Length -= Amount;
  }
}

void ShiftProgramRight(program*Program, uint32_t Amount) {
  if(Program->Length+Amount < Program->Capacity) {
    memmove(&Program->Instructions[Program->Cursor+Amount], &Program->Instructions[Program->Cursor], (Program->Length-Program->Cursor)*sizeof(instruction));
    memset(&Program->Instructions[Program->Cursor], 0, Amount*sizeof(instruction));

    memmove(&Program->Assembly[Program->Cursor+Amount], &Program->Assembly[Program->Cursor], (Program->Length-Program->Cursor)*sizeof(string));
    for(int i = 0; i < Amount; i++) {
      InitializeString(&Program->Assembly[Program->Cursor+i]);
    }

    Program->Length += Amount;
  }
}

void ExtendInstructions(program*Program) {
  Program->Capacity *= 2;

  instruction*NewInstructionBuffer = (instruction*)malloc(Program->Capacity*sizeof(instruction));
  memmove(NewInstructionBuffer, Program->Instructions, Program->Capacity/2*sizeof(instruction));

  string*NewAssemblyBuffer = (string*)malloc(Program->Capacity*sizeof(string));
  memmove(NewAssemblyBuffer, Program->Assembly, Program->Capacity/2*sizeof(string));

  free(Program->Instructions);
  free(Program->Assembly);
  
  Program->Instructions = NewInstructionBuffer;
  Program->Assembly = NewAssemblyBuffer;

  for(size_t i = 0; i < Program->Capacity/2; i++) {
    InitializeString(&Program->Assembly[Program->Capacity/2+i]);
  }
}

void InitializeProgram(program*Program) {
  Program->Capacity = 16;
  Program->Length = 1;
  Program->Cursor = 0;
  Program->Instructions = (instruction*)malloc(Program->Capacity*sizeof(instruction));
  Program->Assembly = (string*)malloc(Program->Capacity*sizeof(string));

  memset(Program->Instructions, 0, Program->Capacity*sizeof(instruction));
  for(size_t i = 0; i < Program->Capacity; i++) {
    InitializeString(&Program->Assembly[i]);
  }
}

void TerminateNCurses() {
  endwin();
}

int SY, SX;
void InitializeNCurses() {
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);

  atexit(TerminateNCurses);

  getmaxyx(stdscr, SY, SX);
}

void Render(program Program) {
  move(0, 0); clrtobot();
  int LinesToRender;
  if(Program.Length > SY) {
    LinesToRender = SY-1;
  } else {
    LinesToRender = Program.Length;
  }

  // TODO: Scrolling :)
  for (int i = 0; i < LinesToRender; i++) {
    char*Disassembly = Disassemble(Program.Instructions[i]);

    if(i == Program.Cursor) {
      attron(A_REVERSE);
      mvprintw(i, 0, "%04d:%-02d| %32s | %-32s | %08X", i+1, Program.Assembly[i].Cursor+1,
        Disassembly, Program.Assembly[i].Data, Program.Instructions[i]);
      attroff(A_REVERSE);
      
      if(Program.Assembly[i].Data[Program.Assembly[i].Cursor] != 0) {
        mvaddch(i, Program.Assembly[i].Cursor+44, Program.Assembly[i].Data[Program.Assembly[i].Cursor]);
      } else {
        mvaddch(i, Program.Assembly[i].Cursor+44, ' ');
      }
    } else {
      mvprintw(i, 0, "%04d:%-02d| %32s | %-32s | %08X", i+1, Program.Assembly[i].Cursor+1,
        Disassembly, Program.Assembly[i].Data, Program.Instructions[i]);
      clrtoeol();
    }
    free(Disassembly);
  }
}

void SaveProgram(program Program)
{

}

int main(int argc, char**argv) {
  InitializeNCurses();
  
  program Program;
  InitializeProgram(&Program);

  int Key;
  do { inputhandler:
    Render(Program); // renders constantly :(
    Key = getch();
    switch(Key) {
      case -1:
        goto inputhandler;
      break;

      case KEY_F(2):
        SaveProgram(Program);
      break;

      case '\r':
      case '\n':
        if(!(Program.Length < Program.Capacity-1)) {
          ExtendInstructions(&Program);
        }

        if(Program.Cursor < Program.Length) {
          Program.Cursor++;
          ShiftProgramRight(&Program, 1);
        } else if(Program.Length < Program.Capacity) {
          Program.Cursor++;
        }
      break;

      case KEY_BACKSPACE:
        if(Program.Assembly[Program.Cursor].Cursor > 0) {
          ShiftStringLeft(&Program.Assembly[Program.Cursor], 1);
          Program.Assembly[Program.Cursor].Cursor--;
        } else if(Program.Cursor > 0) {
          if(Program.Cursor < Program.Length-1) {
            Program.Cursor++;
            ShiftProgramLeft(&Program, 1);
            Program.Cursor--;
            Program.Cursor--;
          } else {
            Program.Instructions[Program.Cursor] = 0;
            ResetString(&Program.Assembly[Program.Cursor]);
            Program.Length--;
            Program.Cursor--;
          }
        }
      break;

      case KEY_DC:
        if(Program.Assembly[Program.Cursor].Cursor < Program.Assembly[Program.Cursor].Length) {
          Program.Assembly[Program.Cursor].Cursor++;
          ShiftStringLeft(&Program.Assembly[Program.Cursor], 1);
          Program.Assembly[Program.Cursor].Cursor--;
        } else if(Program.Cursor < Program.Length-1) {
          Program.Cursor++;
          Program.Cursor++;
          ShiftProgramLeft(&Program, 1);
          Program.Cursor--;
          Program.Cursor--;
        }
      break;

      case KEY_UP:
        if(Program.Cursor > 0) {
          Program.Cursor--;
        } else {
          Program.Assembly[Program.Cursor].Cursor = 0;
        }
      break;

      case KEY_DOWN:
        if(Program.Cursor < Program.Length-1) {
          Program.Cursor++;
        } else {
          Program.Assembly[Program.Cursor].Cursor = Program.Assembly[Program.Cursor].Length;
        }
      break;

      case KEY_LEFT:
        if(Program.Assembly[Program.Cursor].Cursor > 0) {
          Program.Assembly[Program.Cursor].Cursor--;
        } else if(Program.Cursor > 0) {
          Program.Cursor--;
        }
      break;

      case KEY_RIGHT:
        if(Program.Assembly[Program.Cursor].Cursor < Program.Assembly[Program.Cursor].Length) {
          Program.Assembly[Program.Cursor].Cursor++;
        } else if(Program.Cursor < Program.Length-1) {
          Program.Cursor++;
        }
      break;

      case KEY_HOME:
        Program.Assembly[Program.Cursor].Cursor = 0;
      break;

      case KEY_END:
        Program.Assembly[Program.Cursor].Cursor = Program.Assembly[Program.Cursor].Length;
      break;

      case KEY_NPAGE:
      break;

      case KEY_PPAGE:
      break;

      default:
        if(Program.Assembly[Program.Cursor].Cursor < Program.Assembly[Program.Cursor].Length) {
          ShiftStringRight(&Program.Assembly[Program.Cursor], 1);
          Program.Assembly[Program.Cursor].Data[Program.Assembly[Program.Cursor].Cursor] = Key;
          Program.Assembly[Program.Cursor].Cursor++;
        } else if(Program.Assembly[Program.Cursor].Length < Program.Assembly[Program.Cursor].Capacity) {
          Program.Assembly[Program.Cursor].Data[Program.Assembly[Program.Cursor].Cursor] = Key;
          Program.Assembly[Program.Cursor].Cursor++;
          Program.Assembly[Program.Cursor].Length++;
        }

        Program.Instructions[Program.Cursor] = Assemble(Program.Assembly[Program.Cursor]);
      break;
    }
  } while(Key != KEY_F(1));

  return 0;
}