FLAGS = -std=c++20
INC= -I/Users/jake/Documents/Jake/computer-science/cpp/quantum_chess/include -I/opt/homebrew/include -I/usr/local/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks 
GL= -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
SRC = src
OBJ = obj

main: src/glad.c $(OBJ)/main.o $(OBJ)/Game.o $(OBJ)/Shader.o $(OBJ)/Texture.o $(OBJ)/Resources.o $(OBJ)/SpriteRenderer.o
	g++ $^ ${INC} ${GL} $(FLAGS) -o main

$(OBJ)/main.o: $(SRC)/main.cpp
	g++ ${INC} ${FLAGS} -c $(SRC)/main.cpp -o $@

$(OBJ)/Game.o: $(SRC)/Game.cpp $(SRC)/Game.h
	g++ ${INC} ${FLAGS} -c $(SRC)/Game.cpp -o $@

$(OBJ)/Shader.o: $(SRC)/Shader.cpp $(SRC)/Shader.h
	g++ ${INC} ${FLAGS} -c $(SRC)/Shader.cpp -o $@

$(OBJ)/Texture.o: $(SRC)/Texture.cpp $(SRC)/Texture.h
	g++ ${INC} ${FLAGS} -c $(SRC)/Texture.cpp -o $@

$(OBJ)/Resources.o: $(SRC)/Resources.cpp $(SRC)/Resources.h
	g++ ${INC} ${FLAGS} -c $(SRC)/Resources.cpp -o $@

$(OBJ)/SpriteRenderer.o: $(SRC)/SpriteRenderer.cpp $(SRC)/SpriteRenderer.h
	g++ ${INC} ${FLAGS} -c $(SRC)/SpriteRenderer.cpp -o $@

clean:
	rm -f $(OBJ)/*.o main


