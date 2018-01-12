#include "../include/model.h"
#include "../include/mesh.h"

Model::Model(Ptr<Mesh> mesh) {
	mMesh = mesh;
	mAnimating = false;
	mFPS = 0;
	mCurrentFrame = 0;

	for (int i = 0; i < mMesh->GetBones().Size(); i++) {
		mAnimMatrices.Add(glm::mat4());
	}
}

Ptr<Model> Model::Create(Ptr<Mesh> mesh) {
	if (mesh == nullptr) {
		return nullptr;
	}
	else {
		return new Model(mesh);
	}
}

Ptr<Mesh> Model::GetMesh() {
	return mMesh;
}

void Model::Animate(bool animate) {
	if (mAnimMatrices.Size() > 0) {
		mAnimating = animate;
	}
}

int Model::GetFPS() const {
	return mFPS;
}

void Model::SetFPS(int fps) {
	mFPS = fps;
}

float Model::GetCurrentFrame() const {
	return mCurrentFrame;
}

void Model::SetCurrentFrame(float frame) {
	mCurrentFrame = frame;
}

void Model::Update(float elapsed) {
	if (mAnimating) {
		mCurrentFrame += mFPS * elapsed;

		if (mCurrentFrame > mMesh->GetLastFrame()) {
			mCurrentFrame = 0;
		}

		if (mCurrentFrame < 0) {
			mCurrentFrame = static_cast<float>(mMesh->GetLastFrame());
		}

		mMesh->CalculateAnimMatrices(mCurrentFrame, mAnimMatrices);
	}
}

void Model::Render() {
	Entity::Render();
	Renderer::Instance()->SetSkinned(mAnimating);

	if (mAnimating) {
		Renderer::Instance()->SetAnimMatrices(mAnimMatrices);
	}

	mMesh->Render();
}