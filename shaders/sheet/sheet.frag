#version 450

layout(location = 0) in vec2 fragUV;
layout (location = 1) in flat uint index;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texsampler[1];



void main() {
    outColor = texture(texsampler[index], fragUV);
}
