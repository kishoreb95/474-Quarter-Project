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
	vector<shared_ptr<keyframe>> anim2;
	static vector<float> cylinder;
	static vector<float> cylinder_normals;
	mat4 *mat = NULL;
	mat4 arrayelem = mat4(1.0f); 
	void matrix(int time, mat4 parent, vector<mat4> &models, int mode)
	{
		if (mode == 0)
			arrayelem =  parent * keyframes[time]->convert();
		else
			arrayelem = parent * anim2[time]->convert();

		models[index] = arrayelem;
		for (int i = 0; i < kids.size(); i++)
			kids[i]->matrix(time, arrayelem, models, mode);
	}
	string name;
	vec3 pos; //End point of this bone
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;
	int index;
	void write_to_VBO(vec3 origin, vector<vec4> &vpos, vector<vec3> &vnorm)
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
			vec3 norm = vec3(cylinder_normals[i], cylinder_normals[i + 1], cylinder_normals[i + 2]);
			if (parent == NULL || vert.y < 0.0f)
				vpos.push_back(vec4(vert.x, vert.y, vert.z, index));
			else
				vpos.push_back(vec4(vert.x, vert.y, vert.z, parent->index));
			vnorm.push_back(norm);
		}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBO(endp, vpos, vnorm);
	}
};
int readtobone(bone **root);
static void setCylinder(vector<float> in)
{
	bone::cylinder = in;
}
