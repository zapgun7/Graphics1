#include "cMesh.h"
#include <iostream>			// For KillAllHumans() cout

#ifdef _DEBUG
//#include "cGlobal.h"	//cDebugRenderer* 

#endif


// Constructor: Called on creation   c'tor
cMesh::cMesh()
{
	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->eulerOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
	this->scale = 1.0f;

	this->bIsWireframe = false;
	this->bDoNotLight = false;

	this->bIsVisible = true;

	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// If it's NULL or 0, then it's ignored by the physics loop
	this->pPhysProps = NULL;		// or 0 or nullptr

}

// Destructor: Called on deletion   d'tor
cMesh::~cMesh()
{

}

void cMesh::setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
{
	this->m_qOrientation = glm::quat(newEulerAngleXYZ);
	eulerOrientation = newEulerAngleXYZ;
}

void cMesh::setDrawOrientation(const glm::vec3& newOrientation)
{
	this->setRotationFromEuler(newOrientation);
	return;
}

void cMesh::adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust)
{
	eulerOrientation += EulerAngleXYZ_Adjust;
	glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
	this->m_qOrientation *= qChange;
}

glm::quat cMesh::get_qOrientation(void)
{
	return m_qOrientation;
}

glm::vec3 cMesh::getEulerOrientation(void)
{
	return eulerOrientation;
	// Weird outcomes, so we just return stored euler version
	//return glm::eulerAngles(this->get_qOrientation());
}

//void cMesh::KillAllHumans(void)
//{
//	std::cout << "Kill all humans!" << std::endl;
//	return;
//}


void cMesh::Update(double deltaTime)
{
#ifdef _DEBUG
//	::g_pDebugRenderer->AddSphere();
#endif
	return;
}