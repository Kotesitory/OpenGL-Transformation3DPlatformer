#pragma once
#include <unordered_map>
#include <thread>
#include <mutex>
#include <deque>

#include "Chunk.h"
#include "Camera.h"

#define OUT_OF(x) if(x >= 15 || x < 0) __debugbreak();

struct hashFunc {
	inline size_t operator()(const Chunk& x) const {
		glm::vec3 pos = x.getPosition();
		size_t h1 = std::hash<float>()(pos.x);
		size_t h2 = std::hash<float>()(pos.y);
		size_t h3 = std::hash<float>()(pos.z);
		return (h1 ^ (h2 << 1)) ^ h3;
	}
};

struct equalsFunc {
	inline bool operator()(const Chunk& a, const Chunk& b) const {
		return a.getPosition().x == b.getPosition().x && a.getPosition().y == b.getPosition().y && a.getPosition().z == b.getPosition().z;
	}
};

class ChunkManager {
private:
	static std::unordered_map<glm::vec3, Chunk, hashFunc, equalsFunc> generatedChunks;
	static glm::vec3 currentChunk;
	static glm::vec3 prevChunk;
	static std::vector<glm::vec3> chunksForRender;
	static std::mutex mutex;
	static std::thread generatingThreads[9];
	static std::condition_variable cond;
	static std::deque<glm::vec3> chunksForGeneration;
	static bool stopGenerating;
	static float scale;

	ChunkManager() {}

	static void GenerateChunks();
	static glm::vec3 getChunkFromPlayerPosition(const glm::vec3& playerPosition);
	static glm::vec3 getChunkFromWorldCoords(const glm::vec3 &position);
	static std::pair<glm::vec3, glm::vec3> findVoxelLocalPosition(const glm::vec3 &voxelPos);

public:
	static void Init(const glm::vec3 &position, float scale);
	static std::pair<std::vector<float>, glm::vec3> Update(const Camera &camera, glm::vec3 start, glm::vec3 end);
	static void Stop() { 
		std::unique_lock<std::mutex> locker(mutex);
		stopGenerating = true;
		locker.unlock();
		cond.notify_all();
		for (std::thread &t : generatingThreads) {
			t.join();
		}
	}

	static glm::vec3 voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end);
};