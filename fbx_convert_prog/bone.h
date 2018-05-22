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
	static vector<float> cylinder;
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
		/*if (parent == NULL)
			vpos.push_back(vec4(origin, index));
		else
			vpos.push_back(vec4(origin, parent->index));
		vec3 endp = origin + pos;
		vpos.push_back(vec4(endp, index));*/
		vec3 endp = origin + pos;
		for (int i = 0; i < cylinder.size() - 3; i++)
		{
			vec4 vert = vec4(cylinder[i], cylinder[i + 1], cylinder[i + 2], 1.0f);
			vert = glm::translate(mat4(1.0f), glm::mix(origin, endp, .5f)) * vert;
			//TODO: Rotate into position
			vpos.push_back(vec4(vert.x, vert.y, vert.z, index));
		}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBO(endp, vpos);
	}
};
int readtobone(bone **root);
static void setCylinder(vector<float> in)
{
	bone::cylinder = in;
}
