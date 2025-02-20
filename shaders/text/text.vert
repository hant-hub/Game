#version 450


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;
layout(location = 3) in float layer;

layout (location = 0) out vec2 fragUV;
layout (location = 1) out vec4 TextColor;

layout(binding = 0) uniform UniformBuffer {
    mat4 proj;
    mat4 view;
} ub;

void main() {
    gl_Position = ub.proj * ub.view * vec4(inPosition, layer, 1.0f);
    fragUV = uv;
    TextColor = color;
}

