TARGET = yonas
OBJS = main.o picohttpparser.o
CXXFLAGS = -O2 -std=c++11
CFLAGS = -O2

all: $(TARGET)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $?

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONEY: all clean
