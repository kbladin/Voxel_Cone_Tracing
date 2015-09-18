#version 410 core

in vec2 texCoord;
uniform sampler3D texUnit;
out vec4 color;

void main(void)
{
    color = texture(texUnit, vec3(texCoord.x,texCoord.y,1));
}
