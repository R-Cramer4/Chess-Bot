FLAGS = -std=c++20 -g
INC_MAC= -I/Users/jake/Documents/Jake/computer-science/cpp/quantum_chess/include -I/opt/homebrew/include -I/usr/local/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks 
GL_MAC= -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
INC_LINUX= -Iinclude
GL_LINUX= -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
SRC = src
OBJ = obj

main: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o $(OBJ)/Piece.o $(OBJ)/Board.o
	g++ $^ ${INC_LINUX} ${GL_LINUX} $(FLAGS) -o main

$(OBJ)/main.o: $(SRC)/main.cpp
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/main.cpp -o $@

$(OBJ)/Game.o: $(SRC)/Game.cpp $(SRC)/Game.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Game.cpp -o $@

$(OBJ)/Shader.o: $(SRC)/Shader.cpp $(SRC)/Shader.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Shader.cpp -o $@

$(OBJ)/Texture.o: $(SRC)/Texture.cpp $(SRC)/Texture.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Texture.cpp -o $@

$(OBJ)/Resources.o: $(SRC)/Resources.cpp $(SRC)/Resources.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Resources.cpp -o $@

$(OBJ)/SpriteRenderer.o: $(SRC)/SpriteRenderer.cpp $(SRC)/SpriteRenderer.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/SpriteRenderer.cpp -o $@

$(OBJ)/Piece.o: $(SRC)/Piece.cpp $(SRC)/Piece.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Piece.cpp -o $@

$(OBJ)/Board.o: $(SRC)/Board.cpp $(SRC)/Board.h
	g++ ${INC_LINUX} ${FLAGS} -c $(SRC)/Board.cpp -o $@

clean:
	rm -f $(OBJ)/*.o main


