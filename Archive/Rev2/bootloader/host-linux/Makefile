TRG = fboot

SRC = $(TRG).c com.c
HD  = com.h protocol.h
OBJ = $(SRC:.c=.o)

CCFLAGS = -Wall -g -O3

all : $(TRG)

%.o : %.c $(HD)
	gcc $(CCFLAGS) -c $< -o $@

$(TRG) : $(OBJ)
	gcc $(CCFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ)
	rm -f $(TRG)
