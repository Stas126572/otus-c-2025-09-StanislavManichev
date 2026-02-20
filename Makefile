RUN_FILE = programm_file.out
DIR_PATH = build
TARGET = $(DIR_PATH)/$(RUN_FILE)
DIR ?= .
THREAD_NUM ?= 2
# Только инклуды, без -O3
INC = -I../look_directory -I../get_size_of_file -I../get_files_contains #cat get_files_contains.h
CFLAGS += -Wall -Wextra $(INC)
SRC = main.c get_size_of_file/get_size_of_file.c look_directory/get_directory_files_name.c get_files_contains/get_files_contains.c
HDR = $(wildcard $(SRC:.c=.h))
VALGDRIND = valgrind
VALGDRINDFLAGS = -q --track-fds=yes --show-mismatched-frees=yes --leak-check=full --show-leak-kinds=all --track-origins=yes
IGNOREDESCRIPTORS ?= --ignore-fds=35
PROJECT_NAME=logs_file
PROGRAMM_ARGS=$(DIR) $(THREAD_NUM)
HELP_INSTRUCT=This is help. For make all file, write make all. For get the all inside files, write absolute_all. For get the defined values print make print_values. \
	      For look help, write make help. For make .tar file, write make dist. For get memory debug, write make debug. For test for thread race, write \
	      make thread_sanitazer. For make github, write make git. If you want to update git, write make push. For delete all make information, write make clean.
.PHONY: run all clean debug dist print_values create_directory help absolute_all git push


all: create_directory $(TARGET)

help:
	@echo $(HELP_INSTRUCT)



create_directory:
	@mkdir -p $(DIR_PATH)

print_values:
	@echo $(TARGET)


absolute_all: create_directory
	@$(CC) $(CFLAGS) -save-temps $(SRC) -o $(TARGET)

# Тут добавляем -O3 только для основной сборки
$(TARGET):
	@$(CC) $(CFLAGS) -O3 $(SRC) -o $@

run: $(TARGET) 
	@./$(TARGET) $(PROGRAMM_ARGS)

# Тут используем -g и -O0 (чтобы не было путаницы в стеке)
debug: clean create_directory
	@$(CC) $(CFLAGS) -g -O0 $(SRC) -o $(TARGET)
	@$(VALGDRIND) $(VALGDRINDFLAGS) ./$(TARGET) $(PROGRAMM_ARGS) 2>&1 \
  | grep -vE "Open file descriptor 35|inherited from parent|FILE DESCRIPTORS: 4 open" \
  | sed '/^==.*==[[:space:]]*$$/d' \
  || true 

thread_sanitazer: clean
	@$(CC) $(CFLAGS) -g -O1 -fsanitize=thread $(SRC) -o $(TARGET)
	@./$(TARGET) $(PROGRAMM_ARGS)

dist: debug clean
	@tar -cvzf $(PROJECT_NAME)_v1.tar.gz $(SRC) $(HDR) Makefile > /dev/null 2>&1
	@echo "Tar created"

git: 
	@git init
	@git add $(SRC) $(HDR) Makefile
	@read -p "Msg: " msg
	@git commit -m "$$msg"

push: debug clean git
	@git push origin main

clean:
	@$(RM) -rf $(DIR_PATH) $(TARGET) *.out *.tar.gz $(TARGET)-*
