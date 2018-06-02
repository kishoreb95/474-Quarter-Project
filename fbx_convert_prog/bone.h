#pragma once
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;

class keyframe
	{
	public:
		quat quaternion;
		vec3 translation;
		long long timestamp_ms;
	};
class animation_per_bone
	{
	public:
		string name;
		long long duration;
		int frames;
		string bone;
		vector<keyframe> keyframes;
	};
class all_animations
	{
	public:
		vector<animation_per_bone> animations;

	};



//**************************************************

class bone
{
public:

	static vector<float> cylinder;
	static vector<float> cylinder_normals;

	vector<animation_per_bone*> animation;	//all the animations of the bone
	string name;
	vec3 pos;
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;			
	unsigned int index;			//a unique number for each bone, at the same time index of the animatiom matrix array
	mat4 *mat = NULL;			//address of one lement from the animation matrix array

	quat inter(quat prev, quat curr, quat next)
	{
		return exp((log(next * inverse(curr)) + log(prev * inverse(curr))) / -4.0f) * curr;
	}

	//interpolates between keyframenumber and keyframenumber + 1 in the animation
	quat quaterpolate(animation_per_bone *anim, int keyframenumber, float mix)
	{
		int prev = anim->keyframes.size() - 1;
		int next = 0;
		int next_next = 0;
		if (keyframenumber - 1 > 0)
			prev = keyframenumber - 1;
		if (keyframenumber + 1 < anim->keyframes.size())
			next = keyframenumber + 1;
		if (next + 1 < anim->keyframes.size())
			next_next = next + 1;

		quat s1 = inter(anim->keyframes[prev].quaternion, anim->keyframes[keyframenumber].quaternion, anim->keyframes[next].quaternion);
		quat s2 = inter(anim->keyframes[keyframenumber].quaternion, anim->keyframes[next].quaternion, anim->keyframes[next_next].quaternion);
		return squad(anim->keyframes[keyframenumber].quaternion, anim->keyframes[next].quaternion, s1, s2, mix);
	}

	// searches for the animation and sets the animation matrix element to the recent matrix gained from the keyframe
	void play_animation(int keyframenumber, string animationname)
		{
     // printf("%s\n", name);
		for (int i = 0; i < animation.size(); i++)
			if (animation[i]->name == animationname)
				{
				keyframenumber = keyframenumber % animation[i]->keyframes.size();
				if (animation[i]->keyframes.size() > keyframenumber)
					{
					quat q = animation[i]->keyframes[keyframenumber].quaternion;
					vec3 tr = animation[i]->keyframes[keyframenumber].translation; 
               
					if (name == "Hips")
						tr = vec3(0, 0, 0);
					mat4 M = mat4(q);
					mat4 T = translate(mat4(1), tr);
					M = T * M;
					if (mat)
						{
						mat4 parentmat = mat4(1);
						if (parent)
							parentmat = *parent->mat;
						*mat = parentmat * M;
						}
					}
				else
					*mat = mat4(1);
				}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->play_animation(keyframenumber,animationname);
		}

	//basically it takes floats for indices, so we can go between indices (1.5 is a .5 mix between 1 and 2)
	void play_animation(float keyframepos, string animationname) 
	{
		int nextkeyframe;
		int keyframenumber = (int)keyframepos;
		float mix = keyframepos - keyframenumber;
		for (int i = 0; i < animation.size(); i++)
			if (animation[i]->name == animationname)
			{

				keyframenumber = keyframenumber % (animation[i]->keyframes.size() - 1);
				nextkeyframe = (keyframenumber + 1) % (animation[i]->keyframes.size() - 1);
				quat q1, q2;
				vec3 t1, t2;
				vec3 tr = vec3(0, 0, 0);
				
				/*q1 = animation[i]->keyframes[keyframenumber].quaternion;
				q2 = animation[i]->keyframes[nextkeyframe].quaternion;*/

				t1 = animation[i]->keyframes[keyframenumber].translation;
				t2 = animation[i]->keyframes[nextkeyframe].translation;

				//quat q = glm::mix(q1, q2, mix);
				quat q = quaterpolate(animation[i], keyframenumber, mix);
				if (name != "Hips")
					tr = glm::mix(t1, t2, mix);

				mat4 M = mat4(q);
				mat4 T = translate(mat4(1), tr);
				M = T * M;
				if (mat)
				{
					mat4 parentmat = mat4(1);
					if (parent)
						parentmat = *parent->mat;
					*mat = parentmat * M;
				}
			}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->play_animation(keyframepos, animationname);
	}

