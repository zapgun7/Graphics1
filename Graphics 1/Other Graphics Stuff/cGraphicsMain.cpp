#include "cGraphicsMain.h"

#include<iostream>


#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "GLWF_CallBacks.h"


cGraphicsMain* cGraphicsMain::m_pTheOnlyGraphicsMain = NULL;



static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


cGraphicsMain* cGraphicsMain::getGraphicsMain(void) // Making graphics main a singleton
{
	if (cGraphicsMain::m_pTheOnlyGraphicsMain == NULL)
	{
		cGraphicsMain::m_pTheOnlyGraphicsMain = new cGraphicsMain();
		if (!cGraphicsMain::m_pTheOnlyGraphicsMain->Initialize())
		{
			cGraphicsMain::m_pTheOnlyGraphicsMain->Destroy();
		}
	}
	return cGraphicsMain::m_pTheOnlyGraphicsMain;
}

cGraphicsMain::cGraphicsMain()
{
	m_cameraEye = glm::vec3(0.0, 0.0f, 100.0f);
	m_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	m_cameraRotation = glm::vec3(0.0, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ShowLightEditor = false;
	m_ShowMeshEditor = false;
	//m_io = ImGui::GetIO();
}

bool cGraphicsMain::Initialize()
{
	m_InputHandler = new cInputHandler();


	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	m_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetKeyCallback(m_window, key_callback);

	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	m_pShaderThing = new cShaderManager();
	m_pShaderThing->setBasePath("assets/shaders");

	cShaderManager::cShader vertexShader;
	vertexShader.fileName = "vertexShader01.glsl";

	cShaderManager::cShader fragmentShader;
	fragmentShader.fileName = "fragmentShader01.glsl";

	if (!m_pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader))
	{
		std::cout << "Error: Couldn't compile or link:" << std::endl;
		std::cout << m_pShaderThing->getLastError();
		return -1;
	}


	//
	m_shaderProgramID = m_pShaderThing->getIDFromFriendlyName("shader01");

	m_pMeshManager = new cVAOManager();

	m_pMeshManager->setBasePath("assets/models");

	m_pTheLights = new cLightManager();


	// MODEL LOADING /////////////////

	LoadModels();


	// Initialize lights here if ya want em
	m_lastTime = glfwGetTime();

	//ImGui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = ImGui::GetIO(); (void)m_io;
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	// Our state
	//bool show_credits_window = false;


	return 1;
}






