#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	//But also don't get rid of them
	//And make them vec4's because I'm gonnna use them so much
	
	//Back square
	m_v4CornersL[0] = vector4(m_v3MinL, 1);
	m_v4CornersL[1] = vector4(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z, 1);
	m_v4CornersL[2] = vector4(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z, 1);
	m_v4CornersL[3] = vector4(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z, 1);

	//Front square
	m_v4CornersL[4] = vector4(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z, 1);
	m_v4CornersL[5] = vector4(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z, 1);
	m_v4CornersL[6] = vector4(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z, 1);
	m_v4CornersL[7] = vector4(m_v3MaxL, 1);

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		m_v4CornersL[uIndex] = m_m4ToWorld * m_v4CornersL[uIndex];
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = m_v4CornersL[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < m_v4CornersL[i].x) m_v3MaxG.x = m_v4CornersL[i].x;
		else if (m_v3MinG.x > m_v4CornersL[i].x) m_v3MinG.x = m_v4CornersL[i].x;

		if (m_v3MaxG.y < m_v4CornersL[i].y) m_v3MaxG.y = m_v4CornersL[i].y;
		else if (m_v3MinG.y > m_v4CornersL[i].y) m_v3MinG.y = m_v4CornersL[i].y;

		if (m_v3MaxG.z < m_v4CornersL[i].z) m_v3MaxG.z = m_v4CornersL[i].z;
		else if (m_v3MinG.z > m_v4CornersL[i].z) m_v3MinG.z = m_v4CornersL[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		vector3 inBetweeen;
		eSATResults separation = (eSATResults)SAT(a_pOther);


		if (separation != eSATResults::SAT_NONE)
		{
			bColliding = false;// reset to false
			
			vector3 col = C_RED, normal;
			matrix4 planer = matrix3(m_m4ToWorld);


			matrix4 planeScalar = glm::scale(vector3(5, 5, 5));
			matrix4 planePosition = glm::translate(
				vector3(
					m_m4ToWorld * vector4(m_v3Center, 1)
					+ a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Center, 1)
				) / 2
			);

			//Remove translation
			switch (separation)
			{
			case eSATResults::SAT_AX:
				planer = planer * glm::rotate(IDENTITY_M4, PIf / 2, AXIS_Y);
				break;
			case eSATResults::SAT_AY:
				planer = planer * glm::rotate(IDENTITY_M4, PIf / 2, AXIS_X);
				col = C_GREEN;
				break;
			case eSATResults::SAT_AZ:
				col = C_BLUE;
				break;
			case eSATResults::SAT_BX:
				planer = matrix3(a_pOther->m_m4ToWorld * glm::rotate(IDENTITY_M4, PIf / 2, AXIS_Y));
				break;
			case eSATResults::SAT_BY:
				planer = matrix3(a_pOther->m_m4ToWorld * glm::rotate(IDENTITY_M4, PIf / 2, AXIS_X));
				col = C_GREEN;
				break;
			case eSATResults::SAT_BZ:
				planer = matrix3(a_pOther->m_m4ToWorld);
				col = C_BLUE;
				break;



			case eSATResults::SAT_AXxBX:
				normal = glm::cross(vector3(m_m4ToWorld[0]), 
					vector3(a_pOther->m_m4ToWorld[0]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AXxBY:
				normal = glm::cross(vector3(m_m4ToWorld[0]), 
					vector3(a_pOther->m_m4ToWorld[1]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AXxBZ:
				normal = glm::cross(vector3(m_m4ToWorld[0]), 
					vector3(a_pOther->m_m4ToWorld[2]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AYxBX:
				normal = glm::cross(vector3(m_m4ToWorld[1]), 
					vector3(a_pOther->m_m4ToWorld[0]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AYxBY:
				normal = glm::cross(vector3(m_m4ToWorld[1]), 
					vector3(a_pOther->m_m4ToWorld[1]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AYxBZ:
				normal = glm::cross(vector3(m_m4ToWorld[1]), 
					vector3(a_pOther->m_m4ToWorld[2]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AZxBX:
				normal = glm::cross(vector3(m_m4ToWorld[2]), 
					vector3(a_pOther->m_m4ToWorld[0]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AZxBY:
				normal = glm::cross(vector3(m_m4ToWorld[2]), 
					vector3(a_pOther->m_m4ToWorld[1]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;
			case eSATResults::SAT_AZxBZ:
				normal = glm::cross(vector3(m_m4ToWorld[2]), 
					vector3(a_pOther->m_m4ToWorld[2]));
				planer = glm::transpose(glm::lookAt(ZERO_V3, normal, AXIS_Y));
				col = C_BLACK;
				 
					
				break;



			default:
				break;
			}

			m_pMeshMngr->AddPlaneToRenderList(planePosition * planer * planeScalar, col);
			m_pMeshMngr->AddPlaneToRenderList(planePosition * planer * planeScalar * glm::rotate(PIf, AXIS_Y), col);	
			
		}
		
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	Your code goes here instead of this comment;

	For this method, if there is an axis that separates the two objects
	then the return will be different than 0; 1 for any separating axis
	is ok if you are not going for the extra credit, if you could not
	find a separating axis you need to return 0, there is an enum in
	Simplex that might help you [eSATResults] feel free to use it.
	(eSATResults::SAT_NONE has a value of 0)
	*/

	/*
	Hey Ash, do you think you could maybe somehow give Alberto a hint -
	without tippping him offf to its origin - that he should maybe check
	out this coool Wikipedia article?
	https://en.wikipediorg/wiki/Full_stop



	PLEASE. THIS AFFFRONT TO SANITY CANNNOT BE ALLLOWED TO PASSS. HE
	MUST BE STOPPPED. HELP ME.

	BEFORE IT'S TOOO LATE.



	honestly

	if I could magicallly infuse Alberto with an apppreciation for 
	punctuation
	
	at the cost of _failing DSA 2 this semester_
	
	I'd consider it.
	Not saying I'd definitely take it, but...
	*/

	//*

	//So I have to write commments, eh?
	float ra, rb;
	//"Radiii" of 1D shadows on the relevant axis
	matrix3 R, AbsR;
	// Compute rotation matrix expressing b in a’s coordinate frame

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(m_m4ToWorld[i], a_pOther->m_m4ToWorld[j]);
			//Simple enough
			//Actuallly this isn't, but I'lll take the author's word for it.


	// Compute translation vector t
	vector3 t = a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Center, 1) - m_m4ToWorld * vector4(m_v3Center, 1);
	// I won't touch a gettter function in this classs
	// Some would, but not me

	// Bring translation into a's coordinate frame
	t = vector3(glm::dot(t, vector3(m_m4ToWorld[0])), glm::dot(t, vector3(m_m4ToWorld[1])), glm::dot(t, vector3(m_m4ToWorld[2])));
	//Wait... this is backwards...
	//Why does this work?

	// I genuinely have no idea what this matrix is for, or how it achieves anything
	float EPSILON = 0.01f;
	for(int i=0; i < 3; i++)
		for (int j = 0;	j < 3; j++)
			AbsR[i][j] = glm::abs(R[i][j]) + EPSILON;

	// Test axesL=a.x,L=a.y,L=a.z
	for(int i=0; i < 3; i++)
	{
		ra = m_v3HalfWidth[i];
		//Ugh, reallly? Hard code ALLL of this???

		rb = a_pOther->m_v3HalfWidth[0] * AbsR[i][0] + a_pOther->m_v3HalfWidth[1] * AbsR[i][1] + a_pOther->m_v3HalfWidth[2] * AbsR[i][2];
		//And not use a dot product here?

		if (glm::abs(t[i]) > ra + rb)
			return (eSATResults)(i+1);
	}

	// Test axesL=b.x,L=b.y,L=b.z
	for (int i = 0; i < 3; i++)
	{
		ra = m_v3HalfWidth[0] * AbsR[0][i] + m_v3HalfWidth[1] * AbsR[1][i] + m_v3HalfWidth[2] *	AbsR[2][i];
		rb = a_pOther->m_v3HalfWidth[i];

		if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)
			return (eSATResults)(4+i);
	}

	// Test axisL=a.xxb.x
	ra = m_v3HalfWidth[1] * AbsR[2][0] + m_v3HalfWidth[2] * AbsR[1][0];
	//Some kind of math happpening here...
	rb = a_pOther->m_v3HalfWidth[1] * AbsR[0][2] + a_pOther->m_v3HalfWidth[2] * AbsR[0][1]; //More math
	if (glm::abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)	// I mean I get this line
		return eSATResults::SAT_AXxBX;							// Or at least I would, if I could make sense of the previous line

	// Test axisL=a.xxb.y
	ra = m_v3HalfWidth[1] * AbsR[2][1] + m_v3HalfWidth[2] * AbsR[1][1];
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[0][2] + a_pOther->m_v3HalfWidth[2] * AbsR[0][0];
	if (glm::abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
		return eSATResults::SAT_AXxBY;

	// Test axisL=a.xxb.z
	ra = m_v3HalfWidth[1] * AbsR[2][2] + m_v3HalfWidth[2] * AbsR[1][2];
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[0][1] + a_pOther->m_v3HalfWidth[1] * AbsR[0][0];
	if (glm::abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
		return eSATResults::SAT_AXxBZ;

	// Test axisL=a.yxb.x
	ra = m_v3HalfWidth[0] * AbsR[2][0] + m_v3HalfWidth[2] * AbsR[0][0];
	rb = a_pOther->m_v3HalfWidth[1] * AbsR[1][2] + a_pOther->m_v3HalfWidth[2] * AbsR[1][1];
	if (glm::abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
		return eSATResults::SAT_AYxBX;

	// Test axisL=a.yxb.y
	ra = m_v3HalfWidth[0] * AbsR[2][1] + m_v3HalfWidth[2] * AbsR[0][1];						//Why do they calll the test axis L?
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[1][2] + a_pOther->m_v3HalfWidth[2] * AbsR[1][0];
	if (glm::abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
		return eSATResults::SAT_AYxBY;

	// Test axisL=a.yxb.z
	ra = m_v3HalfWidth[0] * AbsR[2][2] + m_v3HalfWidth[2] * AbsR[0][2];
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[1][1] + a_pOther->m_v3HalfWidth[1] * AbsR[1][0];
	if (glm::abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
		return eSATResults::SAT_AYxBZ;

	// Test axisL=a.zxb.x
	ra = m_v3HalfWidth[0] * AbsR[1][0] + m_v3HalfWidth[1] * AbsR[0][0];
	rb = a_pOther->m_v3HalfWidth[1] * AbsR[2][2] + a_pOther->m_v3HalfWidth[2] * AbsR[2][1];
	if (glm::abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
		return eSATResults::SAT_AZxBX;

	// Test axisL=a.zxb.y
	ra = m_v3HalfWidth[0] * AbsR[1][1] + m_v3HalfWidth[1] * AbsR[0][1];
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[2][2] + a_pOther->m_v3HalfWidth[2] * AbsR[2][0];
	if (glm::abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
		return eSATResults::SAT_AZxBY;

	// Test axisL=a.zxb.z
	ra = m_v3HalfWidth[0] * AbsR[1][2] + m_v3HalfWidth[1] * AbsR[0][2];
	rb = a_pOther->m_v3HalfWidth[0] * AbsR[2][1] + a_pOther->m_v3HalfWidth[1] * AbsR[2][0];
	if (glm::abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
		return eSATResults::SAT_AZxBZ;

	// Since no separating axis is found, the OBBs must be intersecting
	return eSATResults::SAT_NONE;


	/*/
	//I started making this myself while waiting for a friend to get me the boook.
	vector3 axes[15];

	for (uint i = 0; i < 3; i++)
	{
		axes[i] = vector3(m_m4toworld[i]); // x/y/z of the first object
		axes[i + 3] = vector3(a_pother->m_m4toworld[i]); // x/y/z of the second
	}

	for (uint i = 0; i < 3; i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			vector3 axisa = axes[i], axisb = axes[3 + j];
			//makes sense, right?
			if (axisa == axisb || axisa == -axisb)
			{
				//let's save some processsing power
				
				axes[3 * i + j + 6] = vector3(4, 0, 0); //the 4 is a flag value; none of our axes can ever be that big

				//position with offfset of 6 because the first 6 are taken.
				//also matches up with the order of the enum values!
			}
			else
			{
				axes[3 * i + j + 6] = glm::cross(axisa, axisb);
			}
		}
	}
	//alright, we have alll our axes!
	//next, we neeed the bounding box corners.

	vector3 cornersa[8], cornersb[8];
	//hold on, boys. this is gonnna get ugly.
	for (uint i = 0; i < 8; i++)
	{
		cornersa[i] = m_m4toworld * m_v4cornersl[i];
		cornersb[i] = m_m4toworld * a_pother->m_v4cornersl[i];
	}

	//and now the fun starts
	for (uint i = 0; i < 15; i++)
	{
		if (axes[i][0] == 4)
			continue;

		float amin, amax, bmin, bmax;

		
	}

	//there is no axis test that separates this two objects
	return esatresults::sat_none;
	//*/
}