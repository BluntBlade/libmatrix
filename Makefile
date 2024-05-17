CC = gcc

CFLAGS = -g -O3 -fPIC -I. -I/usr/local/include -msse4.1 -Wno-int-conversion

ifdef OPENMP
    CFLAGS := $(CFLAGS) -Xpreprocessor -fopenmp
    LDFLAGS := $(LDFLAGS) -L/usr/local/lib
    LIBS := $(LIBS) -lomp
endif

#TEST_LDFLAGS = -Wl,--wrap=malloc
TEST_LDFLAGS = -g

SRC = matrix.c
OBJ = $(SRC:.c=.o)
TARGET = libmatrix.so

TEST_SRC = test_matrix.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = test_matrix

BENCHMARK_MATRIX_MULTIPLY_SRC = benchmark_matrix_multiply.c
BENCHMARK_MATRIX_MULTIPLY_OBJ = $(BENCHMARK_MATRIX_MULTIPLY_SRC:.c=.o)  $(OBJ)
BENCHMARK_MATRIX_MULTIPLY = benchmark_matrix_multiply

.PHONY : all test

all : $(TARGET)
test : $(TEST_TARGET)
benchmark : $(BENCHMARK_MATRIX_MULTIPLY)

$(TARGET) : $(OBJ)
	gcc -o $@ --shared $^ $(LDFLAGS) $(LIBS)

$(TEST_OBJ) : $(SRC)

$(TEST_TARGET) : $(TEST_OBJ)
	gcc -o $@ $(TEST_LDFLAGS) $^ $(LDFLAGS) $(LIBS)

$(BENCHMARK_MATRIX_MULTIPLY) : $(BENCHMARK_MATRIX_MULTIPLY_OBJ)
	gcc -o $@ $^ $(LDFLAGS) $(LIBS)
