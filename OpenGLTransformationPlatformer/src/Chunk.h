#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "LookupTables.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "SimplexNoise.h"

#define OUT_OF_BOUNDS(x) if(x < 0 || x >= 4096) __debugbreak();

struct Vertex {
	float x;
	float y;
	float z;
	float val;
};



class Chunk {
private:
	glm::vec3 position;
	std::vector<Vertex> test_points;
	std::vector<float> vertices;
	float surface_val;
	static SimplexNoise simplexNoise;
	static float voxelVertices[];

	float Normalize(float value);
	int getCubeVertices(int i, int j, int k, Vertex* vertices);
	glm::vec3 InterpolateVertices(Vertex a, Vertex b, float value);
	void addVertex(glm::vec3 vert, glm::vec3 normal);
	

public:
	const static int chunk_size;
	
	Chunk(glm::vec3 position);
	~Chunk();

	size_t operator()(void) const {}
	bool operator()(const Chunk &other) const {}

	void LoadChunk();
	void RenderChunk();
	std::vector<float> getVertices() { return vertices; }
	int getNumberOfVerticies() { return vertices.size(); }
	glm::vec3 getPosition() const { return position; }
	int getVoxelValue(glm::vec3 voxelPos);
	void translate(glm::vec3 translation) { 
		position += translation; 
		int a = 2; 
	}
};