	//Transition between anim1 and anim2, when finished sets anim2 equal to anim1
	void play_animation_mix(float keyframepos, string &anim1, string &anim2)
	{
		animation_per_bone * current = NULL;
		animation_per_bone * next = NULL;
		float mix;
		int keyframenumber = (int)keyframepos;
		float framemix = keyframepos - keyframenumber;
		int size;
		for (int i = 0; i < animation.size(); i++)
		{
			//Find the current animation
			if (animation[i]->name == anim1)
			{
				current = animation[i];
				size = current->keyframes.size();
				keyframenumber = keyframenumber % size;
				//Wait until we are at the end of it before beginning transition
				if (anim1 == anim2 || (keyframenumber + framemix) / (size - 1) < 0.7)
				{
					play_animation(keyframepos, anim1);
					return;
				}
			}
			//Find the next animation
			else if (animation[i]->name == anim2)
			{
				next = animation[i];
			}
			if (current != NULL && next != NULL)
			{
				int nextKeyFrame = (keyframenumber + 1) % size;
				quat q1, q2;
				vec3 t1, t2;
				vec3 tr = vec3(0, 0, 0);
				
				q1 = quaterpolate(current, keyframenumber, framemix);
				q2 = next->keyframes[0].quaternion;
					
				t1 = glm::mix(current->keyframes[keyframenumber].translation, current->keyframes[nextKeyFrame].translation, framemix);
				t2 = next->keyframes[0].translation;

				//Calculate how far we are between the two animation
				mix = (keyframenumber + framemix) / float(size - 1) * 5.0f - 3.5f;
				quat q = glm::mix(q1, q2, mix);
				if (name != "Hips") //The hips are default vec3(0,0,0)
					tr = glm::mix(t1, t2, mix);

				mat4 M = mat4(q);
				mat4 T = translate(mat4(1), tr);
				M = T * M;
				if (mat)
				{
					mat4 parentmat = mat4(1);
					if (parent)
						parentmat = *parent->mat;
					*mat = parentmat * M;
				}
				current = NULL;
				next = NULL;
			}
		}
		for (int i = 0; i < kids.size(); i++)
		{
			kids[i]->play_animation_mix(keyframepos, anim1, anim2);
		}
		//Tell the caller that we have finished the animation transition
     // printf("%s", name.c_str());
		if (name == "Hips" && mix >= 0.99f)
		{
			anim1 = anim2;
		}
	}
	//writes into the segment positions and into the animation index VBO
	void write_to_VBOs(vector<vec3> &vpos, vector<vec3> &vnorm, vector<unsigned int> &imat)
		{

		for (int i = 0; i < cylinder.size() - 3; i++)
		{
			vec3 vert = vec3(cylinder[i], cylinder[i + 1], cylinder[i + 2]);
			vec3 norm = vec3(cylinder_normals[i], cylinder_normals[i + 1], cylinder_normals[i + 2]);
			if (name == "Humanoid:Head_End" || name == "Humanoid:Head")
			{
				vec4 head = scale(mat4(1.0f), vec3(100.0f, 1.0f, 100.0f)) * vec4(vert, 0.0f);
				vert = vec3(head.x, head.y, head.z);
			}
			vpos.push_back(vert);
			vnorm.push_back(norm);
			if (parent && vert.y >= 0.0f)
				imat.push_back(parent->index);
			else
				imat.push_back(index);
		}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBOs(vpos, vnorm, imat);
		}
	//searches for the correct animations as well as sets the correct element from the animation matrix array
   void set_animations(all_animations *all_anim, mat4 *matrices, int &animsize)
   {
      for (int ii = 0; ii < all_anim->animations.size(); ii++)
      {
      
      printf("%s", name.c_str());
      if (all_anim->animations[ii].bone == name)
         animation.push_back(&all_anim->animations[ii]);
      }
		mat = &matrices[index];
		animsize++;

		for (int i = 0; i < kids.size(); i++)
			kids[i]->set_animations(all_anim, matrices, animsize);
		}

};
int readtobone(string file,all_animations *all_animation, bone **proot, string name);