bool cGraphicsMain::Update() // Main "loop" of the window. Not really a loop, just gets called every tick
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	// Check input for camera movement
	m_InputHandler->queryKeys(m_window);


	// ---------------------------IMGUI WINDOWS---------------------------------------//
	{
		ImGui::Begin("Main Editor Window");

		static int available_obj_idx = 0;
		if (ImGui::BeginListBox("Available Objects"))
		{
			for (int n = 0; n < m_AvailableModels.size(); n++)
			{
				const bool is_selected = (available_obj_idx == n);
				if (ImGui::Selectable(m_AvailableModels[n].c_str(), is_selected))
					available_obj_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		static char buf1[32] = ""; ImGui::InputText("Object Name", buf1, 32);
		ImGui::SameLine();
		if (ImGui::Button("AddObject")) // Button to add new object to the scene
		{
			if (std::strlen(buf1) > 0)
			{
				addNewMesh(m_AvailableModels[available_obj_idx], buf1);
				memset(buf1, 0, 32);
			}
		}


		if (ImGui::Button("Mesh Editor"))
		{
			if (m_ShowMeshEditor)
				m_ShowMeshEditor = false;
			else
				m_ShowMeshEditor = true;
		}



		ImGui::End();
	}


	if (m_ShowMeshEditor)
	{
		ImGui::Begin("Mesh Editor");

		static int mesh_obj_idx = 0;
		if (ImGui::BeginListBox("Available Objects"))
		{
			for (int n = 0; n < m_vec_pMeshesToDraw.size(); n++)
			{
				const bool is_selected = (mesh_obj_idx == n);
				if (ImGui::Selectable(m_vec_pMeshesToDraw[n]->friendlyName.c_str(), is_selected))
					mesh_obj_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		bool isExistingMesh = true;
		if (m_vec_pMeshesToDraw.size() > 0)
			isExistingMesh = false;
		float xPos = 0;
		float yPos = 0;
		float zPos = 0;
		float xOri = 0;
		float yOri = 0;
		float zOri = 0;
		float scale = 0;
		if (!isExistingMesh)
		{
			xPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.x;
			yPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.y;
			zPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.z;
			glm::vec3 meshEulerOri = m_vec_pMeshesToDraw[mesh_obj_idx]->getEulerOrientation();
			xOri = meshEulerOri.x;
			yOri = meshEulerOri.y;
			zOri = meshEulerOri.z;
			scale = m_vec_pMeshesToDraw[mesh_obj_idx]->scale;
		}

		ImGui::SeparatorText("Position");
		ImGui::DragFloat("X", &xPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y", &yPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z", &zPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		ImGui::SeparatorText("Orientation");
		ImGui::DragFloat("X-Rotation", &xOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y-Rotation", &yOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z-Rotation", &zOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		ImGui::SeparatorText("Scale");
		ImGui::DragFloat("Scale", &scale, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		
		// List all object attributes the user is able to edit

		if (!isExistingMesh)
		{
			glm::vec3 newPos = glm::vec3(xPos, yPos, zPos);
			glm::vec3 newOri = glm::vec3(xOri, yOri, zOri);
			updateSelectedMesh(mesh_obj_idx, "A NEW FRIENDLY NAME", newPos, newOri, scale);
		}
		ImGui::End();
	}






	ImGui::Render();
	// ---------------------------IMGUI END-------------------------------------------//


	float ratio;
	int width, height;

	glUseProgram(m_shaderProgramID);

	glfwGetFramebufferSize(m_window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glCullFace(GL_BACK);


	// *****************************************************************
	// if ya want lights
	m_pTheLights->UpdateUniformValues(m_shaderProgramID);


	// *****************************************************************
			//uniform vec4 eyeLocation;

	flyCameraInput(width, height); // UPDATE CAMERA STATS

	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		m_cameraEye.x, m_cameraEye.y, m_cameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.6f,
		ratio,
		0.1f,
		1000.0f);



	//glm::quat povRotation = glm::quat(glm::vec3(m_cameraForwardRotation));


	//m_cameraTarget = m_cameraEye + m_cameraTarget;
	//std::cout << "X: " << m_cameraTarget.x << " Y: " << m_cameraTarget.y << " Z: " << m_cameraTarget.z << std::endl;
	
	glm::mat4 matView = glm::lookAt(m_cameraEye,
		m_cameraEye + m_cameraTarget,
		m_upVector);

	//glm::quat povRotation = glm::quat(m_cameraForwardRotation);
	//matView *= glm::mat4(povRotation);

	GLint matProjection_UL = glGetUniformLocation(m_shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(m_shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	// *********************************************************************
	// Draw all the objects
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++) // Prob black or smthn
	{
		cMesh* pCurrentMesh = m_vec_pMeshesToDraw[index];

		if (pCurrentMesh->bIsVisible)
		{

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}//if (pCurrentMesh->bIsVisible)

	}//for ( unsigned int index

	// Time per frame (more or less)
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - m_lastTime;
	//        std::cout << deltaTime << std::endl;
	m_lastTime = currentTime;

	glfwPollEvents();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

	if (glfwWindowShouldClose(m_window))
		return -1;
	else
		return 0;
}



void cGraphicsMain::Destroy()
{

	glfwDestroyWindow(m_window);
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	exit(EXIT_SUCCESS);
}

// Creates a new mesh to bind to an object; returns a pointer which the arena can associate to specific objects to animate them


// void cGraphicsMain::addToDrawMesh(cMesh* newMesh)
// {
// 	m_vec_pMeshesToDraw.push_back(newMesh);
// 	return;
// }

void cGraphicsMain::removeFromDrawMesh(int ID)
{
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
	{
		if (m_vec_pMeshesToDraw[i]->uniqueID == ID)
		{
			m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + i);
			return;
		}
	}
}

cMesh* cGraphicsMain::m_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++)
	{
		if (m_vec_pMeshesToDraw[index]->friendlyName == friendlyNameToFind)
		{
			// Found it
			return m_vec_pMeshesToDraw[index];
		}
	}
	// Didn't find it
	return NULL;
}

void cGraphicsMain::DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{

	//         mat4x4_identity(m);
	glm::mat4 matModel = matModelParent;



	// Translation
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		glm::vec3(pCurrentMesh->drawPosition.x,
			pCurrentMesh->drawPosition.y,        
			pCurrentMesh->drawPosition.z));


	// Rotation matrix generation
// 	glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.x, // (float)glfwGetTime(),
// 		glm::vec3(1.0f, 0.0, 0.0f));
// 
// 
// 	glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.y, // (float)glfwGetTime(),
// 		glm::vec3(0.0f, 1.0, 0.0f));
// 
// 	glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.z, // (float)glfwGetTime(),
// 		glm::vec3(0.0f, 0.0, 1.0f));


	// Quaternion Rotation
	glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


	// Scaling matrix
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		glm::vec3(pCurrentMesh->scale,
			pCurrentMesh->scale,
			pCurrentMesh->scale));
	//--------------------------------------------------------------

	// Combine all these transformation
	matModel = matModel * matTranslate;

// 	matModel = matModel * matRotateX;
// 	matModel = matModel * matRotateY;
// 	matModel = matModel * matRotateZ;

	matModel = matModel * matRotation;

	matModel = matModel * matScale;

	//        m = m * rotateZ;
	//        m = m * rotateY;
	//        m = m * rotateZ;



	   //mat4x4_mul(mvp, p, m);
	//    glm::mat4 mvp = matProjection * matView * matModel;

	//    GLint mvp_location = glGetUniformLocation(shaderProgramID, "MVP");
	//    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));


	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

	// uniform mat4 matModel_IT;
	GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
	glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));


	if (pCurrentMesh->bIsWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//        glPointSize(10.0f);


			// uniform bool bDoNotLight;
	GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

	if (pCurrentMesh->bDoNotLight)
	{
		// Set uniform to true
		glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
	}
	else
	{
		// Set uniform to false;
		glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
	}

	//uniform bool bUseDebugColour;	
	GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");
	if (pCurrentMesh->bUseDebugColours)
	{
		glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
		//uniform vec4 debugColourRGBA;
		GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
		glUniform4f(debugColourRGBA_UL,
			pCurrentMesh->wholeObjectDebugColourRGBA.r,
			pCurrentMesh->wholeObjectDebugColourRGBA.g,
			pCurrentMesh->wholeObjectDebugColourRGBA.b,
			pCurrentMesh->wholeObjectDebugColourRGBA.a);
	}
	else
	{
		glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
	}



	sModelDrawInfo modelInfo;
	if (m_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
	{
		// Found it!!!

		glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
		glDrawElements(GL_TRIANGLES,
			modelInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0);
		glBindVertexArray(0); 			            // disable VAO (and everything else)

	}

	return;
}

// Loads in all models that are available to us into the VAO
bool cGraphicsMain::LoadModels(void)
{
	sModelDrawInfo modelDrawingInfo;
	m_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba.ply",
		modelDrawingInfo, m_shaderProgramID);
	std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
	m_AvailableModels.push_back("bathtub_xyz_n_rgba.ply");

	m_pMeshManager->LoadModelIntoVAO("legospiderman_head_xyz_n_rgba.ply",
		modelDrawingInfo, m_shaderProgramID);
	std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
	m_AvailableModels.push_back("legospiderman_head_xyz_n_rgba.ply");

	m_pMeshManager->LoadModelIntoVAO("Sphere_1_unit_Radius.ply",
		modelDrawingInfo, m_shaderProgramID);
	std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
	m_AvailableModels.push_back("Sphere_1_unit_Radius.ply");

	m_pMeshManager->LoadModelIntoVAO("Terrain_xyz_n_rgba.ply",
		modelDrawingInfo, m_shaderProgramID);
	std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
	m_AvailableModels.push_back("Terrain_xyz_n_rgba.ply");




	return true;
}

// Adds new object to the meshestodraw
void cGraphicsMain::addNewMesh(std::string fileName, char* friendlyName)
{
	cMesh* meshToAdd = new cMesh();
	meshToAdd->meshName = fileName; // Set object type
	meshToAdd->friendlyName = friendlyName;
	meshToAdd->bDoNotLight = true;
	

	m_vec_pMeshesToDraw.push_back(meshToAdd);
	return;
}

// Updates values of selected object from the gui
void cGraphicsMain::updateSelectedMesh(int meshIdx, std::string friendlyName, glm::vec3 newPos, glm::vec3 newOri, float newScale) // Will need to pass in a lot more info
{
	m_vec_pMeshesToDraw[meshIdx]->drawPosition = newPos;
	//m_vec_pMeshesToDraw[meshIdx]->setRotationFromEuler(newOri);
	glm::vec3 oldOri = m_vec_pMeshesToDraw[meshIdx]->getEulerOrientation();
	glm::vec3 deltaOri = newOri - oldOri;
	m_vec_pMeshesToDraw[meshIdx]->adjustRotationAngleFromEuler(deltaOri);
	//m_vec_pMeshesToDraw[meshIdx]->adjustRotationAngleFromEuler(glm::vec3(0.0f, 0.0f, 0.01f));
	m_vec_pMeshesToDraw[meshIdx]->scale = newScale;
}

void cGraphicsMain::flyCameraInput(int width, int height)
{
	static bool isRightClicking = false;
	static double mouseXPos = 0;
	static double mouseYPos = 0;

	int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS) // Start tracking delta mouse position
	{
		if (!isRightClicking) // start tracking
		{
			mouseXPos = width / 2;
			mouseYPos = height / 2;
			//glfwSetCursorPos(m_window, width / 2, height / 2);
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//glfwGetCursorPos(m_window, &mouseXPos, &mouseYPos);
			isRightClicking = true;
		}
		else
		{
			double deltaMouseX, deltaMouseY;
			glfwGetCursorPos(m_window, &deltaMouseX, &deltaMouseY); // Get current mouse position
			deltaMouseX -= mouseXPos; // Set the delta mouse positions
			deltaMouseY -= mouseYPos; // for this tick

			// Camera rotation here :)
			m_cameraRotation.x -= deltaMouseX / 1000;

			m_cameraRotation.y -= deltaMouseY / 1000;
			m_cameraTarget.y = m_cameraRotation.y;     // This is pitch
			m_cameraTarget.x = sin(m_cameraRotation.x);         // This is just y-rotation
			m_cameraTarget.z = sin(m_cameraRotation.x + 1.57);  //
			m_cameraTarget = glm::normalize(m_cameraTarget);
			m_cameraTarget.y *= 2;
			glfwSetCursorPos(m_window, width / 2, height / 2);
			glfwGetCursorPos(m_window, &mouseXPos, &mouseYPos); // Update this for next loop
		}

	}
	else if (isRightClicking)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isRightClicking = false;
	}

	if (isRightClicking) // Have movement tied to right-clicking too
	{
		state = glfwGetKey(m_window, GLFW_KEY_W);
		if (state == GLFW_PRESS) // Move forward
		{
			m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_S);
		if (state == GLFW_PRESS) // Move backwards
		{
			m_cameraEye -= glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_A);
		if (state == GLFW_PRESS) // Move left
		{
			m_cameraEye += glm::normalize(glm::cross(m_upVector, m_cameraTarget)) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_D);
		if (state == GLFW_PRESS) // Move right
		{
			m_cameraEye -= glm::normalize(glm::cross(m_upVector, m_cameraTarget)) * m_FlyCamSpeed;
		}
	}
}
