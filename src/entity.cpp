#include "../include/entity.h"
#include "../include/scene.h"

Entity::Entity() {
	mPosition = glm::vec3(0, 0, 0);
	mRotation = glm::quat(1, 0, 0, 0);
	mScale = glm::vec3(1, 1, 1);
}

Ptr<Entity> Entity::Create() {
	return new Entity();
}

const glm::vec3& Entity::GetPosition() const {
	return mPosition;
}

glm::vec3& Entity::Position() {
	return mPosition;
}

const glm::quat& Entity::GetRotation() const {
	return mRotation;
}

glm::quat& Entity::Rotation() {
	return mRotation;
}

const glm::vec3& Entity::GetScale() const {
	return mScale;
}

glm::vec3& Entity::Scale() {
	return mScale;
}

void Entity::Move(const glm::vec3& speed) {
	mPosition += mRotation * speed;
}

void Entity::Render() {
	float angle = glm::angle(mRotation);
	glm::vec3 axis = glm::axis(mRotation);
	glm::mat4 model;

	model = glm::translate(model, mPosition);
	model = glm::rotate(model, angle, axis);
	model = glm::scale(model, mScale);

	Scene::Instance()->SetModel(model);
}