#version 410 core
layout(location = 0) in vec4 vertPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M[100];
uniform mat4 S;
void main()
{
	int index = int(vertPos.w+0.0001);
	vec4 pos = M[index] * vec4(vertPos.xyz,1.0);
	gl_Position = P * V * S * pos;
}
