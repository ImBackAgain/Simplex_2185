#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 Simplex::MyCamera::GetPosition(void) { return m_v3Position; }
void Simplex::MyCamera::SetForward(vector3 a_v3Target) { m_v3Forward = a_v3Target; }
vector3 Simplex::MyCamera::GetForward(void) { return m_v3Forward; }
void Simplex::MyCamera::SetUp(vector3 a_v3Up) { m_v3Up = a_v3Up; }
vector3 Simplex::MyCamera::GetUp(void) { return m_v3Up; }
void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }
void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }
void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }
void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }
void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }
void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }
matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }
matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Forward, vector3 a_v3Up)
{
	Init(); //Initialize the object
	matrix3 m3orientation = matrix3();
	vector3 v3right = glm::cross(a_v3Forward, a_v3Up);
	
	SetPositionForwardAndUp(a_v3Position, a_v3Forward, a_v3Up); //set the position, forward and upward
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_m3Orientation = other.m_m3Orientation;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionForwardAndUp(other.m_v3Position, other.m_v3Forward, other.m_v3Up);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_m3Orientation, other.m_m3Orientation);
	
	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	
	m_v3Right = -AXIS_X;
	m_v3Up = AXIS_Y;
	m_v3Forward = -AXIS_Z;

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionForwardAndUp(vector3 a_v3Position, vector3 a_v3Forward, vector3 a_v3Up)
{
	m_v3Position = a_v3Position;
	m_v3Forward = a_v3Forward;
	m_v3Right = glm::cross(a_v3Forward, a_v3Up);
	m_v3Up = a_v3Up;

	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//Calculate the look at most of your assignment will be reflected in this method
	m_m4View = glm::lookAt(m_v3Position, m_v3Position + m_v3Forward, m_v3Up); //position, target, upward
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective) //perspective
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else //Orthographic
	{
		m_m4Projection = glm::ortho(m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
			m_v2Vertical.x, m_v2Vertical.y, //vertical
			m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void MyCamera::MoveForward(float a_fDistance)
{
	vector3 move = a_fDistance * m_v3Forward;
	m_v3Position += move;
	//m_v3Forward += move;
	//m_v3Above += vector3(0.0f, 0.0f, -a_fDistance);
}

void MyCamera::MoveVertical(float a_fDistance)
{
	vector3 move = a_fDistance * m_v3Up;
	m_v3Position += move;
	//m_v3Forward += move;
}

void MyCamera::MoveGlobalVertical(float a_fDistance)
{
	vector3 move = a_fDistance * AXIS_Y;
	m_v3Position += move;
	//m_v3Forward += move;
}

//Move to the right
void MyCamera::MoveSideways(float a_fDistance)
{
	vector3 move = a_fDistance * m_v3Right;
	m_v3Position += move;
	//m_v3Forward += move;
}

void Simplex::MyCamera::RotateSideways(float a_fAngle)
{
	m_m3Orientation = matrix4(glm::angleAxis(a_fAngle, -AXIS_Y)) * matrix4(m_m3Orientation);
}

void Simplex::MyCamera::RotateVertical(float a_fAngle)
{
	//Clamp
	if (a_fAngle > 0)
	{
		//Rotating upward
		//Don't do it if the local forward vector is close to pointing up
		//I.E. m_v3Forward[1] (y-cooordinate) is close to 1
		if (m_v3Forward[1] > 0.8f) //With a bufffer zone
			return;
	}
	else
	{
		//Rotating downward
		//Stop if local forward vector is almost pointing straight down
		//I.E. m_v3Forward[1] (y) is close to -1
		if (m_v3Forward[1] < -0.8f) //With a bufffer zone
			return;
	}

	m_m3Orientation = matrix4(glm::angleAxis(a_fAngle, m_v3Right)) * matrix4(m_m3Orientation);
}

