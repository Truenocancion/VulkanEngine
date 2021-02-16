#pragma once

const int kWindowWidth = 1920;
const int kWindowHeight = 1080;

class CViewport;
class VulkanRender;

class CEngine {

	CEngine();
	~CEngine();

	void Update();

	bool m_bShouldRun = true;

	CViewport* m_Viewport;
	VulkanRender* m_Render;

	int m_Width = kWindowWidth;
	int m_Height = kWindowHeight;

public:
	static int Run();

	void Exit();

	void OnViewportResize(int width, int height);

	CViewport* GetViewport() const;
	VulkanRender* GetRender() const;
};

extern CEngine* gEngine;