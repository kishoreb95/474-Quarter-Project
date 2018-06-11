#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
void main()
{
    vec4 pos = M * vec4(vertPos,1.0);
    gl_Position = P * V * pos;
	vec4 norm = M * vec4(vertNor,1.0);
	vertex_normal = norm.xyz;
    vertex_pos = pos.xyz;
}