#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>

using namespace std;

class Shader{
    public:
        unsigned int ID; // program id
        
        Shader(){ }

        Shader &Use();

        void Compile(const char *vertexSource, const char *fragmentSource, const char *geoSource = nullptr);

        void SetInteger(const char *name, int value, bool useShader = false);
        void SetFloat(const char *name, float value, bool useShader = false);
        void SetVector2f (const char *name, float x, float y, bool useShader = false);
        void SetVector2f (const char *name, const glm::vec2 &value, bool useShader = false);
        void SetVector3f (const char *name, float x, float y, float z, bool useShader = false);
        void SetVector3f (const char *name, const glm::vec3 &value, bool useShader = false);
        void SetVector4f (const char *name, float x, float y, float z, float w, bool useShader = false);
        void SetVector4f (const char *name, const glm::vec4 &value, bool useShader = false);
        void SetMatrix4  (const char *name, const glm::mat4 &matrix, bool useShader = false);

    private:
        void checkCompileErrors(unsigned int shader, string type);

};

#endif
