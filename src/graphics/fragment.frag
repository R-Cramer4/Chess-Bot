#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main(){
    vec4 f = texture(image, TexCoords);
    if(f.w == 0.0) discard;
    color = vec4(spriteColor, 1.0) * f;
}
