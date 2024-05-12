CFLAGS = -fPIC -I.
LDFLAGS = --shared
#TEST_LDFLAGS = -Wl,--wrap=malloc

SRC = matrix.c
OBJ = $(SRC:.c=.o)
TARGET = libmatrix.so
TEST_SRC = test_matrix.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = test_matrix


.PHONY : all test

all : $(TARGET)
test : $(TEST_TARGET)

$(TARGET) : $(OBJ)
	gcc -o $@ $(LDFLAGS) $^

$(TEST_TARGET) : $(TEST_OBJ)
	gcc -o $@ $(TEST_LDFLAGS) $^
