#include "../include/scene.h"
#include "../include/renderer.h"

Ptr<Scene> Scene::mInstance = nullptr;

Scene::Scene() {
	Renderer::Instance()->Setup3D();

	mDepthCamera = Camera::Create();
	mDepthCamera->SetUsesTarget(true);

	Ptr<Texture> depthTexture = Texture::Create(1024, 1024, true);
	mDepthCamera->SetViewport(0, 0, depthTexture->GetWidth(), depthTexture->GetHeight());
	Ptr<FrameBuffer> depthFrameBuffer = FrameBuffer::Create(nullptr, depthTexture);
	mDepthCamera->SetFrameBuffer(depthFrameBuffer);

	mDepthBias = glm::mat4();
	mDepthFar = 0.0f;
	mEnableShadows = false;
}

Ptr<Scene> Scene::Instance() {
	if (mInstance == nullptr) {
		mInstance = new Scene();
	}

	return mInstance;
}

Ptr<const Camera> Scene::GetCurrentCamera() const {
	return mCurrentCamera.ConstCast();
}

Ptr<Camera> Scene::GetCurrentCamera() {
	return mCurrentCamera;
}

const glm::mat4 & Scene::GetModel() const {
	return mModelMatrix;
}

void Scene::SetModel(const glm::mat4& m) {
	mModelMatrix = m;
	Renderer::Instance()->SetMatrices(mModelMatrix, mCurrentCamera->GetView(), mCurrentCamera->GetProjection(), mDepthBias);
}

void Scene::AddEntity(Ptr<Entity> entity) {
	mEntities.Add(entity);
	Ptr<Camera> camera = entity.DownCast<Camera>();
	Ptr<Light> light = entity.DownCast<Light>();

	if (camera != nullptr) {
		mCameras.Add(camera);
	}

	if (light != nullptr) {
		mLights.Add(light);
	}
}

void Scene::RemoveEntity(Ptr<Entity> entity) {
	Ptr<Camera> camera = entity.DownCast<Camera>();
	Ptr<Light> light = entity.DownCast<Light>();

	if (camera != nullptr) {
		mCameras.Remove(camera);
	}

	if (light != nullptr) {
		mLights.Remove(light);
	}

	mEntities.Remove(entity);
}

uint32 Scene::GetNumEntities() const {
	return mEntities.Size();
}

Ptr<const Entity> Scene::GetEntity(uint32 index) const {
	return mEntities[index].ConstCast();
}

Ptr<Entity> Scene::GetEntity(uint32 index) {
	return mEntities[index];
}

void Scene::SetAmbient(const glm::vec3& ambient) {
	Renderer::Instance()->SetAmbient(ambient);
}

void Scene::SetDepthOrtho(float left, float right, float bottom, float top, float near, float far) {
	mDepthCamera->SetProjection(glm::ortho(left, right, bottom, top, near, far));
	mDepthFar = far / 2;
}

void Scene::EnableShadows(bool enable) {
	mEnableShadows = enable;
}

void Scene::Update(float elapsed) {
	for (uint32 i = 0; i < mEntities.Size(); i++) {
		mEntities[i]->Update(elapsed);
	}
}

void Scene::Render() {
	if (mEnableShadows) {
		if (mLights.Size() > 0) {
			for (int i = 0; i < mLights.Size(); i++) {
				mCurrentCamera = mDepthCamera;
				Renderer::Instance()->UseProgram(Renderer::Instance()->GetDepthProgram());

				// Set camera position
				glm::vec3 lightPosition = glm::normalize(mLights[i]->Position());
				mCurrentCamera->Position() = lightPosition * mDepthFar;
				mCurrentCamera->SetUsesTarget(true);
				mCurrentCamera->GetTarget() = mCurrentCamera->Position() * -1.0f;
				mCurrentCamera->Prepare();

				// Generate DepthBias matrix
				glm::mat4 transformRangeMatrix(
					0.5f, 0, 0, 0,
					0, 0.5f, 0, 0,
					0, 0, 0.5f, 0,
					0.5f, 0.5f, 0.5f, 1);
				mDepthBias = transformRangeMatrix * mCurrentCamera->GetProjection() * mCurrentCamera->GetView();

				// Render scene entities
				for (int j = 0; j < mEntities.Size(); j++) {
					mEntities[j]->Render();
				}

				Renderer::Instance()->UseProgram(0);
				Renderer::Instance()->SetDepthTexture(mCurrentCamera->GetFrameBuffer()->GetDepthTexture()->GetHandle());
				Renderer::Instance()->EnableShadows(mEnableShadows);

				mCurrentCamera = nullptr;
				break;
			}
		}
	}

	for (uint32 i = 0; i < mCameras.Size(); i++) {
		mCurrentCamera = mCameras[i];
		mCurrentCamera->Prepare();

		if (mLights.Size() > 0) {
			Renderer::Instance()->EnableLighting(true);

			for (int j = 0; j < mLights.Size(); j++) {
				mLights[j]->Prepare();
			}
		}

		for (int k = 0; k < mEntities.Size(); k++) {
			mEntities[k]->Render();
		}
	}

	if (mLights.Size() > 0) {
		for (int i = 0; i < mLights.Size(); i++) {
			Renderer::Instance()->EnableLight(i, false);
		}
	}
}