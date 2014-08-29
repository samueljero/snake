SRC=gc.cc
CPP=g++
CPPFLAGS=$(CFLAGS) -g

.PHONY: clean

all:
	make gc

clean:
	rm -f $(BINARIES) *.o
	
gc: $(SRC)
	$(CPP) $(CPPFLAGS) $(SRC) -o gc -lpthread
