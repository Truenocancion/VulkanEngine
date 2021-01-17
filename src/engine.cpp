#include "engine.h"
#include "viewport.h"
#include "render.h"

CEngine* gEngine = nullptr;

CEngine::CEngine() {
	gEngine = this;

	m_Viewport = new CViewport();
	m_Render = new CRender();
}

CEngine::~CEngine(){
	delete m_Render;
	delete m_Viewport;
}

void CEngine::Update() {
	m_Viewport->Update();
	m_Render->Update();
}

void CEngine::Exit() {
	m_bShouldRun = false;
}

int CEngine::Run() {
	CEngine engine;
	while (engine.m_bShouldRun) {
		engine.Update();
	}
	return 0;
}

void CEngine::OnViewportResize(int width, int height) {
	m_Width = width;
	m_Height = height;
}

CViewport* CEngine::GetViewport() const
{
	return m_Viewport;
}

CRender* CEngine::GetRender() const
{
	return m_Render;
}
