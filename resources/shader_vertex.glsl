#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in uint vertimat;
layout(location = 2) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
uniform mat4 Manim[200];
void main()
{
	mat4 Ma = Manim[vertimat];
    vec4 pos = Ma*vec4(vertPos,1.0);
    //pos.x = Ma[3][0];  
    //pos.y = Ma[3][1];
    //pos.z = Ma[3][2];
    gl_Position = P * V * M * pos;
	vec4 norm = Ma*vec4(vertNor,1.0);
	vertex_normal = norm.xyz;
    vertex_pos = pos.xyz;
	
}
