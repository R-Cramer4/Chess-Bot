FLAGS = -std=c++20 -g -O0
INC_MAC= -Iinclude -I/opt/homebrew/include -I/usr/local/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks 
GL_MAC= -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
INC_LINUX= -Iinclude
GL_LINUX= -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
SRC = src
OBJ = obj
TEST = test

main: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o $(OBJ)/Piece.o $(OBJ)/Board.o $(OBJ)/Test.o
	g++ $^ ${INC_MAC} ${GL_MAC} $(FLAGS) -o main

$(OBJ)/main.o: $(SRC)/main.cpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/main.cpp -o $@

$(OBJ)/Game.o: $(SRC)/Game.cpp $(SRC)/Game.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Game.cpp -o $@

$(OBJ)/Shader.o: $(SRC)/Shader.cpp $(SRC)/Shader.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Shader.cpp -o $@

$(OBJ)/Texture.o: $(SRC)/Texture.cpp $(SRC)/Texture.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Texture.cpp -o $@

$(OBJ)/Resources.o: $(SRC)/Resources.cpp $(SRC)/Resources.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Resources.cpp -o $@

$(OBJ)/SpriteRenderer.o: $(SRC)/SpriteRenderer.cpp $(SRC)/SpriteRenderer.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/SpriteRenderer.cpp -o $@

$(OBJ)/Piece.o: $(SRC)/Piece.cpp $(SRC)/Piece.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Piece.cpp -o $@

$(OBJ)/Board.o: $(SRC)/Board.cpp $(SRC)/Board.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Board.cpp -o $@

valgrind: main
	valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --keep-debuginfo=yes --smc-check=all --track-origins=yes ./main

perft: main
	/usr/bin/time ./main perft 5


$(OBJ)/Test.o: $(SRC)/Test.cpp $(SRC)/Test.h
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Test.cpp -o $@

testEx: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o $(OBJ)/Piece.o $(OBJ)/Board.o $(OBJ)/Test.o
	g++ $^ ${INC_MAC} ${GL_MAC} $(FLAGS) -o testBot

test: testEx
	./testBot test $(TEST)/perftsuite.epd $(TEST)/output.epd


clean:
	rm -f $(OBJ)/*.o main testBot


