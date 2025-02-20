#version 450

layout (location = 0) in vec2 fragUV;
layout (location = 1) in vec4 TextColor;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texsampler;

void main() {
 //   outColor = vec4(1.0f);
    outColor = vec4(texture(texsampler, fragUV).r) 
        * TextColor;
}
