FLAGS = -std=c++20 -g -O0
INC_MAC= -Iinclude -I/opt/homebrew/include -I/usr/local/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks 
GL_MAC= -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
INC_LINUX= -Iinclude
GL_LINUX= -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
SRC = src
GRAPHICS = src/graphics
OBJ = obj
TEST = test

main: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o $(OBJ)/Piece.o $(OBJ)/Board.o $(OBJ)/Test.o $(OBJ)/Move.o
	g++ $^ ${INC_MAC} ${GL_MAC} $(FLAGS) -o main

testEx: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o $(OBJ)/Piece.o $(OBJ)/Board.o $(OBJ)/Test.o $(OBJ)/Move.o
	g++ $^ ${INC_MAC} ${GL_MAC} $(FLAGS) -o testBot

$(OBJ)/main.o: $(SRC)/main.cpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/main.cpp -o $@

$(OBJ)/Game.o: $(SRC)/Game.cpp $(SRC)/Game.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Game.cpp -o $@

$(OBJ)/Shader.o: $(GRAPHICS)/Shader.cpp $(GRAPHICS)/Shader.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(GRAPHICS)/Shader.cpp -o $@

$(OBJ)/Texture.o: $(GRAPHICS)/Texture.cpp $(GRAPHICS)/Texture.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(GRAPHICS)/Texture.cpp -o $@

$(OBJ)/Resources.o: $(GRAPHICS)/Resources.cpp $(GRAPHICS)/Resources.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(GRAPHICS)/Resources.cpp -o $@

$(OBJ)/SpriteRenderer.o: $(GRAPHICS)/SpriteRenderer.cpp $(GRAPHICS)/SpriteRenderer.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(GRAPHICS)/SpriteRenderer.cpp -o $@

$(OBJ)/Piece.o: $(GRAPHICS)/Piece.cpp $(GRAPHICS)/Piece.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(GRAPHICS)/Piece.cpp -o $@

$(OBJ)/Board.o: $(SRC)/Board.cpp $(SRC)/Board.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Board.cpp -o $@

$(OBJ)/Move.o: $(SRC)/Move.cpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Move.cpp -o $@

$(OBJ)/Test.o: $(SRC)/Test.cpp $(SRC)/Test.hpp
	g++ ${INC_MAC} ${FLAGS} -c $(SRC)/Test.cpp -o $@

test: testEx
	./testBot test $(TEST)/perftsuite.epd $(TEST)/output.epd

valgrind: main
	valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --keep-debuginfo=yes --smc-check=all --track-origins=yes ./main

perft: main
	/usr/bin/time ./main perft 5


clean:
	rm -f $(OBJ)/*.o main testBot


