CC:=gcc
ASM:=gcc

INCLUDES:=-I. -I../includes
CFLAGS:=-fprofile-arcs -ftest-coverage

DIR_SRC:=../src
DIR_TEST:=../test
DIR_OBJ:=.

SRC_C:=$(wildcard $(DIR_SRC)/*.c)
SRC_C+=$(wildcard $(DIR_TEST)/*.c)
OBJ_O:=$(patsubst %.c,%.o,$(SRC_C))

OUTFILE:=start

.PHONY:all clean
all:$(OBJ_O)
	$(CC) $(CFLAGS) -o $(OUTFILE).out -Wl,-Map=$(OUTFILE).map ./*.o
	
%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(DIR_OBJ)/$(notdir $@)
%.o:%.s
	$(ASM) $(ASFLAG) -c $< -o $(DIR_OBJ)/$(notdir $@)

asm:
	$(OBJDUMP) -D $(OUTFILE).elf > $(OUTFILE).asm

clean:
	rm -f *.o *.out
	rm -f *.elf
	rm -f *.map 
	rm -f *.asm
	rm -rf *.gcda *.gcno *.gcov *.info	
	rm -rf ./gcovdir
