#pragma once

#ifndef Collisions_hpp
#define Collisions_hpp

#include "Mesh.hpp"
#include "Model3D.hpp"

#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <iostream>

namespace gps {
	class Boundary {
	public:
		Boundary(gps::Model3D model);

		bool collisionDetection(glm::vec3 pos);
	private:
		float minX, maxX, minY, maxY, minZ, maxZ;
	};
}

#endif
