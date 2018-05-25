#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <fbxsdk.h>
using namespace std;

#define PIe 3.141592654


#include <fstream>
#include "bone.h"
using namespace glm;

/* Tab character ("\t") counter */
int numTabs = 0;

/**
* Print the required number of tabs.
*/
void PrintTabs(FILE *file) {
	for (int i = 0; i < numTabs; i++)
		fprintf(file,"\t");
}

/**
* Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	
	ifstream g;
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
* Print a node, its attributes, and all its children recursively.
*/
bone *root=NULL;
int indexcount = 0;
void PrintNode(bone *actual, FbxNode* pNode, int lastlevel)
{
	actual->index = indexcount++;
	//actual->mat = &modelmat[actual->index];

	int level = lastlevel +1;
	//cout << "bonename: " <<  pNode->GetName() << endl;
	//cout << "bone level: " << level << endl;
	
	actual->name = pNode->GetName();

	FbxDouble3 bone_vector = pNode->LclTranslation.Get();

	float t1, t2, t3;
	t1 = bone_vector[0];
	t2 = bone_vector[1];
	t3= bone_vector[2];
	//cout << "bone vector (x,y,z): " << t1 << ", " << t2 << ", " << t3 << endl;
	actual->pos.x = t1;
	actual->pos.y = t2;
	actual->pos.z = t3;
	// Get the node's default local transformation matrix.
	FbxAMatrix& lLocalTransform = pNode->EvaluateLocalTransform();

	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	float u0, u1, u2, u3;
	float e1, e2, e3;
	e1 = rotation[0];
	e2 = rotation[1];
	e3 = rotation[2];

	u0 = sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)* cos(e3*PIe / 180) + 1) / 2;
	u1 = (cos(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e3*PIe / 180) + sin(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u2 = (sin(e2*PIe / 180)*sin(e3*PIe / 180) - cos(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180) - sin(e1*PIe / 180)) / sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u3 = (sin(e2*PIe / 180)*cos(e1*PIe / 180) + sin(e2*PIe / 180)*cos(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	
	//cout << "bone quaternion (i,j,k,re): " << u1 << ", " << u2 << ", " << u3 << ", " << u0 << endl;
	actual->q.x = u1;
	actual->q.y = u2;
	actual->q.z = u3;
	actual->q.w = u0;
	


	int child_count = pNode->GetChildCount();
	//cout << "bone children count: " << child_count << endl;
	//cout << "bone children names:" << endl;
	for (int j = 0; j < pNode->GetChildCount(); j++)
		{
	
		//cout << "\t" << pNode->GetChild(j)->GetName() << endl;
		}

	//for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
	//	PrintAttribute(file,pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		bone *k = new bone;
		actual->kids.push_back(k);
		k->parent = actual;
		PrintNode(k,pNode->GetChild(j), level);
	}


}

void CountBones(FbxNode* pNode, int &count)
{
	count++;
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		CountBones(pNode->GetChild(j), count);
	}

}

void PrintAnimationData( FbxScene* lScene, bone *actual, int mode);
void CalcTransRotAnim(FbxScene* lScene, FbxNode* lNode, int animno, bone *actual, int mode)
{
	FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(animno);
	FbxString animStackName = currAnimStack->GetName();
	FbxString mAnimationName = animStackName.Buffer();
	FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	long long duration = end.GetMilliSeconds();
	int keyframecount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

	const char* nodeName = lNode->GetName();
	//cout << endl << "\t" << "bone name: " << nodeName << "index: " << actual->index << endl << endl;

	for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
	{
	FbxTime currTime;
	currTime.SetFrame(i, FbxTime::eFrames24);
	shared_ptr<keyframe> key = make_shared<keyframe>();

	long long time_ms=currTime.GetMilliSeconds();
	key->timestamp_ms = time_ms;
	//cout << "\t" << "\t" << "frame time stamp (ms): " << time_ms << endl;
	FbxDouble3 translation = lNode->EvaluateLocalTranslation(currTime);
	float t1, t2, t3;
	t1 = translation[0];
	t2 = translation[1];
	t3 = translation[2];
	//cout << "\t" << "\t" << "translation (x,y,z): " << t1 << ", " << t2 << ", " << t3 << endl;
	key->translation = vec3(t1, t2, t3);
	FbxDouble3 rotation = lNode->EvaluateLocalRotation(currTime);
	FbxAMatrix& loctraf = lNode->EvaluateLocalTransform(currTime);

	float e1, e2, e3;
	e1 = rotation[0];
	e2 = rotation[1];
	e3 = rotation[2];

	float u0, u1, u2, u3;

	u0 = sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)* cos(e3*PIe / 180) + 1) / 2;
	u1 = (cos(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e3*PIe / 180) + sin(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u2 = (sin(e2*PIe / 180)*sin(e3*PIe / 180) - cos(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180) - sin(e1*PIe / 180)) / sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u3 = (sin(e2*PIe / 180)*cos(e1*PIe / 180) + sin(e2*PIe / 180)*cos(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;

	u3 = -u3;
	u0 = -u0;

	e1 = e1*PIe / 180;
	e2 = e2*PIe / 180;
	e3 = e3*PIe / 180;

	float q0, q1, q2, q3;
	q0 = -(cos(e1 / 2)*cos(e2 / 2)*cos(e3 / 2) + sin(e1 / 2)*sin(e2 / 2)*sin(e3 / 2));
	q1 = -(sin(e1 / 2)*cos(e2 / 2)*cos(e3 / 2) - cos(e1 / 2)*sin(e2 / 2)*sin(e3 / 2));
	q2 =-( cos(e1 / 2)*sin(e2 / 2)*cos(e3 / 2) + sin(e1 / 2)*cos(e2 / 2)*sin(e3 / 2));
	q3 =-( cos(e1 / 2)*cos(e2 / 2)*sin(e3 / 2) - sin(e1 / 2)*sin(e2 / 2)*cos(e3 / 2));

	//cout << "\t" << "\t" << "quaternion (i,j,k,re): " << q1 << ", " << q2 << ", " << q3 << ", " << q0 << endl;
	key->quaternion = quat(q0, q1, q2, q3);
	if (mode == 0)
		actual->keyframes.push_back(key);
	else if (mode == 1)
		actual->anim2.push_back(key);
}
for (int k = 0; k < lNode->GetChildCount();k++)
	CalcTransRotAnim( lScene, lNode->GetChild(k), animno, actual->kids[k], mode);

}



//***************************************************************************************************************************************************************
void PrintAnimationData(FbxScene* lScene, bone *actual, int mode)
{
	int i;

	int count_animations = lScene->GetSrcObjectCount<FbxAnimStack>();
	
	////falls es keine animation gibt
	if (count_animations == 0)
	{
		cout << "no animations in file" << endl;
		return;
	}

	FbxNode* lNode = lScene->GetRootNode();
	for (int l = 0; l < count_animations; l++)
	{		
		FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(l);
		FbxString animStackName = currAnimStack->GetName();
		FbxString mAnimationName = animStackName.Buffer();
		FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
		long long duration = end.GetMilliSeconds();
		int keyframecount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

		cout << endl;
		cout << "animation name: " << mAnimationName << endl;
		cout << "key frame count: " << keyframecount << endl;
		cout << "animation duration (ms): " << duration << endl;
		for (int k = 0; k < lNode->GetChildCount(); k++)
			CalcTransRotAnim(lScene, lNode->GetChild(k), l, actual, mode);
	}
}

/**
* Main function - loads the hard-coded fbx file,
* and prints its contents in an xml format to stdout.
*/

int readtobone(bone **proot) 
{

	//ifstream fileHandle("fgdfg");
	string name_of_file;
	cout << endl << "Enter filename:" << endl;
	//getline(cin,name_of_file);
	name_of_file = "test.fbx";
	const char* lFilename = name_of_file.c_str();
	FILE *checkfile=fopen(lFilename,"rb");
	if (!checkfile)
		{
		cout << endl << "file not found!" << endl;
		return 0;
		}
	else
		cout << endl << "file exists!" << endl;
	fclose(checkfile);
	

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		FbxString error = lImporter->GetStatus().GetErrorString();
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	string create_file_path;
	
	
	///////////////////
	///	Model (Skeleton)
	/////////////////
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	FbxNode* lRootNode = lScene->GetRootNode();
	int count_bones=0;
	int child_count = lRootNode->GetChildCount();
	for (int i = 0; i < child_count; i++)//nur einen knochen machen
		CountBones(lRootNode->GetChild(i),count_bones);

	cout << endl;
	cout << "Skeleton" << endl;
	cout << endl;
	cout << "count bones: " << count_bones << endl;
	
	
		bone *root = new bone;
		*proot = root;

	if (lRootNode) 	
	{
		int anz = lRootNode->GetChildCount();
		for (int i = 0; i < lRootNode->GetChildCount(); i++)//nur einen knochen machen
			{
			PrintNode(root,lRootNode->GetChild(i), -1);
			root->kids[i]->parent = NULL;
			}			
	}

	cout << "----------------------------------------------------------------------------------------------------" << endl;
	///////////////////
	///	Animation Data 
	/////////////////
	//extract animation stacks
	/*int numStacks = lScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack));*/

	//cout << endl;
	//cout << "Animation" << endl;	
	PrintAnimationData(lScene, root, 0);

	lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	lImporter = FbxImporter::Create(lSdkManager, "");

	const char* walk = "Walking.fbx";
	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(walk, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		FbxString error = lImporter->GetStatus().GetErrorString();
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	lScene = FbxScene::Create(lSdkManager, "myScene");
	PrintAnimationData(lScene, root, 1);
	/////////////////////
	/////	End
	///////////////////
	//// Destroy the SDK manager and all the other objects it was handling.
	//lSdkManager->Destroy();
	//system("pause");
	return 0;
}



