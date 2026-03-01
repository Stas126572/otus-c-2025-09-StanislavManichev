#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CHECK(mas, a, b, c) ((mas)[a] == (mas)[b]) && ((mas)[b] == (mas)[c])

typedef enum states {
  CHOOSE_FIRST_PLAYER_LETTER,
  LETTER_HAVE_CHOSEN,
  WAITING_FIRST_PLAYER_STEP,
  FIRST_PLAYER_STEP,
  WAITING_SECOND_PLAYER_STEP,
  SECOND_PLAYER_STEP,
  FIRST_PLAYER_WIN,
  SECOND_PLAYER_WIN,
  NICHYA
} states;

static const int MS_PER_FRAME = 30;
char num[200];
states state = CHOOSE_FIRST_PLAYER_LETTER;
SDL_Color white = {255, 255, 255, 255};
TTF_Font *font;
#define WIN_HEIGH 640
#define WIN_WIDTH 480
#define CHECK_ELEMENTS_NICHYA(a, b, c)                                         \
  ((symbols[a] != 0) && (symbols[b] != 0) && (symbols[c] != 0))
#define CHECK_ELEMENTS_WIN(a, b, c)                                            \
  ((symbols[a] == symbols[b]) && (symbols[b] == symbols[c]) &&                 \
   (symbols[a] != 0) && (symbols[b] != 0) && (symbols[c] != 0))
#define CHECK_GORIZONTALY_WIN                                                  \
  (CHECK_ELEMENTS_WIN(0, 1, 2) || CHECK_ELEMENTS_WIN(3, 4, 5) ||               \
   CHECK_ELEMENTS_WIN(6, 7, 8))
#define CHECK_VERTICALY_WIN                                                    \
  (CHECK_ELEMENTS_WIN(0, 3, 6) || CHECK_ELEMENTS_WIN(1, 4, 7) ||               \
   CHECK_ELEMENTS_WIN(2, 5, 8))
#define CHECK_DIAGONAL_WIN                                                     \
  (CHECK_ELEMENTS_WIN(0, 4, 8) || CHECK_ELEMENTS_WIN(2, 4, 6))
#define CHECK_NICHYA                                                           \
  (CHECK_ELEMENTS_NICHYA(0, 1, 2) && CHECK_ELEMENTS_NICHYA(3, 4, 5) &&         \
   CHECK_ELEMENTS_NICHYA(6, 7, 8))
static const int WIN_SIZE = MIN(WIN_HEIGH, WIN_WIDTH);
static const int FRAME_SIZE = WIN_SIZE / 3;
int cell_index = -1;
uint8_t symbols[9] = {0};
SDL_Renderer *ren;
SDL_Texture *tex_first_player_symbol;
SDL_Texture *tex_second_player_symbol;
SDL_Texture *tex_choose_first_player_letter;
SDL_Texture *tex_first_player_win;
SDL_Texture *tex_second_player_win;
SDL_Texture *tex_nichya;
double getCurrentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double)(tv.tv_sec) * 1000.0 + (double)(tv.tv_usec) / 1000.0;
}


void check_is_win_or_nichya() {

  if (CHECK_GORIZONTALY_WIN | CHECK_VERTICALY_WIN | CHECK_DIAGONAL_WIN) {
    switch (state) {
    case WAITING_SECOND_PLAYER_STEP:
      state = FIRST_PLAYER_WIN;
      break;
    case WAITING_FIRST_PLAYER_STEP:
      state = SECOND_PLAYER_WIN;
      break;
    }
  }
  if (CHECK_NICHYA) {
    state = NICHYA;
  }
}
void render() {

  SDL_Rect rect;
  switch (state) {
  case CHOOSE_FIRST_PLAYER_LETTER:
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    rect = (SDL_Rect){0, WIN_SIZE / 2, WIN_SIZE - 10, 50};
    SDL_RenderCopy(ren, tex_choose_first_player_letter, NULL, &rect);
    break;
  case LETTER_HAVE_CHOSEN:
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    state = WAITING_FIRST_PLAYER_STEP;
    break;
  case WAITING_SECOND_PLAYER_STEP:
    if (cell_index != -1) {
      rect = (SDL_Rect){(cell_index % 3) * FRAME_SIZE,
                        (cell_index / 3) * FRAME_SIZE, FRAME_SIZE, FRAME_SIZE};
      SDL_RenderCopy(ren, tex_first_player_symbol, NULL, &rect);
      cell_index = -1;
    }
    break;
  case WAITING_FIRST_PLAYER_STEP:
    if (cell_index != -1) {
      rect = (SDL_Rect){(cell_index % 3) * FRAME_SIZE,
                        (cell_index / 3) * FRAME_SIZE, FRAME_SIZE, FRAME_SIZE};
      SDL_RenderCopy(ren, tex_second_player_symbol, NULL, &rect);
      cell_index = -1;
    }
    break;
  case FIRST_PLAYER_WIN:
    SDL_RenderClear(ren);
    rect = (SDL_Rect){0, WIN_SIZE / 2, WIN_SIZE - 10, 50};
    SDL_RenderCopy(ren, tex_first_player_win, NULL, &rect);
    break;
  case SECOND_PLAYER_WIN:
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    rect = (SDL_Rect){0, WIN_SIZE / 2, WIN_SIZE - 10, 100};
    SDL_RenderCopy(ren, tex_second_player_win, NULL, &rect);
    break;
  case NICHYA:
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    rect = (SDL_Rect){0, WIN_SIZE / 2, WIN_SIZE - 10, 100};
    SDL_RenderCopy(ren, tex_nichya, NULL, &rect);
    break;
  }
  SDL_RenderPresent(ren);
  check_is_win_or_nichya();
}

