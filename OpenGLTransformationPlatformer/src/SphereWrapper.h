#pragma once

#include "Sphere.h"

class SphereWrapper {
private:
	std::vector<float> Vertices;
public:
	SphereWrapper(const Sphere &sphere) {
		const float* vertices = sphere.getVertices();
		const float* normals = sphere.getNormals();
		const unsigned int* indices = sphere.getIndices();
		for (int i = 0; i < sphere.getIndexCount(); i++) {
			int index = indices[i] * 3;
			Vertices.push_back(vertices[index]);
			Vertices.push_back(vertices[index + 1]);
			Vertices.push_back(vertices[index + 2]);

			Vertices.push_back(normals[index]);
			Vertices.push_back(normals[index + 1]);
			Vertices.push_back(normals[index + 2]);
		}
	}

	std::vector<float> getVertices() {
		return Vertices;
	}
};