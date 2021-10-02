CFLAGS = -std=gnu99 -w -m32
LIBS = -lpthread -pthread
SOURCES1 = app1.c
SOURCES2 = app2.c
SOURCES3 = app3.c
SOURCES4 = app4.c
OUT1 = app1
OUT2 = app2
OUT3 = app3
OUT4 = app4

default:
	gcc $(CFLAGS) $(LIBS) -o $(OUT1) $(SOURCES1)
	gcc $(CFLAGS) $(LIBS) -o $(OUT2) $(SOURCES2)
	gcc $(CFLAGS) $(LIBS) -o $(OUT3) $(SOURCES3)
debug:
	gcc -g $(CFLAGS) $(LIBS) -o $(OUT1) $(SOURCES1)
	gcc -g $(CFLAGS) $(LIBS) -o $(OUT2) $(SOURCES2)
	gcc -g $(CFLAGS) $(LIBS) -o $(OUT3) $(SOURCES3)
	gcc -g $(CFLAGS) $(LIBS) -o $(OUT4) $(SOURCES4)
all:
	gcc $(CFLAGS) $(LIBS) -o $(OUT1) $(SOURCES1)
	gcc $(CFLAGS) $(LIBS) -o $(OUT2) $(SOURCES2)
	gcc $(CFLAGS) $(LIBS) -o $(OUT3) $(SOURCES3)
clean:
	rm $(OUT1) $(OUT2) $(OUT3)
