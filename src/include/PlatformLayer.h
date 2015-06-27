#pragma once

#include "Renderer.h"

//Pure Virtual Class for the Platform Layer.
//Each platform should extend this class and provide all the platform specific code
class PlatformLayer {
public:
	//This should initialize the platform layer, including all the platform specific subsystems.
	virtual void Initialize() = 0;

	//UpdateGame
	virtual bool Update(float DeltaTime) = 0;

	//This should return a platform specific renderer
	virtual Renderer* GetRenderer() = 0;
	
	//This should return a PlatformLayer pointer suitable for the current platform
	static PlatformLayer* PlatformLayerFactory() {

	}
};