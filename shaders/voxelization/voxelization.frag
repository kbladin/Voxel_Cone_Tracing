#version 450 core
#extension GL_ARB_shader_image_load_store : enable

out vec4 color;
in vec3 vertexPosition_worldspace;

layout(RGBA32F) uniform image3D voxelImage;

void main()
{
    //ivec3 size = imageSize(voxelImage);
    int hej = 64;
    color = vec4(hej,hej,hej, 1.0);
    ivec3 imageCoord = ivec3(32, 32, 32);
    vec4 data = vec4(1,1,1,1);
    imageStore(voxelImage, imageCoord, vec4(1.0f, 1.0f, 1.0f, 1.0f));
}