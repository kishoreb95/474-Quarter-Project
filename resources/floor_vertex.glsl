#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
void main()
{
    vec4 pos = M * vec4(vertPos,1.0);
    gl_Position = P * V * pos;
	vec3 norm = vec4(M * vec4(vertNor,0.0)).xyz;
	vertex_normal = norm.xyz;
    vertex_pos = pos.xyz;
	vertex_tex = vertTex;
}