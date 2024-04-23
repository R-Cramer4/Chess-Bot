#include "Resources.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

map<string, Shader> Resources::Shaders;
map<string, Texture2D> Resources::Textures;


// creates shader program
Shader Resources::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, string name){
    Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}
Shader Resources::GetShader(string name){
    return Shaders[name];
}
// creates texture
Texture2D Resources::LoadTexture(const char *file, bool alpha, string name){
    Textures[name] = LoadTextureFromFile(file, alpha);
    return Textures[name];
}
Texture2D Resources::GetTexture(string name){
    return Textures[name];
}
// clears resources
void Resources::Clear(){
    for(auto i : Shaders){
        glDeleteProgram(i.second.ID);
    }
    for(auto i : Textures){
        glDeleteTextures(1, &i.second.ID);
    }
}
Shader Resources::LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile){
    string vertexCode;
    string fragmentCode;
    string geometryCode;
    try{
        ifstream vertexShaderFile(vShaderFile);
        ifstream fragmentShaderFile(fShaderFile);
        stringstream vShaderStream, fShaderStream;

        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if(gShaderFile != nullptr){
            ifstream geoShaderFile(gShaderFile);
            stringstream gShaderStream;
            gShaderStream << geoShaderFile.rdbuf();
            geoShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }catch(exception e){
        cout << "failed to read shader files" << endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();

    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;

}
Texture2D Resources::LoadTextureFromFile(const char *file, bool alpha){
    Texture2D texture;
    if(alpha){
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    int width, height, nrChannels;
    unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);

    texture.Generate(width, height, data);
    stbi_image_free(data);
    return texture;
}
