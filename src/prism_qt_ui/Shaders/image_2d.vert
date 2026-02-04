//image_2d.vert
#version 440

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 vTexCoord;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec2 uvScale;
};

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    vTexCoord =   vec2(texCoord.x * uvScale.x, texCoord.y * uvScale.y);
    gl_Position =   qt_Matrix * vec4(vertex, 0.0, 1.0)  ;
}

