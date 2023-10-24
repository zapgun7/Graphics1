#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// This is where we are going to head...
struct sPhsyicsProperties
{
	sPhsyicsProperties()
	{
		this->position = glm::vec3(0.0f);
		this->velocity = glm::vec3(0.0f);
		this->acceleration = glm::vec3(0.0f);
		this->inverse_mass = 1.0f;	// What should this be??
	}

//	typeOfSimplePhyiscThingy = "Sphere"

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	// Since division is "slow" and we are dividing my mass, 
	// Could make this immovable by making this 0 (infinite mass)
	float inverse_mass;	//	float mass;		

	// Rotational
//	glm::vec3 momentOfWhatever;
//	glm::vec3 rotationalSpeed;
};


class cMesh
{
public:
	cMesh();		// Called on creation   c'tor
	~cMesh();		// Called on deletion   d'tor

	std::string meshName;

	std::string friendlyName;		// "Ground"
	int uniqueID;

	// Draw loop uses this: 
	glm::vec3 drawPosition;				
	glm::vec3 eulerOrientation;
	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ);
	void setDrawOrientation(const glm::vec3& newOrientation);

	void adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust);
	glm::quat get_qOrientation(void);
	glm::vec3 getEulerOrientation(void); // Returns euler orientation

private:
	glm::quat m_qOrientation; // Hiding this
public:
	float scale;

	bool bIsVisible;

	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;

	// Physics properties
	// Physics update this:
	sPhsyicsProperties* pPhysProps;

	bool bIsWireframe;
	bool bDoNotLight;

	void Update(double deltaTime);

};

#endif

