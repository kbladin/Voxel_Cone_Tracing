#version 450
#extension GL_ARB_shader_image_load_store : enable

layout(RGBA8) uniform image2D image;

in vec2 texCoord;

//out vec4 color;

void main() {
    imageStore(image, ivec2(32+0, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec2(32+1, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec2(32+2, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec2(32+3, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //color = vec4(1,1,1,1);
}