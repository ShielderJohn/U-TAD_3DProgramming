#ifndef UGINE_VERTEX_H
#define UGINE_VERTEX_H

#include "types.h"

struct Vertex {
	Vertex() : mPosition(0,0,0), mTexCoords(0,0) {}
	Vertex(const glm::vec3& position, const glm::vec2& texCoords) : mPosition(position), mTexCoords(texCoords) {}
	bool operator==(const Vertex& other) const { return mPosition == other.mPosition && mTexCoords == other.mTexCoords; }

	glm::vec3 mPosition;
	glm::vec2 mTexCoords;
	glm::vec3 mNormals;

	glm::vec4 mBoneIndices;
	glm::vec4 mBoneWeights;
};

#endif // UGINE_VERTEX_H
