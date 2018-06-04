LIBS=-lsfml-graphics -lsfml-window -lsfml-system
CXX := g++

all: ECS

ECS: main.cpp
  @echo "Building game..."
  $(CXX) -ECS $(LIBS)
  
clean: 
  @echo "Removing executable..."
  rm -f ECS
  
install:
  @echo "Installing..."
  cp ECS /usr/bin/
  
uninstall:
  @echo "Uninstalling..."
  $(RM) /usr/bin/ESC #or rm -f instead of $(RM)
