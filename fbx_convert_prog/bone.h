#pragma once
#include <memory>
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


class keyframe
	{
	public:
		quat quaternion;
		vec3 translation;
		long long timestamp_ms;
		mat4 convert()
		{
			return translate(mat4(1.0f), translation) * mat4_cast(quaternion);
		};
	};

//**************************************************

class bone
{
public:
	vector<shared_ptr<keyframe>> keyframes;
	mat4 *mat = NULL;
	mat4 arrayelem = mat4(1.0f); 
	void matrix(int time, mat4 parent, vector<mat4> &models)
	{
		arrayelem =  parent * keyframes[time]->convert();
		models[index] = arrayelem;
		for (int i = 0; i < kids.size(); i++)
			kids[i]->matrix(time, arrayelem, models);
	}
	string name;
	vec3 pos; //End point of this bone
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;
	int index;
	void write_to_VBO(vec3 origin, vector<vec4> &vpos)
	{
		if (parent == NULL)
			vpos.push_back(vec4(origin, index));
		else
			vpos.push_back(vec4(origin, parent->index));
		vec3 endp = origin + pos;
		vpos.push_back(vec4(endp, index));
		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBO(endp, vpos);
	}
};
int readtobone(bone **root);