CFLAGS = -fPIC
LDFLAGS = --shared

SRC = matrix.c
OBJ = $(SRC:.c=.o)
TARGET = libmatrix.so

.PHONY : all

all : $(TARGET)

$(TARGET) : $(OBJ)
	gcc -o $@ $(LDFLAGS) $^
