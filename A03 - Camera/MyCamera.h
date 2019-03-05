/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2018/09
----------------------------------------------*/
#ifndef __MYCAMERACLASS_H_
#define __MYCAMERACLASS_H_

#include "Definitions.h"

//I use a matrix to hold the orientation of the camera
#define m_v3Right m_m3Orientation[0]
#define m_v3Up m_m3Orientation[1]
#define m_v3Forward m_m3Orientation[2]
//For ease of rotating

namespace Simplex
{

class MyCamera
{
	vector3 m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	
	//What was that about "up" being confusing?
	//Sometimes I think I just like confusing myself... sigh...

	matrix3 m_m3Orientation; //And scale, I guesss. But since we can asssume a lot of things about a camera object...

	bool m_bPerspective = true; //perspective view? False is Orthographic

	float m_fFOV = 45.0f; //Field of View

	vector2 m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	vector2 m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	vector2 m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	vector2 m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	matrix4 m_m4View; //View matrix
	matrix4 m_m4Projection; //Projection Matrix
public:
	/*
	USAGE: Constructor
	ARGUMENTS: ---
	OUTPUT: the class object
	*/
	MyCamera();

	/*
	USAGE: constructor
	ARGUMENTS:
	-	vector3 a_v3Position -> Where my camera is located
	-	vector3 a_v3Target -> What I'm looking at
	-	vector3 a_v3Upward -> What direction is up
	OUTPUT: ---
	*/
	MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward);

	/*
	USAGE: Copy constructor
	ARGUMENTS: MyCamera const& other -> object to copy
	OUTPUT: ---
	*/
	MyCamera(MyCamera const& other);

	/*
	USAGE: Copy Assignment operator
	ARGUMENTS: MyCamera const& other -> object to copy
	OUTPUT: ---
	*/
	MyCamera& operator=(MyCamera const& other);

	/*
	USAGE: Initialize the class
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Init(void);

	/*
	USAGE: Release pointers in the class
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Release(void);

	/*
	USAGE: swap the content with an incoming object
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Swap(MyCamera& other);

	/*
	USAGE: Destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~MyCamera(void);

	/*
	USAGE: Sets the position of the camera
	ARGUMENTS: vector3 a_v3Position -> Position where we want the camera to be
	OUTPUT: ---
	*/
	void SetPosition(vector3 a_v3Position);

	/*
	USAGE: Gets the position of the camera
	ARGUMENTS: ---
	OUTPUT: position of the camera
	*/
	vector3 GetPosition(void);

	/*
	USAGE: Sets what the camera will be looking at
	ARGUMENTS: vector3 a_v3Target -> Direction in which the camera should face
	OUTPUT: ---
	*/
	void SetForward(vector3 a_v3Forward);

	/*
	USAGE: Gets what the camera will be looking at
	ARGUMENTS: ---
	OUTPUT: target of the camera
	*/
	vector3 GetForward(void);

	/*
	USAGE: Sets the position of the point above the camera
	ARGUMENTS: vector3 a_v3Above -> The direction that is upward
	OUTPUT: ---
	*/
	void SetUp(vector3 a_v3Up);

	/*
	USAGE: Gets the direction that is upward
	ARGUMENTS: ---
	OUTPUT: position above the camera
	*/
	vector3 GetUp(void);


	/*
	USAGE: Sets Perspective Camera
	ARGUMENTS: bool a_bPerspective = true -> is camera perspective or orthographic
	OUTPUT: ---
	*/
	void SetPerspective(bool a_bPerspective = true);
	/*
	USAGE: Sets the Field of View of the camera for perspective use
	ARGUMENTS: float a_fFOV -> field of view to apply to the camera
	OUTPUT: ---
	*/
	void SetFOV(float a_fFOV);
	/*
	USAGE: Sets the resolution of the camera aspect ratio
	ARGUMENTS: vector2 a_v2Resolution -> resolution of the window
	OUTPUT: ---
	*/
	void SetResolution(vector2 a_v2Resolution);
	/*
	USAGE: Sets the near and far planes of the camera
	ARGUMENTS: vector2 a_v2NearFar -> near far planes
	OUTPUT: ---
	*/
	void SetNearFar(vector2 a_v2NearFar);
	/*
	USAGE: Sets the horizontal planes of the camera
	ARGUMENTS: vector2 a_v2Horizontal -> horizontal planes
	OUTPUT: ---
	*/
	void SetHorizontalPlanes(vector2 a_v2Horizontal);
	/*
	USAGE: Sets the vertical planes of the camera
	ARGUMENTS: vector2 a_v2Vertical -> vertical planes
	OUTPUT: ---
	*/
	void SetVerticalPlanes(vector2 a_v2Vertical);

	/*
	USAGE: Gets the projection matrix of the camera
	ARGUMENTS: ---
	OUTPUT: projection matrix of the camera
	*/
	matrix4 GetProjectionMatrix(void);

	/*
	USAGE: Gets the view matrix of the camera
	ARGUMENTS: ---
	OUTPUT: view matrix of the camera
	*/
	matrix4 GetViewMatrix(void);

	/*
	USAGE: Resets the camera to default values
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void ResetCamera(void);

	/*
	USAGE: Set the position target and up of the camera at once
	ARGUMENTS:
	-	vector3 a_v3Position -> Where my camera is located
	-	vector3 a_v3Forward -> Which direction is forward
	-	vector3 a_v3Up -> Which way is up
	OUTPUT: ---
	*/
	void SetPositionForwardAndUp(vector3 a_v3Position, vector3 a_v3Forward, vector3 a_v3Up);

	/*
	USAGE: Calculate what the camera should be looking at with the values of position target and up
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void CalculateViewMatrix(void);

	/*
	USAGE: Calculate how the camera sees the world
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void CalculateProjectionMatrix(void);

	/*
	USAGE: Translates the camera forward or backward
	ARGUMENTS: float a_fDistance = 0.1f -> amount of movement
	OUTPUT: ---
	*/
	void MoveForward(float a_fDistance = 0.1f);
	/*
	USAGE: Translates the camera Upward or downward, normal to direction of view
	ARGUMENTS: float a_fDistance = 0.1f -> amount of movement
	OUTPUT: ---
	*/
	void MoveVertical(float a_fDistance = 0.1f);
	/*
	USAGE: Translates the camera Upward or downward in global space
	ARGUMENTS: float a_fDistance = 0.1f -> amount of movement
	OUTPUT: ---
	*/
	void MoveGlobalVertical(float a_fDistance);
	/*
	USAGE: Translates the camera right or left
	ARGUMENTS: float a_fDistance = 0.1f -> amount of movement
	OUTPUT: ---
	*/
	void MoveSideways(float a_fDistance = 0.1f);
	/*
	USAGE: Turns the camera about the global vertical axis.
	ARGUMENTS: angle to turn to the right
	OUTPUT: ---
	*/
	void RotateSideways(float a_fAngle = 0.1f);
	/*
	USAGE: Turns the camera about the local left-right axis
	ARGUMENTS: angle to turn downward
	OUTPUT: ---
	*/
	void RotateVertical(float a_fAngle = 0.1f);
};

} //namespace Simplex

#endif //__MYCAMERACLASS_H_

	/*
	USAGE:
	ARGUMENTS: ---
	OUTPUT: ---
	*/