CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c client.c agent.c semaphore.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=projet

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o
	rm projet