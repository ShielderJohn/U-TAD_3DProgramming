#ifndef UGINE_SCREEN_H
#define UGINE_SCREEN_H

#include "string.h"
#include "types.h"

#define MOUSE_LEFT       0
#define MOUSE_RIGHT      1
#define MOUSE_MIDDLE     2

class Screen {
public:
	static Ptr<Screen> Instance() { if ( mInstance == nullptr ) mInstance = new Screen(); return mInstance; }

	// Screen
	void Open(uint16 width, uint16 height, bool fullscreen);
	void Close();
	bool ShouldClose() const;
	void SetTitle(const String& title);
	void Refresh();
	uint16 GetWidth() const { return mWidth; }
	uint16 GetHeight() const { return mHeight; }
	uint16 GetDesktopWidth() const;
	uint16 GetDesktopHeight() const;
	float GetElapsed() const { return mElapsed; }

	// Mouse
	void ShowMouse(bool show);
	void MoveMouse(int x, int y);
	int GetMouseX();
	int GetMouseY();
	bool IsMousePressed(int button) const;

	// Keyboard
	bool IsKeyPressed(int key) const;
protected:
	Screen();
	~Screen();
private:
	static Ptr<Screen> mInstance;

	GLFWwindow* mWindow;
	uint16 mWidth, mHeight;
	int32 mMousex, mMousey;
	float mLastTime, mElapsed;
friend class Ptr<Screen>;
friend class Ptr<const Screen>;
};

#endif // UGINE_SCREEN_H
