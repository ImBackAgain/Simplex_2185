#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateCube(1.0f, C_BLACK);

	m_vv3Positions.push_back(vector3(3, 1, 0));
	m_vv3Positions.push_back(vector3(3, 0, 0));
	m_vv3Positions.push_back(vector3(3, -1, 0));
	m_vv3Positions.push_back(vector3(2, 2, 0));
	m_vv3Positions.push_back(vector3(2, 1, 0));
	m_vv3Positions.push_back(vector3(1, 5, 0));
	m_vv3Positions.push_back(vector3(1, 3, 0));
	m_vv3Positions.push_back(vector3(1, 2, 0));
	m_vv3Positions.push_back(vector3(1, 1, 0));
	m_vv3Positions.push_back(vector3(1, 0, 0));
	m_vv3Positions.push_back(vector3(1, -1, 0));
	m_vv3Positions.push_back(vector3(0, 4, 0));
	m_vv3Positions.push_back(vector3(0, 3, 0));
	m_vv3Positions.push_back(vector3(0, 1, 0));
	m_vv3Positions.push_back(vector3(0, 0, 0));
	m_vv3Positions.push_back(vector3(0, -2, 0));
	m_vv3Positions.push_back(vector3(-1, 3, 0));
	m_vv3Positions.push_back(vector3(-1, 2, 0));
	m_vv3Positions.push_back(vector3(-1, 1, 0));
	m_vv3Positions.push_back(vector3(-1, 0, 0));
	m_vv3Positions.push_back(vector3(-1, -2, 0));
	m_vv3Positions.push_back(vector3(-2, 3, 0));
	m_vv3Positions.push_back(vector3(-2, 2, 0));
	m_vv3Positions.push_back(vector3(-2, 1, 0));
	m_vv3Positions.push_back(vector3(-2, 0, 0));
	m_vv3Positions.push_back(vector3(-3, 3, 0));
	m_vv3Positions.push_back(vector3(-3, 2, 0));
	m_vv3Positions.push_back(vector3(-3, 1, 0));
	m_vv3Positions.push_back(vector3(-3, 0, 0));
	m_vv3Positions.push_back(vector3(-3, -2, 0));
	m_vv3Positions.push_back(vector3(-4, 4, 0));
	m_vv3Positions.push_back(vector3(-4, 3, 0));
	m_vv3Positions.push_back(vector3(-4, 1, 0));
	m_vv3Positions.push_back(vector3(-4, 0, 0));
	m_vv3Positions.push_back(vector3(-4, -2, 0));
	m_vv3Positions.push_back(vector3(-5, 5, 0));
	m_vv3Positions.push_back(vector3(-5, 3, 0));
	m_vv3Positions.push_back(vector3(-5, 2, 0));
	m_vv3Positions.push_back(vector3(-5, 1, 0));
	m_vv3Positions.push_back(vector3(-5, 0, 0));
	m_vv3Positions.push_back(vector3(-5, -1, 0));
	m_vv3Positions.push_back(vector3(-6, 2, 0));
	m_vv3Positions.push_back(vector3(-6, 1, 0));
	m_vv3Positions.push_back(vector3(-7, 1, 0));
	m_vv3Positions.push_back(vector3(-7, 0, 0));
	m_vv3Positions.push_back(vector3(-7, -1, 0));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	

	//matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = -10.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(10 * sin(value/6), 5 * sin(value/3), .0f));
	value += 0.1f;

	
	matrix4 m4Model = m4Translate;

	for (vector3 v : m_vv3Positions)
	{
		m_pMesh->Render(m4Projection, m4View, glm::translate(m4Model, v));
	}
	//m_pMesh->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	m_pMeshMngr->AddGridToRenderList();
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}