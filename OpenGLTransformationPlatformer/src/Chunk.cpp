#include "Chunk.h"

float lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;;
}

Chunk::Chunk(glm::vec3 position) 
	:position(position)
{
}

Chunk::~Chunk()
{

}

void Chunk::LoadChunk() 
{
	float noise, min = 0.0f, max = 0.0f;;

	for (int i = 0; i <= chunk_size; i++) {
		for (int j = 0; j <= chunk_size; j++) {
			for (int k = 0; k <= chunk_size; k++) {
				float x = Normalize(position.x + k);
				float y = Normalize(position.y + j);
				float z = Normalize(position.z + i);
				noise = simplexNoise.fractal(1, x, y, z);
				Vertex vertex = { position.x + k, position.y + j, position.z + i, noise };
				test_points.push_back(vertex);

				if (noise < min) min = noise;
				if (noise > max) max = noise;
			}
		}
	}

	surface_val = (min + max) * 0.20f;
	float surface_val_normalised = (surface_val - min) / (max - min);

	for (int i = 0; i < chunk_size; i++) {
		for (int j = 0; j < chunk_size; j++) {
			for (int k = 0; k < chunk_size; k++) {
				Vertex cube_corners[8];
				int cube_index = getCubeVertices(i, j, k, cube_corners);

				for (int l = 0; triangulation[cube_index][l] != -1; l += 3) {
					// Get indices of corner points A and B for each of the three edges
					// of the cube that need to be joined to form the triangle.
					int a0 = cornerIndexAFromEdge[triangulation[cube_index][l]];
					int b0 = cornerIndexBFromEdge[triangulation[cube_index][l]];

					int a1 = cornerIndexAFromEdge[triangulation[cube_index][l + 1]];
					int b1 = cornerIndexBFromEdge[triangulation[cube_index][l + 1]];

					int a2 = cornerIndexAFromEdge[triangulation[cube_index][l + 2]];
					int b2 = cornerIndexBFromEdge[triangulation[cube_index][l + 2]];

					glm::vec3 p1 = InterpolateVertices(cube_corners[a0], cube_corners[b0], surface_val);
					glm::vec3 p2 = InterpolateVertices(cube_corners[a1], cube_corners[b1], surface_val);
					glm::vec3 p3 = InterpolateVertices(cube_corners[a2], cube_corners[b2], surface_val);

					glm::vec3 U = p2 - p1;
					glm::vec3 V = p3 - p1;
					glm::vec3 Normal;
					Normal.x = U.y * V.z - U.z * V.y;
					Normal.y = U.z * V.x - U.x * V.z;
					Normal.z = U.x * V.y - U.y * V.x;

					addVertex(p1, Normal);
					addVertex(p2, Normal);
					addVertex(p3, Normal);
				}
			}
		}
	}
}

void Chunk::RenderChunk() {
	if (vertices.size() <= 0)
		return;

	GLCall(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));
}

int Chunk::getVoxelValue(glm::vec3 voxelPos)
{
	int result = 0;
	Vertex cube_corners[8];
	int i = voxelPos.x;
	int j = voxelPos.y;
	int k = voxelPos.z;
	return getCubeVertices(i, j, k, cube_corners);
}

int Chunk::getCubeVertices(int i, int j, int k, Vertex* vertices) {
	int chp1_sq = (chunk_size + 1) * (chunk_size + 1);
	int chp1 = chunk_size + 1;

	OUT_OF_BOUNDS((i + 1) * (chunk_size + 1) * (chunk_size + 1) + j * chp1 + k);
	OUT_OF_BOUNDS((i + 1) * (chunk_size + 1) * (chunk_size + 1) + j * chp1 + 1 + k);
	OUT_OF_BOUNDS(i * chp1_sq + j * chp1 + 1 + k);
	OUT_OF_BOUNDS(i * chp1_sq + j * chp1 + k);
	OUT_OF_BOUNDS((i + 1) * (chunk_size + 1) * (chunk_size + 1) + (j + 1) * (chunk_size + 1) + k);
	OUT_OF_BOUNDS((i + 1) * (chunk_size + 1) * (chunk_size + 1) + (j + 1) * (chunk_size + 1) + 1 + k);
	OUT_OF_BOUNDS(i * chp1_sq + (j + 1) * (chunk_size + 1) + 1 + k);
	OUT_OF_BOUNDS(i * chp1_sq + (j + 1) * (chunk_size + 1) + k);

	vertices[0] = test_points[(i + 1) * (chunk_size + 1) * (chunk_size + 1) + j * chp1 + k];
	vertices[1] = test_points[(i + 1) * (chunk_size + 1) * (chunk_size + 1) + j * chp1 + 1 + k];
	vertices[2] = test_points[i * chp1_sq + j * chp1 + 1 + k];
	vertices[3] = test_points[i * chp1_sq + j * chp1 + k];
	vertices[4] = test_points[(i + 1) * (chunk_size + 1) * (chunk_size + 1) + (j + 1) * (chunk_size + 1) + k];
	vertices[5] = test_points[(i + 1) * (chunk_size + 1) * (chunk_size + 1) + (j + 1) * (chunk_size + 1) + 1 + k];
	vertices[6] = test_points[i * chp1_sq + (j + 1) * (chunk_size + 1) + 1 + k];
	vertices[7] = test_points[i * chp1_sq + (j + 1) * (chunk_size + 1) + k];

	int result = 0;
	for (int l = 0; l < 8; l++) {
		if (vertices[l].val < surface_val) {
			result |= 1 << l;
		}
	}

	return result;
}

glm::vec3 Chunk::InterpolateVertices(Vertex a, Vertex b, float value) {
	glm::vec3 interpolation;
	float p = abs(a.val - value) / abs(b.val - a.val);
	interpolation.x = lerp(a.x, b.x, p);
	interpolation.y = lerp(a.y, b.y, p);
	interpolation.z = lerp(a.z, b.z, p);

	return interpolation;
}

void Chunk::addVertex(glm::vec3 vert, glm::vec3 normal) {
	vertices.push_back(vert.x);
	vertices.push_back(vert.y);
	vertices.push_back(vert.z);

	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);
}

float Chunk::Normalize(float value) {
	return (value - 1.0f) / (2.0f);
}

float Chunk::voxelVertices[] = {
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,

	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
};

const int Chunk::chunk_size = 15;
SimplexNoise Chunk::simplexNoise = SimplexNoise(0.15f);