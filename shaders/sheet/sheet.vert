#version 450

struct Uniform {
    mat4 model;
    vec2 uvoffset;
    vec2 uvscale; 
    uint texIndex;
};


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out uint index;

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
} ub;

layout(std140, set = 0, binding = 1) readonly buffer ModelBuffer {
    Uniform ModelData[];
} models;




void main() {
    gl_Position = ub.proj * 
                  ub.view * 
                  models.ModelData[gl_InstanceIndex].model * 
                  vec4(inPosition, 1.0f, 1.0f);

    fragUV = (inTexCoords - models.ModelData[gl_InstanceIndex].uvoffset) *
             models.ModelData[gl_InstanceIndex].uvscale; 

    index = models.ModelData[gl_InstanceIndex].texIndex;
}

