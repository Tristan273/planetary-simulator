CC = gcc

CFLAGS = -Wall -Wextra -O2 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lm

SRC = rendering.c physics.c
OBJ = $(SRC:.c=.o)

TARGET = program

all: $(TARGET)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Creating the .o
%.o: %.c physics.h
	$(CC) -c $< -o $@ $(CFLAGS)

# Remove the .o
clean:
	rm -f *.o

# Remove the final programm
fclean: clean
	rm -f $(TARGET)

re: fclean all