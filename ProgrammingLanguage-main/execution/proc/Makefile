ifeq ($(OS),Windows_NT)
	OUTPUT_FILE_NAME = main.exe
else
    OUTPUT_FILE_NAME = main
endif

CC = g++
CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -D_DEBUG -D_EJUDGE_CLIENT_SIDE
IFLAGS = -I./include/ 

#main

SRC_DIR = ./
OBJ_DIR = ./lib/
UTILS_DIR = ../../utils/

SRC   = proc.cpp
UTILS = fileUtils.cpp stringUtils.cpp stack.cpp
OBJ   = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC)) 
U_OBJ = $(patsubst %.cpp, $(UTILS_DIR)/$(OBJ_DIR)%.o, $(UTILS)) 

#linking
all : $(OBJ) $(U_OBJ) 
	$(CC) $(IFLAGS) $(CFLAGS) $^ -o $(OUTPUT_FILE_NAME)

#main
$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

#utils
$(UTILS_DIR)/$(OBJ_DIR)%.o : $(UTILS_DIR)%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

clear: 
	rm -f $(OBJ_DIR)*.o