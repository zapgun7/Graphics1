#include "cSceneManagement.h"


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace rapidjson;

cSceneManagement::cSceneManagement()
{

}

cSceneManagement::~cSceneManagement()
{

}

void cSceneManagement::Initialize()
{
	//m_GraphicsMain = cGraphicsMain::getGraphicsMain();
	m_saveFilePath = "../saves/";
	// Maybe should initialize string vec to store available save states?
}

bool cSceneManagement::saveScene(std::string fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights)
{
	Document output;
	output.SetObject();

	Value meshes(kArrayType); // Big ol' array to store mesh object data
	Value meshobj(kObjectType); // Each mesh object we add to meshes
	Value string(kObjectType); // Variable to repeatedly use for strings
	Value vec(kArrayType); // Array to hold pos, orientation, etc.
	std::string str;
	glm::vec3 vec3;
	float num;
	bool state;

	for (unsigned int i = 0; i < MeshVec.size(); i++) //Iterate through all meshes
	{
		// Object filename and friendlyname
		str = MeshVec[i]->meshName; // filename
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		meshobj.AddMember("meshName", string, output.GetAllocator()); // Add meshname
		str = MeshVec[i]->friendlyName; // friendlyname
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		meshobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname
		// Pos, orientation(euler)
		vec3 = MeshVec[i]->drawPosition; // Draw position
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("drawPosition", vec, output.GetAllocator()); // Add drawPosition
		vec.SetArray(); // Clear the vec
		vec3 = MeshVec[i]->eulerOrientation; // Draw orientation
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("eulerOrientation", vec, output.GetAllocator()); // Add eulerOrientation
		vec.SetArray(); // Clear the vec
		// Scale and bools
		num = MeshVec[i]->scale; // scale
		meshobj.AddMember("scale", num, output.GetAllocator()); // Add scale
		state = MeshVec[i]->bIsVisible;
		meshobj.AddMember("isVisible", state, output.GetAllocator()); // Add isVisible
		state = MeshVec[i]->bIsWireframe;
		meshobj.AddMember("isWireframe", state, output.GetAllocator()); // Add isWireframe
		state = MeshVec[i]->bDoNotLight;
		meshobj.AddMember("doNotLight", state, output.GetAllocator()); // Add doNotLight

		meshes.PushBack(meshobj, output.GetAllocator()); // Add to array of objects
		meshobj.SetObject(); // Clear mesh object for next iteration
	}
	output.AddMember("meshes", meshes, output.GetAllocator()); // Add array of objects to root object


	// Lights time (so much data oh god; just a lotta vec4s)

	// Creating new ones for naming purposes
	Value lights(kArrayType); // Big ol' array to store light data
	Value lightobj(kObjectType); // Each mesh object we add to meshes
	glm::vec4 vec4;



	for (unsigned int i = 0; i < Lights->NUMBER_OF_LIGHTS_IM_USING; i++) // Iterate through all lights (yes even the ones we're not using)
	{
		str = Lights->theLights[i].friendlyName; // friendlyname
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		lightobj.AddMember("friendlyname", string, output.GetAllocator()); // Add friendlyname
		// Position
		vec4 = Lights->theLights[i].position;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("position", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Diffuse
		vec4 = Lights->theLights[i].diffuse;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("diffuse", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Specular
		vec4 = Lights->theLights[i].specular;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("specular", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Attenuation
		vec4 = Lights->theLights[i].atten;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("atten", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Direction
		vec4 = Lights->theLights[i].direction;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("direction", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Param1
		vec4 = Lights->theLights[i].param1;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("param1", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Param2
		vec4 = Lights->theLights[i].param2;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("param2", vec, output.GetAllocator());
		vec.SetArray(); // Clear 


		lights.PushBack(lightobj, output.GetAllocator()); // Add to array of lights
		lightobj.SetObject(); // Clear light object for next iteration
	}
	output.AddMember("lights", lights, output.GetAllocator()); // Add array of objects to root object


	std::ofstream outputFile;
	outputFile.open(m_saveFilePath + fileName + ".json"); // Will open file at this location if existing
														  // Will create a new one if not
	StringBuffer strbuf;
	PrettyWriter<StringBuffer> writer(strbuf); // Formats string... prettily

	output.Accept(writer);

	outputFile << strbuf.GetString();

	outputFile.close();

	return true;
}
// 
// 
//

void cSceneManagement::loadScene(std::string fileName)
{

}

std::vector<std::string> cSceneManagement::getAvailableSaves()
{
	std::vector<std::string> placeholder;
	return placeholder;
}
