NAME=shoot02
CC=gcc
CFLAGS=-Wall -I. -g
LDFLAGS=$(shell pkg-config --libs assimp glfw3 opengl) -lm
OBJECTS=shoot02.o stb_image.o gl.o res.o r_res.o r_model.o r_camera.o \
		r_shader.o libmap/geo_generator.o libmap/map_data.o \
		libmap/map_parser.o libmap/surface_gatherer.o

$(NAME): $(OBJECTS) 

clean:
	rm -f $(OBJECTS)
	rm -f $(NAME)

