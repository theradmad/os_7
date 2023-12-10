TASK1_SRC	:= list.c util.c mmu.c 
EXE		:= mmu

all: $(EXE)

mmu: $(TASK1_SRC)
	gcc -Wall  -std=c99 -std=gnu99 -Werror -pedantic -g $^ -o $@

clean:
	rm -f $(EXE)