#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstant
{
    vec3 color;
    vec3 direction;
}ps;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

const float AMBIENT = 0.1;//间接照明

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    vec3 normalWorldSpace=normalize(mat3(ubo.model)*inNormal);

    float lightIntensity=AMBIENT + max(dot(normalWorldSpace,normalize(ps.direction)),0);

    fragColor =lightIntensity* ps.color;
    fragTexCoord = inUv;
}