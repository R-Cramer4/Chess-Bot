#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


int main(){

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

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize glad" << endl;
        return -1;
    }


    // process vertex
    /*
    float vertices[] = {
        // position           // colors         // texture coords
        0.8f,  0.8f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f,  // top right
        0.8f, -0.8f, 0.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // bottom right
        -0.8f, -0.8f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom left
        -0.8f,  0.8f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f   // top left
    };
    */
    // game
    Game *game = new Game();
    game->Init();


    // window loop
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // rendering
        glClearColor((17.5 / 100.0f), (16.3 / 100.0f), (18.5 / 100.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

void displayPieces(Game *game){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            // loop through the board
            switch(game->board[i][j]){
                case 'P':

                    break;
                default:
                    break;
            }
        }
    }

}
