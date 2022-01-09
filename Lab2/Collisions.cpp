#include "Collisions.hpp"

namespace gps {
	Boundary::Boundary(gps::Model3D model) {
		minX = INT_MAX; maxX = INT_MIN;
		minY = INT_MAX; maxY = INT_MIN;
		minZ = INT_MAX; maxZ = INT_MIN;

		for (gps::Mesh mesh : model.getMeshes()) {
			for (gps::Vertex vert : mesh.vertices) {
				if (vert.Position.x < minX) {
					minX = vert.Position.x;
				}
				else if (vert.Position.x > maxX) {
					maxX = vert.Position.x;
				}

				if (vert.Position.y < minY) {
					minY = vert.Position.y;
				}
				else if (vert.Position.y > maxY) {
					maxY = vert.Position.y;
				}

				if (vert.Position.z < minZ) {
					minZ = vert.Position.z;
				}
				else if (vert.Position.z > maxZ) {
					maxZ = vert.Position.z;
				}
			}
		}
	}

	bool Boundary::collisionDetection (glm::vec3 pos) {
		if (pos.x < minX || pos.x > maxX) {
			return false;
		}

		if (pos.y < minY || pos.y > maxY) {
			return false;
		}

		if (pos.z < minZ || pos.z > maxZ) {
			return false;
		}

		return true;
	}
}