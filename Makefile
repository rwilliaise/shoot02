NAME=shoot02
CC=gcc
CFLAGS=-Wall -I.
LDFLAGS=$(shell pkg-config --libs glfw3 opengl) -lm
OBJECTS=shoot02.o gl.o res.o r_camera.o r_shader.o

$(NAME): $(OBJECTS) 

clean:
	rm -f $(OBJECTS)
	rm -f $(NAME)

