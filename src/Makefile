CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c18 -pedantic
LDFLAGS = -lm -lgsl -lgslcblas
SOURCES = main.c distributions.c

all: rebuild

rebuild: clean build

clean:
	rm -rf *.o

build:
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o spreadings.o

clean_plots:
	rm -rf data/plots/*.png

clean_data:
	rm -rf data/*.txt