#version 440

layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D tex_1;
layout(binding = 2) uniform sampler2D tex_2;
layout(binding = 3) uniform sampler2D tex_3;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec2 uvScale;
};

const mat3 yuv2rgb = mat3(
1, 1, 1, 0, -0.39465,  2.03211, 1.13983, -0.58060,  0
);


//wikipedia
// 1, 1, 1, 0, -0.39465,  2.03211, 1.13983, -0.58060,  0
// 1, 1, 1, 0, -0.21482,  2.12798, 1.28033, -0.38059,  0

void main()
{
    vec3 yuv;
    yuv.x = texture(tex_1,  vTexCoord).r;
    yuv.y = texture(tex_2, vTexCoord).r - 0.5;
    yuv.z = texture(tex_3, vTexCoord).r - 0.5;

    vec3 rgb = yuv2rgb * yuv;

    fragColor = vec4(rgb, 1.0);
}
