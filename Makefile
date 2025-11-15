# Makefile

#recompile: clean all

all: weather_reporter

weather_reporter: main.c cJSON.c cJSON.h
	$(CC) $(CFLAGS)  -Wall -Wextra -pedantic -std=c11 $^ -o $@ `pkg-config --cflags --libs libcurl`
clean:
	$(RM) weather_reporter

.PHONY: all clean #recompile
