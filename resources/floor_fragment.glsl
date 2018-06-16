#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

void main()
{
color.rgb = vec3(1,1,1)*0.1 + vec3(0.6, 0.3, 0);
}
