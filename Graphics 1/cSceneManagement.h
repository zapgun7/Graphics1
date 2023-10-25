#pragma once

#include <vector>
#include <string>
#include "Other Graphics Stuff/cMesh.h"            // Need these two, as we're storing these objects
#include "Other Graphics Stuff/cLightManager.h"    //
//#include "Other Graphics Stuff/cGraphicsMain.h"


class cSceneManagement
{

public:
	cSceneManagement();
	~cSceneManagement();

	void Initialize();

	bool saveScene(std::string fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights);
	void loadScene(std::string fileName);

	std::vector<std::string> getAvailableSaves(); // Returns available filenames to load from

private:
	//cGraphicsMain* m_GraphicsMain; // To load info back into mesh and light objects (which are in the graphics main)
	std::string m_saveFilePath;

};