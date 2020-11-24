

TARGET = benchmark
CC := g++
CFLAGS := -g 
LFLAGS := -L/opt/X11/lib -lX11 -lXext -pthread -std=c++11
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS))
all:$(TARGET)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< $(LFLAGS)
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
clean:
	rm -rf $(TARGET) *.o
