ifdef LLVM
    CC = /usr/bin/gcc
else
    CC = /usr/local/bin/gcc
endif

CFLAGS = -g -fPIC -I. -I/usr/local/include -msse4.1 -mavx2 -Wno-int-conversion

ifdef RELEASE
    CFLAGS := $(CFLAGS) -O3
endif

LDFLAGS := $(LDFLAGS) -g -L/usr/local/lib

ifdef OPENMP
    CFLAGS := $(CFLAGS) -Xpreprocessor -fopenmp
    LIBS := $(LIBS) -lomp
endif

#TEST_LDFLAGS = -Wl,--wrap=malloc
TEST_LDFLAGS = -g

SRC = src/mx_storage.c \
      src/v8si_storage.c \
      src/v8si_operation.c \
      src/i32_matrix.c \
      src/v8sf_storage.c \
      src/v8sf_operation.c
OBJ = $(SRC:.c=.o)
TARGET = bin/libmatrix.dylib

#TEST_SRC = test/matrix.c
#TEST_OBJ = $(TEST_SRC:.c=.o)
#TEST_TARGET = test/matrix

TEST_STOR_SRC = test/v8si_storage.c \
                test/v8sf_storage.c \
                src/mx_storage.c \
                src/v8si_storage.c \
                src/v8sf_storage.c
TEST_STOR_OBJ = $(TEST_STOR_SRC:.c=.o)
TEST_STOR_TARGET = test/storage.out

TEST_OPER_SRC = test/v8si_operation.c \
                src/mx_storage.c \
                src/v8si_storage.c
TEST_OPER_OBJ = $(TEST_OPER_SRC:.c=.o)
TEST_OPER_TARGET = test/operation

BM_MATRIX_MULTIPLY_SRC = benchmark/matrix_multiply.c $(SRC)
BM_MATRIX_MULTIPLY_OBJ = $(BM_MATRIX_MULTIPLY_SRC:.c=.o)
BM_MATRIX_MULTIPLY = benchmark/matrix_multiply

.PHONY : all test clean

all : $(TARGET)
test : $(TEST_STOR_TARGET) $(TEST_OPER_TARGET)
benchmark : $(BM_MATRIX_MULTIPLY)

clean :
	rm -rf $(TARGET) $(TEST_STOR_TARGET) $(TEST_OPER_TARGET) $(BM_MATRIX_MULTIPLY) $(OBJ) $(TEST_OBJ) $(TEST_STOR_OBJ) $(TEST_OPER_OBJ) $(BM_MATRIX_MULTIPLY_OBJ)

$(TARGET) : $(OBJ)
	gcc -o $@ --shared $^ $(LDFLAGS) $(LIBS)

$(TEST_OBJ) : $(SRC)

#$(TEST_TARGET) : $(TEST_OBJ)
#	gcc -o $@ $^ $(LDFLAGS) $(LIBS)

$(TEST_STOR_TARGET) : $(TEST_STOR_OBJ) src/mx_storage.c src/v8si_storage.c
	gcc -o $@ $(TEST_STOR_OBJ) $(LDFLAGS) $(LIBS) -lcriterion

$(TEST_OPER_TARGET) : $(TEST_OPER_OBJ) src/mx_storage.c src/v8si_storage.c src/v8si_operation.c
	gcc -o $@ $(TEST_OPER_OBJ) $(LDFLAGS) $(LIBS) -lcriterion

$(BM_MATRIX_MULTIPLY) : $(BM_MATRIX_MULTIPLY_OBJ)
	gcc -o $@ $^ $(LDFLAGS) $(LIBS)
