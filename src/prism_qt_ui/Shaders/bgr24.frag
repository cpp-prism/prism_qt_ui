//bgr24.frag
#version 440

layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D tex_1;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec2 uvScale;
};

void main()
{
    fragColor = texture(tex_1, vTexCoord);
}
