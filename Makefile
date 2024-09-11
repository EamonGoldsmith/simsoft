# Compiler and flags
CXX = clang++
CXXFLAGS = -Wall -O2 -I/usr/local/include
LIBS = -L/usr/local/lib -lGL -lGLU -lglut
APP = freeglut_window

SRC = *
OBJS = $($(SRC):T)

all: $(OBJS)
	echo $(ALLSRC)

# create obj dir
#$(OBJ_DIR):
	#mkdir -p $(OBJ_DIR)

# link
#$(TARGET): $(OBJS)
	#$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# compile
#$(OBJ_DIR)/$(.PREFIX).o: $(SRC_DIR)/$(.PREFIX).cpp
	#$(CXX) $(CXXFLAGS) -c $(OBJ_DIR)/$(.TARGET) -o $(.PREFIX)

#.PHONY: clean run

#clean:
	#rm -f $(OBJS) $(TARGET)

#run: $(TARGET)
	#./$(TARGET)
