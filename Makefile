CC = gcc
CFLAGS = -g
LDLIBS = -lm
TARGET1 = master
TARGET2 = bin_adder
TARGET3	= randInts
OBJS1 = master.o
OBJS2 = bin_adder.o
OBJS3 = randInts.o
.SUFFIXES: .c .o

all: master bin_adder randInts

$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) $(LDLIBS) -lpthread
$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2) $(LDLIBS) -lpthread
$(TARGET3): $(OBJS3)
	$(CC) -o $(TARGET3) $(OBJS3) $(LDLIBS) -lpthread
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	/bin/rm -f *.o $(TARGET1) $(TARGET2) $(TARGET3) numFile adder_log
