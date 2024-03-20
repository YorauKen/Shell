CC = g++

TARGET = shell
SRCS = shell.cpp   # Replace 'shell.cpp' with your main source file
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC)  -o $@ $^

%.o: %.cpp
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
