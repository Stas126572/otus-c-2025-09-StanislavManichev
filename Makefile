TARGET = 2048_game.out

$(TARGET):
	gcc main.c -o $@ $$(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf)
clean:
	@$(RM) $(TARGET)
