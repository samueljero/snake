SRC=gc.cc
CPP=g++
CPPFLAGS=$(CFLAGS) -std=c++0x -g

.PHONY: clean

all:
	make gc

clean:
	rm -f $(BINARIES) *.o
	
gc: $(SRC)
	$(CPP) $(CPPFLAGS) $(SRC) -o gc -lpthread
