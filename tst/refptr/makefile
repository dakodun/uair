CXX=g++
CXXFLAGS=-Wall -Wno-unused -std=c++17 -ggdb -pipe
INCPATHS=-I ../../inc -I ../../inc/freetype -I ./inc
LIBPATHS=-L ../../bin -L ../../lib -L ./bin
PRJFLAGS=-DGLEW_STATIC=1 -DGLEW_MX=1 -D_GLIBCXX_DEBUG -DGLM_FORCE_RADIANS -DHAVE_GETTIMEOFDAY -D__WINDOWS_WASAPI__ -D_UAIR_DYNAMIC -D_UAIR_DEBUG

LIBSHARED=-luair -llua51
LIBSTATIC=-lglu32 -lopengl32 -lgdi32

SOURCES=$(wildcard *.cpp)
OBJECTS=$(addprefix obj/, $(SOURCES:.cpp=.o))

TARGET=..\..\bin\refptr.exe

$(TARGET): $(OBJECTS)
	windres ../../res/resources.rc -o obj/resources.o
	$(CXX) $(LIBPATHS) $(OBJECTS) obj/resources.o -Wl,-Bstatic $(LIBSTATIC) -Wl,-Bdynamic $(LIBSHARED) -o $(TARGET)

obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATHS) $(PRJFLAGS) -o $@ -c $<

define CLEAN
if exist $(obj) del $(obj)

endef

clean:
	$(foreach obj,$(subst /,\,$(OBJECTS)),$(CLEAN))
	if exist obj\resources.o del obj\resources.o
	if exist $(TARGET) del $(TARGET)
