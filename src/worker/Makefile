SOURCES := worker.cpp
SOURCES += $(wildcard */*.cpp)
_OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS =$(addprefix $(BIN)/, $(_OBJECTS))

CPP = g++
CPPFLAGS = -std=c++17
LIBS = -lpng -ljpeg -lrt

BIN = bin


$(BIN)/worker : $(OBJECTS)
	$(CPP) $(CPPFLAGS) $^ -o $@ $(LIBS)

$(BIN)/%.o: %.cpp
	@mkdir -p  $(@D)
	$(CPP) $(CPPFLAGS) -c $^ -o $@

clean:
	rm -rf ./bin/*