#pragma once

#include "OpenGLCommon.h"
#include <glm/vec3.hpp>
#include "cMesh.h"
#include <vector>
#include <string>
#include <glad/glad.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "../cVAOManager/cVAOManager.h"
#include "../cInputHandler.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "cLightManager.h"
#include "../cSceneManagement.h"

class cGraphicsMain
{
public:
	static cGraphicsMain* getGraphicsMain(void);


	bool Update(); // return false when window is triggered to close
	void Destroy();
	//void addToDrawMesh(cMesh* newMesh);
	void removeFromDrawMesh(int ID);


private:
	cGraphicsMain();
	bool Initialize();

	cMesh* m_pFindMeshByFriendlyName(std::string friendlyNameToFind);
	void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID);
	bool LoadModels(void);

	void addNewMesh(std::string fileName, char* friendlyName); // Adding new objects from the gui
	void updateSelectedMesh(int meshIdx, std::string friendlyName, glm::vec3 newPos, glm::vec3 newOri, float newScale, bool doNotLight); // Updates and existing object by reference to its friendly name (also from the gui)
	void addNewLight(char* friendlyName);
	void updateSelectedLight(int lightIdx, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);

	void flyCameraInput(int width, int height);
	float m_FlyCamSpeed = 3.0f;

	std::vector<std::string> m_AvailableModels; // String of model file names to choose from


	glm::vec3 m_cameraEye;
	glm::vec3 m_cameraTarget;
	glm::vec3 m_cameraRotation;
	glm::vec3 m_upVector;

	cShaderManager* m_pShaderThing;
	GLuint m_shaderProgramID;


	double m_lastTime;
	GLFWwindow* m_window;
	cVAOManager* m_pMeshManager = NULL;
	std::vector< cMesh* > m_vec_pMeshesToDraw;
	cLightManager* m_pTheLights;

	// ImGui
	bool m_ShowMeshEditor;   // 
	bool m_ShowLightEditor;  // Windows to edit existing meshes and lights respectively
	bool m_ShowSceneManager; //

	ImGuiIO m_io; // ImGui io
	cInputHandler* m_InputHandler;

	cSceneManagement* m_pSceneManager;

	static cGraphicsMain* m_pTheOnlyGraphicsMain;
};