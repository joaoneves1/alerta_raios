CC=gcc
SFILES  := c
OFILES  := o

SOURCE_PATH=./src
OBJS_PATH=./build
BIN_PATH  := ./bin/
INCLUDE_PATH=./include

SOURCES = term.c lib_crc.c lib_circular_buffer.c r_change.c
OBJECTS=$(SOURCES:.c=.o)

VPATH=$(SOURCE_PATH):$(OBJS_PATH)
NAME    := term
EXE     := $(BIN_PATH)$(NAME)
CFLAGS := -lm -O2 -g -Wall 
# -m32


.PHONY: all clean

all:$(EXE)

$(EXE): $(OBJECTS) 
	$(CC) $(CFLAGS)  $(OBJS_PATH)/term.o $(OBJS_PATH)/lib_crc.o $(OBJS_PATH)/lib_circular_buffer.o $(OBJS_PATH)/r_change.o -o $@
	cp -f $(EXE) .
	
term_to_disc: term_to_disc.o lib_crc.o lib_circular_buffer.o
	$(CC) $(CFLAGS)  $(OBJS_PATH)/term_to_disc.o $(OBJS_PATH)/lib_crc.o $(OBJS_PATH)/lib_circular_buffer.o -o term_to_disc
# 	cp -f term_to_disc .

%.o:%.c
	$(CC) -c $(CFLAGS) -I$(INCLUDE_PATH) $< -o $(OBJS_PATH)/$@

clean:
	rm -f $(OBJS_PATH)/*.o
	rm -f $(EXE)