void update() {
  if (cell_index < 0 || cell_index > 8)
    return;

  if (symbols[cell_index] == 0) {
    switch (state) {
    case FIRST_PLAYER_STEP:
      symbols[cell_index] = 1;
      state = WAITING_SECOND_PLAYER_STEP;
      break;
    case SECOND_PLAYER_STEP:
      symbols[cell_index] = 2;
      state = WAITING_FIRST_PLAYER_STEP;
      break;
    }
  }
}



void processInput() { return; }

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_Window *win = SDL_CreateWindow("Notes and crosses", 0, 0, WIN_SIZE, WIN_SIZE, 0);
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 200);

  if (!font) {
    return 1;
  }

  SDL_Surface *surf_notes = TTF_RenderText_Blended(font, "O", white);
  tex_first_player_symbol = SDL_CreateTextureFromSurface(ren, surf_notes);
  SDL_FreeSurface(surf_notes);

  SDL_Surface *surf_crosses = TTF_RenderText_Blended(font, "X", white);
  tex_second_player_symbol = SDL_CreateTextureFromSurface(ren, surf_crosses);
  SDL_FreeSurface(surf_crosses);

  SDL_Surface *surf_choose_first_player_letter =
      TTF_RenderText_Blended(font,
                             "START. Type o to start symbol o. Type x to start symbol x",
                             white);
  tex_choose_first_player_letter =
      SDL_CreateTextureFromSurface(ren, surf_choose_first_player_letter);
  SDL_FreeSurface(surf_choose_first_player_letter);

  SDL_Surface *surf_first_player_win =
      TTF_RenderText_Blended(font,
                             "WIN: player 1. Type c for play again. Type q for exit.",
                             white);
  tex_first_player_win =
      SDL_CreateTextureFromSurface(ren, surf_first_player_win);
  SDL_FreeSurface(surf_first_player_win);

  SDL_Surface *surf_second_player_win =
      TTF_RenderText_Blended(font,
                             "WIN: player 2. Type c for play again. Type q for exit.",
                             white);
  tex_second_player_win =
      SDL_CreateTextureFromSurface(ren, surf_second_player_win);
  SDL_FreeSurface(surf_second_player_win);

  SDL_Surface *surf_nichya =
      TTF_RenderText_Blended(font,
                             "WIN: nobody. Type c for play again. Type q for exit.",
                             white);

  tex_nichya = SDL_CreateTextureFromSurface(ren, surf_nichya);
  SDL_FreeSurface(surf_nichya);
  for (int i = 0; i < 3; i++) {
    SDL_Event e;
    while (SDL_PollEvent(&e))
      ;
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
    SDL_Delay(10);
  }
  bool quit = false;
  while (!quit) {
    SDL_Event event;
   
    
      double start = getCurrentTime();
      processInput();
      render();
      double elapsed = getCurrentTime() - start;
      if (MS_PER_FRAME > elapsed) {
        usleep((MS_PER_FRAME - elapsed) * 1000);
      }
    }
  }
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyTexture(tex_second_player_symbol);
  SDL_DestroyTexture(tex_first_player_symbol);
  SDL_DestroyTexture(tex_choose_first_player_letter);
  SDL_DestroyTexture(tex_first_player_win);
  SDL_DestroyTexture(tex_second_player_win);
  SDL_DestroyTexture(tex_nichya);
  SDL_Quit();
  return 0;
}
