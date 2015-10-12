#version 450 core

in vec2 texCoord;
out vec4 color;

uniform sampler2D texUnit;

void main() {
    color = texture(texUnit, texCoord);
}