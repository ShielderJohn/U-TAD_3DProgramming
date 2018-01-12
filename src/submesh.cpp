#include "../include/submesh.h"

Submesh::Submesh(Ptr<Texture> tex) {
	mTexture = tex;
	mDiffColor = glm::vec3(1, 1, 1);
	mShininess = 64;

	mVertexBuffer = Renderer::Instance()->CreateBuffer();
	mIndexBuffer = Renderer::Instance()->CreateBuffer();
}

Submesh::~Submesh() {
	Renderer::Instance()->FreeBuffer(mVertexBuffer);
	Renderer::Instance()->FreeBuffer(mIndexBuffer);
	mVertices.Clear();
	mIndices.Clear();
}

Ptr<Submesh> Submesh::Create(Ptr<Texture> tex) {
	return new Submesh(tex);
}

void Submesh::AddVertex(const Vertex& v) {
	mVertices.Add(v);
}

void Submesh::AddTriangle(uint32 v0, uint32 v1, uint32 v2) {
	mIndices.Add(v0);
	mIndices.Add(v1);
	mIndices.Add(v2);
}

Ptr<Texture> Submesh::GetTexture() const {
	return mTexture;
}

void Submesh::SetTexture(Ptr<Texture> tex) {
	mTexture = tex;
}

const Array<Vertex>& Submesh::GetVertices() const {
	return mVertices;
}

Array<Vertex>& Submesh::GetVertices() {
	return mVertices;
}

const glm::vec3& Submesh::GetColor() const {
	return mDiffColor;
}

void Submesh::SetColor(const glm::vec3& color) {
	mDiffColor = color;
}

uint8 Submesh::GetShininess() const {
	return mShininess;
}

void Submesh::SetShininess(uint8 shininess) {
	mShininess = shininess;
}

void Submesh::Rebuild() {
	Renderer::Instance()->SetVertexBufferData(mVertexBuffer, &mVertices[0], mVertices.Size() * sizeof(Vertex));
	Renderer::Instance()->SetIndexBufferData(mIndexBuffer, &mIndices[0], mIndices.Size() * sizeof(uint16));
}

void Submesh::Render() {
	if (mTexture != nullptr) {
		Renderer::Instance()->SetTexture(mTexture->GetHandle());
		Renderer::Instance()->EnableTexture(true);
	}
	else {
		Renderer::Instance()->SetTexture(0);
		Renderer::Instance()->EnableTexture(false);
	}

	Renderer::Instance()->SetDiffuse(mDiffColor);
	Renderer::Instance()->SetShininess(mShininess);
	Renderer::Instance()->DrawBuffers(mVertexBuffer, mIndexBuffer, mIndices.Size());
	Renderer::Instance()->EnableTexture(false);
}