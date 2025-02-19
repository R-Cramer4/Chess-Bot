#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <string>
#include "glad/glad.h"
#include "Shader.hpp"
#include "Texture.hpp"

using namespace std;

class Resources{
    public:
        // resource storage
        static map<string, Shader> Shaders;
        static map<string, Texture2D> Textures;
        // creates shader program
        static Shader LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, string name);
        static Shader GetShader(string name);
        // creates texture
        static Texture2D LoadTexture(const char *file, bool alpha, string name);
        static Texture2D GetTexture(string name);
        // clears resources
        static void Clear();

    private:
        Resources(){ }
        static Shader LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile);
        static Texture2D LoadTextureFromFile(const char *file, bool alpha);

};

#endif
