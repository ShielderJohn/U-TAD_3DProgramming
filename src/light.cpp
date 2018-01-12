#include "../include/light.h"
#include "../include/scene.h"

bool Light::lightsUsed[MAX_LIGHTS]{ false };

Light::Light() {
	bool found = false;

	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (!lightsUsed[i]) {
			lightsUsed[i] = true;
			found = true;
			mIndex = i;
			break;
		}
	}

	if (!found) {
		mIndex = MAX_LIGHTS;
	}
}

Light::~Light() {
	lightsUsed[mIndex] = false;
}

Ptr<Light> Light::Create() {
	Ptr<Light> light = new Light();

	if (light->mIndex < MAX_LIGHTS) {
		return light;
	}
	else {
		return nullptr;
	}
}

Light::Type Light::GetType() const {
	return mType;
}

void Light::SetType(Type type) {
	mType = type;
}

const glm::vec3& Light::GetColor() const {
	return mColor;
}

void Light::SetColor(const glm::vec3& color) {
	mColor = color;
}

float Light::GetAttenuation() const {
	return mAttenuation;
}

void Light::SetAttenuation(float att) {
	mAttenuation = att;
}

void Light::Prepare() {
	Renderer::Instance()->EnableLight(mIndex, true);
	glm::vec4 lightPosition;

	if (mType == DIRECTIONAL) {
		lightPosition = glm::vec4(Position(), 0);
	}
	else if (mType == POINT) {
		lightPosition = glm::vec4(Position(), 1);
	}

	glm::vec4 obsPosition = Scene::Instance()->GetCurrentCamera()->GetView() * Scene::Instance()->GetModel() * lightPosition;
	Renderer::Instance()->SetLightData(mIndex, obsPosition, mColor, mAttenuation);
}