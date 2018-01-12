#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "../include/u-gine.h"
#define FULLSCREEN false

#define WIDTH  800
#define HEIGHT 600

int main() {
	if (FULLSCREEN) Screen::Instance()->Open(Screen::Instance()->GetDesktopWidth(), Screen::Instance()->GetDesktopHeight(), true);
	else            Screen::Instance()->Open(WIDTH, HEIGHT, false);

	// Model
	Ptr<Model> model = Model::Create(ResourceManager::Instance()->LoadMesh("data/dwarf.msh"));
	model->Animate(true);
	model->SetFPS(24);

	// Camera
	Ptr<Camera>	camera = Camera::Create();
	camera->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
	camera->Position() = glm::vec3(0.0f, 50.f, 120.f);
	camera->Rotation() = glm::quat(glm::vec3(glm::radians(-10.0f), 0, 0));

	// Scene entities
	Scene::Instance()->AddEntity(model.UpCast<Entity>());
	Scene::Instance()->AddEntity(camera.UpCast<Entity>());

	while (!Screen::Instance()->ShouldClose() && !Screen::Instance()->IsKeyPressed(GLFW_KEY_ESCAPE)) {
		Scene::Instance()->Update(Screen::Instance()->GetElapsed());
		Scene::Instance()->Render();

		Screen::Instance()->Refresh();
		Screen::Instance()->SetTitle(Renderer::Instance()->GetProgramError());
	}

	ResourceManager::Instance()->FreeResources();
	return 0;
}
