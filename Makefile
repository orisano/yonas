BINDIR := bin
SRCDIR := src
BUILDDIR := build
DEPSDIR := deps
PICOHTTPPARSERDIR := $(DEPSDIR)/picohttpparser

TARGET := $(BINDIR)/yonas
OBJS := main.o picohttpparser.o
OBJECTS := $(addprefix $(BUILDDIR)/,$(OBJS))
INCLUDES := include $(PICOHTTPPARSERDIR)

CXXFLAGS := -Wall -Wextra -O2 -std=c++11 $(addprefix -I,$(INCLUDES))
CFLAGS := -O2


all: $(TARGET)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJECTS)

ensure:
	@mkdir -p $(DEPSDIR)
	git clone https://github.com/h2o/picohttpparser $(PICOHTTPPARSERDIR)


$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) -o $@ $?

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/picohttpparser.o: $(PICOHTTPPARSERDIR)/picohttpparser.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONEY: all clean ensure
