#include "ChunkManager.h"
#include <iostream>
std::unordered_map<glm::vec3, Chunk, hashFunc, equalsFunc> ChunkManager::generatedChunks;
glm::vec3 ChunkManager::currentChunk;
glm::vec3 ChunkManager::prevChunk;
std::vector<glm::vec3> ChunkManager::chunksForRender;
std::mutex ChunkManager::mutex;
std::condition_variable ChunkManager::cond;
std::thread ChunkManager::generatingThreads[9];
std::deque<glm::vec3> ChunkManager::chunksForGeneration;
bool ChunkManager::stopGenerating = false;
float ChunkManager::scale;

bool CompareChunks(const glm::vec3 &a, const glm::vec3 &b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void ChunkManager::Init(const glm::vec3 &position, float scale)
{
	ChunkManager::scale = scale;
	for (std::thread &t : generatingThreads) {
		t = std::thread(GenerateChunks);
	}

	currentChunk = getChunkFromPlayerPosition(position);
	for (int i = 2; i >= -2; i--) {
		for (int j = 2; j >= -2; j--) {
			glm::vec3 position(currentChunk.x + j * Chunk::chunk_size, currentChunk.y, currentChunk.z + i * Chunk::chunk_size);
			Chunk chunk(position);
			chunk.LoadChunk();
			generatedChunks.insert(std::pair<glm::vec3, Chunk>(chunk.getPosition(), chunk));
			chunksForRender.push_back(position);
			//std::unique_lock<std::mutex> locker(mutex);
			//chunksForGeneration.push_front(position);
			//locker.unlock();
			//cond.notify_all();
		}
	}
	
	prevChunk = currentChunk;
}

std::pair<std::vector<float>, glm::vec3> ChunkManager::Update(const Camera &camera, glm::vec3 start, glm::vec3 end)
{
	currentChunk = getChunkFromPlayerPosition(camera.Position);
	std::vector<float> points;
	if (!CompareChunks(currentChunk,prevChunk)) {
		glm::vec3 translation = (currentChunk - prevChunk);
		for (auto it = chunksForRender.begin(); it != chunksForRender.end(); ++it){
			glm::vec3 &c = *it;
			c += translation;
			auto iter = generatedChunks.find(c);
			if (iter == generatedChunks.end()) {
				std::unique_lock<std::mutex> locker(mutex);
				chunksForGeneration.push_front(c);
				locker.unlock();
				cond.notify_all();	
			}
		}
	}

	glm::vec3 crosshair = voxel_traversal(start, end) + 0.5f;

	for (glm::vec3 c : chunksForRender) {
		std::unique_lock<std::mutex> locker(mutex);
		auto iter = generatedChunks.find(c);
		if (iter != generatedChunks.end()) {
			locker.unlock();
			Chunk chunk = iter->second;
			std::vector<float> a = chunk.getVertices();
			points.insert(points.end(), a.begin(), a.end());
		}
		else {
			locker.unlock();
		}
	}

	prevChunk = currentChunk;
	return std::pair<std::vector<float>, glm::vec3>(points, crosshair);
}

void ChunkManager::GenerateChunks()
{
	glm::vec3 position;
	while (true) {
		std::unique_lock<std::mutex> locker(mutex);
		cond.wait(locker, []() { return !chunksForGeneration.empty() || stopGenerating; });
		if (stopGenerating)
			break;

		position = chunksForGeneration.back();
		chunksForGeneration.pop_back();
		locker.unlock();
		Chunk chunk(position);
		chunk.LoadChunk();
		locker.lock();
		generatedChunks.insert(std::pair<glm::vec3, Chunk>(chunk.getPosition(), chunk));
		locker.unlock();
	}
}

glm::vec3 ChunkManager::getChunkFromPlayerPosition(const glm::vec3 &playerPosition)
{
	glm::vec3 chunkPosition;
	chunkPosition.x = ((int)playerPosition.x - (playerPosition.x > 0 ? 0 : Chunk::chunk_size * 15)) / (Chunk::chunk_size * 15) * Chunk::chunk_size;
	chunkPosition.y = 0.0f;
	chunkPosition.z = ((int)playerPosition.z - (playerPosition.z > 0 ? 0 : Chunk::chunk_size * 15)) / (Chunk::chunk_size * 15) * Chunk::chunk_size;
	return chunkPosition;
}

glm::vec3 ChunkManager::getChunkFromWorldCoords(const glm::vec3 &position) {
	glm::vec3 chunkPosition;
	chunkPosition.x = ((int)position.x - ((int)position.x > 0 ? 0 : Chunk::chunk_size)) / Chunk::chunk_size * Chunk::chunk_size;
	chunkPosition.y = ((int)position.y - ((int)position.y > 0 ? 0 : Chunk::chunk_size)) / Chunk::chunk_size * Chunk::chunk_size;
	chunkPosition.z = ((int)position.z - ((int)position.z > 0 ? 0 : Chunk::chunk_size)) / Chunk::chunk_size * Chunk::chunk_size;
	return chunkPosition;
}

std::pair<glm::vec3, glm::vec3> ChunkManager::findVoxelLocalPosition(const glm::vec3 &voxelPos)
{
	glm::vec3 chunkPos = getChunkFromWorldCoords(voxelPos);
	glm::vec3 localVoxelPos = voxelPos - chunkPos;
	localVoxelPos = glm::clamp(glm::floor(localVoxelPos), glm::vec3(0.0f), glm::vec3(14.0f));
	OUT_OF(localVoxelPos.x);
	OUT_OF(localVoxelPos.y);
	OUT_OF(localVoxelPos.z);

	return std::pair<glm::vec3, glm::vec3>(chunkPos, localVoxelPos);
}

/**
 * @brief returns all the voxels that are traversed by a ray going from start to end
 * @param start : continous world position where the ray starts
 * @param end   : continous world position where the ray end
 * @return vector of voxel ids hit by the ray in temporal order
 *
 * J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
 */

glm::vec3 ChunkManager::voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end) {
	float _bin_size = 1;
	// This id of the first/current voxel hit by the ray.
	// Using floor (round down) is actually very important,
	// the implicit int-casting will round up for negative numbers.
	glm::vec3 current_voxel(std::floor(ray_start.x / _bin_size),
		std::floor(ray_start.y / _bin_size),
		std::floor(ray_start.z / _bin_size));

	// The id of the last voxel hit by the ray.
	// TODO: what happens if the end point is on a border?
	glm::vec3 last_voxel(std::floor(ray_end.x / _bin_size),
		std::floor(ray_end.y / _bin_size),
		std::floor(ray_end.z / _bin_size));

	// Compute normalized ray direction.
	glm::vec3 ray = ray_end - ray_start;
	//ray.normalize();

	// In which direction the voxel ids are incremented.
	double stepX = (ray.x >= 0) ? 1 : -1; // correct
	double stepY = (ray.y >= 0) ? 1 : -1; // correct
	double stepZ = (ray.z >= 0) ? 1 : -1; // correct

	// Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
	double next_voxel_boundary_x = (current_voxel.x + stepX)*_bin_size; // correct
	double next_voxel_boundary_y = (current_voxel.y + stepY)*_bin_size; // correct
	double next_voxel_boundary_z = (current_voxel.z + stepZ)*_bin_size; // correct

	// tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
	// the value of t at which the ray crosses the first vertical voxel boundary
	double tMaxX = (ray.x != 0) ? (next_voxel_boundary_x - ray_start.x) / ray.x : FLT_MAX; //
	double tMaxY = (ray.y != 0) ? (next_voxel_boundary_y - ray_start.y) / ray.y : FLT_MAX; //
	double tMaxZ = (ray.z != 0) ? (next_voxel_boundary_z - ray_start.z) / ray.z : FLT_MAX; //

	// tDeltaX, tDeltaY, tDeltaZ --
	// how far along the ray we must move for the horizontal component to equal the width of a voxel
	// the direction in which we traverse the grid
	// can only be FLT_MAX if we never go in that direction
	double tDeltaX = (ray.x != 0) ? _bin_size / ray.x * stepX : FLT_MAX;
	double tDeltaY = (ray.y != 0) ? _bin_size / ray.y * stepY : FLT_MAX;
	double tDeltaZ = (ray.z != 0) ? _bin_size / ray.z * stepZ : FLT_MAX;

	glm::vec3 diff(0, 0, 0);
	bool neg_ray = false;
	if (current_voxel.x != last_voxel.x && ray.x < 0) { diff.x--; neg_ray = true; }
	if (current_voxel.y != last_voxel.y && ray.y < 0) { diff.y--; neg_ray = true; }
	if (current_voxel.z != last_voxel.z && ray.z < 0) { diff.z--; neg_ray = true; }
	if (neg_ray) {
		current_voxel += diff;
	}

	bool collision = false;
	while (last_voxel != current_voxel && !collision) {
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				current_voxel.x += stepX;
				tMaxX += tDeltaX;
			}
			else {
				current_voxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		else {
			if (tMaxY < tMaxZ) {
				current_voxel.y += stepY;
				tMaxY += tDeltaY;
			}
			else {
				current_voxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}

		std::pair<glm::vec3, glm::vec3> result = findVoxelLocalPosition(current_voxel);
		int value = 0;
		std::unique_lock<std::mutex> locker(mutex);
		auto iter = generatedChunks.find(result.first);
		if (iter != generatedChunks.end()) {
			value = iter->second.getVoxelValue(result.second);
		}
		locker.unlock();
		if (value > 0) {
			collision = true;
			return current_voxel;
		}
		//visited_voxels.push_back(current_voxel);
	}
	return glm::vec3(10000.0f);
}