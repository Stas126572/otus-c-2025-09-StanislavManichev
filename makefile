TARGET = MyApp.out

.PHONY: all clean run


all: $(TARGET)

$(TARGET): main.c
	$(CC) $^ -o $@ -lglfw -lGL -lglut -lGLU

run: 
	@./$(TARGET)


clean: 
	$(RM) $(TARGET)
