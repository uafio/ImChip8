CC 	     = g++
CFLAGS 	 = -g
LIBS     = -lGL -lGLEW -lglfw
INCLUDES = -I ../gl3w/include/GLFW/
SRCS     = ../main.cpp ../chip8emu.cpp ../imgui/imgui.cpp ../imgui/imgui_draw.cpp \
       	   ../imgui/imgui_impl_glfw.cpp ../imgui/imgui_impl_opengl3.cpp \
       	   ../imgui/imgui_widgets.cpp
OBJS     = $(SRCS:.c=.o)
MAIN     = chip8

all:    $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

clean:
	$(RM) *.o $(MAIN)
