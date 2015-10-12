/*#version 450
#extension GL_ARB_shader_image_load_store : enable

//out vec4 color;
in vec3 vertexPosition_worldspace;

//layout(RGBA32F) uniform image3D voxelImage;
layout(RGBA8) uniform image3D image;

void main()
{
    //ivec3 size = imageSize(voxelImage);
    //int hej = 64;
    //color = vec4(hej,hej,hej, 1.0);
    //ivec3 imageCoord = ivec3(32, 32, 32);
    //vec4 data = vec4(1,1,1,1);

    imageStore(image, ivec3(32, 32+0, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec3(32, 32+1, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec3(32, 32+2, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    imageStore(image, ivec3(32, 32+3, 32), vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
*/

#version 450
#extension GL_ARB_shader_image_load_store : enable

layout(RGBA8) uniform image3D voxelImage;

in vec3 vertexPosition_worldspace;

out vec4 color;

void main() {
    imageStore(voxelImage, ivec3(32, 32+0, 32), vec4(0.0f, 1.0f, 1.0f, 1.0f));
    color = vec4(1,1,1,1);
}