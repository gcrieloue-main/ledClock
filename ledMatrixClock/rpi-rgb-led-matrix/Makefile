CXXFLAGS=-Wall -O3 -g -std=c++0x 
OBJECTS=led-clock.o CanvasAnimator.o CanvasConfig.o socket.o
BINARIES=led-clock
ALL_BINARIES=$(BINARIES)

# Where our library resides. It is split between includes and the binary
# library in lib
RGB_INCDIR=include
RGB_LIBDIR=lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

PYTHON_LIB_DIR=python

# Imagemagic flags, only needed if actually compiled.
MAGICK_CXXFLAGS=`GraphicsMagick++-config --cppflags --cxxflags`
MAGICK_LDFLAGS=`GraphicsMagick++-config --ldflags --libs`

all : $(BINARIES)

$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR)

led-clock : led-clock.o CanvasAnimator.o CanvasConfig.o socket.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

led-clock.o : led-clock.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

CanvasAnimator.o : CanvasAnimator.cc 
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

CanvasConfig.o : CanvasConfig.cc 
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

socket.o : socket.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(ALL_BINARIES)
	$(MAKE) -C lib clean
	$(MAKE) -C $(PYTHON_LIB_DIR) clean

build-python: $(RGB_LIBRARY)
	$(MAKE) -C $(PYTHON_LIB_DIR) build

install-python: build-python
	$(MAKE) -C $(PYTHON_LIB_DIR) install

FORCE:
.PHONY: FORCE
