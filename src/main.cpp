#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Game *game = new Game();

int main(int argc, const char *argv[]){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "quantum chess", NULL, NULL);
    if(window == NULL){
        cout << "didnt create window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize glad" << endl;
        glfwTerminate();
        return -1;
    }

    // blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // game
    if(argc == 2) game->Init(argv[1]);
    else if(argc == 3) game->Init(argv[1], stoi(argv[2]));
    else game->Init();


    // window loop
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // rendering
        glClearColor((17.5 / 100.0f), (16.3 / 100.0f), (18.5 / 100.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game->Render();

        if(game->turn == NONE) break;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    game->Clear();
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if(!game->Update(x, y)) glfwSetWindowShouldClose(window, true);
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) game->bitboard.unMovePiece();

}

