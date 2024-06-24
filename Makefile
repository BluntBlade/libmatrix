CC = gcc

CFLAGS = -g -fPIC -I. -I/usr/local/include -msse4.1 -mavx2 -Wno-int-conversion

ifndef DEBUG
    CFLAGS := $(CFLAGS) -O3
endif

LDFLAGS := $(LDFLAGS) -L/usr/local/lib

ifdef OPENMP
    CFLAGS := $(CFLAGS) -Xpreprocessor -fopenmp
    LIBS := $(LIBS) -lomp
endif

#TEST_LDFLAGS = -Wl,--wrap=malloc
TEST_LDFLAGS = -g

SRC = matrix.c mx_storage.c mx_operation.c mx_types.c
OBJ = $(SRC:.c=.o)
TARGET = libmatrix.so

TEST_SRC = test_matrix.c mx_types.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = test_matrix

TEST_STOR_SRC = test_storage.c mx_types.c
TEST_STOR_OBJ = $(TEST_STOR_SRC:.c=.o)
TEST_STOR_TARGET = test_storage

BENCHMARK_MATRIX_MULTIPLY_SRC = benchmark_matrix_multiply.c
BENCHMARK_MATRIX_MULTIPLY_OBJ = $(BENCHMARK_MATRIX_MULTIPLY_SRC:.c=.o)  $(OBJ)
BENCHMARK_MATRIX_MULTIPLY = benchmark_matrix_multiply

.PHONY : all test clean

all : $(TARGET)
test : $(TEST_TARGET) $(TEST_STOR_TARGET)
benchmark : $(BENCHMARK_MATRIX_MULTIPLY)

clean :
	rm -rf *.o $(TARGET) $(TEST_TARGET) $(BENCHMARK_MATRIX_MULTIPLY)

$(TARGET) : $(OBJ)
	gcc -o $@ --shared $^ $(LDFLAGS) $(LIBS)

$(TEST_OBJ) : $(SRC)

$(TEST_TARGET) : $(TEST_OBJ)
	gcc -o $@ $(TEST_LDFLAGS) $^ $(LDFLAGS) $(LIBS)

$(TEST_STOR_TARGET) : $(TEST_STOR_OBJ) mx_storage.c
	gcc -o $@ $(TEST_LDFLAGS) $(TEST_STOR_OBJ) $(LDFLAGS) $(LIBS) -lcriterion

$(BENCHMARK_MATRIX_MULTIPLY) : $(BENCHMARK_MATRIX_MULTIPLY_OBJ)
	gcc -o $@ $^ $(LDFLAGS) $(LIBS)
