#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform mat4 M;
uniform vec2 offset;
uniform vec4 color;
uniform float Z;

void main()
{
    float scale = 0.05f / .005;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = P * M * vec4((vertex.xy * scale) + offset, 0.0f, 1.0);
}