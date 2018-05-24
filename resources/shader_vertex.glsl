#version 410 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M[100];
uniform mat4 S;
out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
void main()
{
	int index = int(vertPos.w+0.0001);
	vec4 pos = S * M[index] * vec4(vertPos.xyz,1.0);
	vertex_normal = vertNor;
	vertex_tex = vertTex;
    vertex_pos = pos.xyz;

	gl_Position = P * V * pos;
}
