﻿#pragma once

#pragma warning( disable:4505 )
	// unreferenced local function has been removed 경고 숨기기위해서 사용		
	// warning 4505는 사용되지 않는 함수가 있다는 경고인데, 
	// 실제로 사용중인데도 메세지가 떴었음.(IsCollide, Intersection어쩌구~) 해결방법을 못 찾던 도중에 
	// 박민근 교수님께 물어보니 교수님께서 끄라고 하시어...
	// 혹시 방법을 찾으신 분은 바꿔주세요~


#include "GameOption.h"
#include "CollisionBox.h"
#include <limits>

/*
작성일 : 2014. 4. 6
내용 : 물리적 계산 수행(물체 운동, 회전, 충돌, 타겟검출 등)
*/

namespace Physics
{
	/*
	input : 현재 위치, 속도, 시간 변화량
	output : 업데이트된 현재 위치
	주의 : 등속운동인 경우 사용, 가속도에 의한 계산이 필요하면 오버로딩된 함수 사용할 것
	*/
	inline void CalcCurrentPosition( _Inout_ D3DXVECTOR3* pos, _In_ const D3DXVECTOR3 &velocity, _In_ float dt, _In_ float speedConstant = DEFAULT_MOVE_CONSTANT )
	{
		// s' = s + v * t	
		pos->x = pos->x + ( velocity.x * dt * speedConstant );
		pos->y = pos->y + ( velocity.y * dt * speedConstant );
		pos->z = pos->z + ( velocity.z * dt * speedConstant );
	}

	/*
	input : 현재 위치, 현재 속도, 가속도, 시간 변화량
	output : 업데이트된 현재 위치, 업데이트된 현재 속도
	주의 : 가속도에 의한 운동인 경우 사용, 등속운동의 경우 오버로딩된 함수 사용할 것
	*/
	inline void CalcCurrentPosition( _Inout_ D3DXVECTOR3* pos, _Inout_ D3DXVECTOR3* velocity, _In_ const D3DXVECTOR3 &acceleration, _In_ float dt, _In_ float speedConstant = DEFAULT_MOVE_CONSTANT )
	{
		// s' = s + v * t + 1/2 * a * t^2

		// v' = v + a * t
		CalcCurrentPosition( velocity, acceleration, dt );

		// v * t
		CalcCurrentPosition( pos, *velocity, dt, speedConstant );

		// 1/2 * a * t^2
		pos->x = pos->x + ( 0.5f * acceleration.x * dt * dt );
		pos->y = pos->y + ( 0.5f * acceleration.y * dt * dt );
		pos->z = pos->z + ( 0.5f * acceleration.z * dt * dt );
	}

	static void SATtest( const D3DXVECTOR3& axis, const D3DXVECTOR3& centerPos, const float& axisLen, float& minAlong, float& maxAlong )
	{
		minAlong = std::numeric_limits<float>::infinity();
		maxAlong = -std::numeric_limits<float>::infinity();

		float dotVal = dotVal = D3DXVec3Dot( &axis, &centerPos );
		minAlong = dotVal - axisLen;
		maxAlong = dotVal + axisLen;
	}

	static void SATtest( const D3DXVECTOR3& axis, const std::array<D3DXVECTOR3, 8>& points, float& minAlong, float& maxAlong )
	{
		minAlong = std::numeric_limits<float>::infinity();
		maxAlong = -std::numeric_limits<float>::infinity();

		for ( int i = 0; i < 8; i++ )
		{
			// 주어진 점 리스트들을 axis에 투영해서 최대 최소 값을 바꿔준다.
			float dotVal = D3DXVec3Dot( &axis, &points[i] );
			if ( dotVal < minAlong )  minAlong = dotVal;
			if ( dotVal > maxAlong )  maxAlong = dotVal;
		}
	}

	// val값이 lowerBound와 upperBound 사이의 값인지 리턴
	inline bool IsBetweenOrdered( float val, float lowerBound, float upperBound ) {
		return lowerBound <= val && val <= upperBound;
	}

	// 두 최대 최소 구간이 서로 겹치는지 리턴
	static bool Overlaps( float min1, float max1, float min2, float max2 )
	{
		return IsBetweenOrdered( min2, min1, max1 ) || IsBetweenOrdered( min1, min2, max2 );
	}

	/*
	inline void TransformCollisionBox( CollisionBox &box )
	{
		D3DXVECTOR4 tempMat;

		// 현재 위치
		D3DXVec3Transform( &tempMat, &box.m_CenterPos, box.m_Transform );
		box.m_CenterPos = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

		// 각 점 좌표
		D3DXVec3TransformCoordArray( 
			box.m_PointList.data(), sizeof( D3DXVECTOR3 ), 
			box.m_PointList.data(), sizeof( D3DXVECTOR3 ), 
			box.m_Transform, BOX_POINT_COUNT 
			);
		
		// 축
		for ( int i = 0; i < 3; ++i )
		{
			// 마지막 항을 0으로 해야 평행이동 값이 벡터에 반영되지 않음
			D3DXVECTOR4 tempAxis = D3DXVECTOR4( box.m_AxisDir[i].x, box.m_AxisDir[i].y, box.m_AxisDir[i].z, 0 );

			D3DXVec4Transform( &tempMat, &tempAxis, box.m_Transform );
			box.m_AxisDir[i] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );
		}
	}
	*/

	static bool IsCollide( const CollisionBox *box1, const CollisionBox *box2 )
	{
		// TransformCollisionBox( box1 );
		// TransformCollisionBox( box2 );

		for ( int i = 0; i < 3; i++ )
		{
			// 첫 번째 박스의 축에 대해서 충돌 확인
			float box1Min, box1Max, box2Min, box2Max;
			SATtest( box1->m_AxisDir[i], box1->m_CenterPos, box1->m_AxisLen[i], box1Min, box1Max );
			SATtest( box1->m_AxisDir[i], box2->m_PointList, box2Min, box2Max );
			if ( !Overlaps( box1Min, box1Max, box2Min, box2Max ) )
			{
				return false;
			}

			// 두 번째 박스의 축에 대해서 충돌 확인
			SATtest( box2->m_AxisDir[i], box1->m_PointList, box1Min, box1Max );
			SATtest( box2->m_AxisDir[i], box2->m_CenterPos, box2->m_AxisLen[i], box2Min, box2Max );
			if ( !Overlaps( box1Min, box1Max, box2Min, box2Max ) )
			{
				return false;
			}

			/*
			for ( int j = 0; j < 3; ++j )
			{
				D3DXVECTOR3 tempAxis;
				D3DXVec3Cross( &tempAxis, &box1.m_AxisDir[i], &box2.m_AxisDir[j] );

				SATtest( tempAxis, box1.m_PointList, box1Min, box1Max );
				SATtest( tempAxis, box2.m_PointList, box1Min, box1Max );
				if ( !overlaps( box1Min, box1Max, box2Min, box2Max ) )
				{
					return false;
				}
			}
			*/
		}

		// 모든 축에 대해서 겹침이 발생하면 충돌로 판정
		return true;
	}

	/*
	CollisionBox의 복사를 최소화하기 위해 포인터를 이용해 전달하면서 operator 연산은 봉인...
	static bool operator& ( const CollisionBox* lhs, const CollisionBox* rhs )
	{
		return IsCollide( lhs, rhs );
	}
	*/

	/*
	// directx Intersects() 함수를 이용하는 방식으로 대체
	static bool IntersectionCheckRayBox( _Inout_ D3DXVECTOR3* spinAxis, _Inout_ float* distance, const D3DXVECTOR3 &viewDirection, const D3DXVECTOR3 &startPoint, const CollisionBox* box )
	{
		D3DXVECTOR3 tempIntersection( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 intersectionPoint( 0.0f, 0.0f, 0.0f );

		float maxDistance = std::numeric_limits<float>::infinity();
		float minDistance = -std::numeric_limits<float>::infinity();

		for ( int i = 0; i < VECTOR_DIRECTION_3; ++i )
		{
			float dotVal = D3DXVec3Dot( &viewDirection, &box->m_AxisDir[i] );
			if ( dotVal == 0.0f )
			{
				// viewDirection의 출발점을 내적한 값이
				float rayStart = D3DXVec3Dot( &startPoint, &box->m_AxisDir[i] );
				float lowerBound = D3DXVec3Dot( &box->m_PointList[7], &box->m_AxisDir[i] ); // 바로 인덱스로 받아서 써도 될 것 같긴한데
				float upperBound = D3DXVec3Dot( &box->m_PointList[1], &box->m_AxisDir[i] );
				
				// box의 두 점을 내적한 값 사이에 있는지 확인
				if ( !IsBetweenOrdered( rayStart, lowerBound, upperBound ) )
				{
					return false;
				}
			}
			else
			{
				// box.m_AxisDir[i]에 수직인 두 plane을 추출해서 
				// viewDirection과 두 평면의 교차점을 찾는다
				D3DXPLANE lowerPlane, upperPlane;
				D3DXPlaneFromPointNormal( &lowerPlane, &box->m_PointList[7], &box->m_AxisDir[i] );
				D3DXPlaneFromPointNormal( &upperPlane, &box->m_PointList[1], &box->m_AxisDir[i] );

				D3DXVECTOR3 lowerPoint, upperPoint;
				D3DXVECTOR3 endPoint = startPoint + viewDirection * SKILL_RANGE;
				D3DXPlaneIntersectLine( &lowerPoint, &lowerPlane, &startPoint, &endPoint );
				D3DXPlaneIntersectLine( &upperPoint, &upperPlane, &startPoint, &endPoint );

				if ( lowerPoint == NULL || upperPoint == NULL )
					return false;
				
				// 두 점과 viewDrection과의 거리를 구해서
				float tempDistance1, tempDistance2, tempMax, tempMin;
				tempDistance1 = tempDistance2 = tempMax = tempMin = 0.0f;
				
				// D3DXPlaneIntersectLine 함수는 인자로 넣은 라인을 무한대로 확장해서 교점을 찾는다
				// 만약 거의 수평에 가까운 면이 있다면 이 면들과의 교점들의 거리는 아주 큰 값 두개
				// 그 중 하나는 사실 음의 방향이지만 거리는 절대값을 가지므로 둘 다 아주 큰 값을 가지는 한 쌍이 되는 현상 발생
				// 예를 들어 원래 -123 : 124 조합이 123 : 124 조합으로 바뀜 
				// in - in - out - out >>> in - out - in - out
				D3DXVECTOR3 lowerLen = lowerPoint - startPoint;
				D3DXVECTOR3 upperLen = upperPoint - startPoint;
				tempDistance1 = D3DXVec3Length( &lowerLen );
				tempDistance2 = D3DXVec3Length( &upperLen );

				// 결국 아래의 값에 방향을 추가해주어야 한다
				// 좀 더 깔끔한 방법을 찾아야 될텐데...
				if ( lowerPoint.x - startPoint.x  < 0 )
				{
					tempDistance1 *= -1;
				}

				if ( upperPoint.x - startPoint.x  < 0 )
				{
					tempDistance2 *= -1;
				}

				if ( tempDistance1 > tempDistance2 )
				{
					tempMax = tempDistance1;
					tempMin = tempDistance2;
					tempIntersection = upperPoint;
				}
				else
				{
					tempMax = tempDistance2;
					tempMin = tempDistance1;
					tempIntersection = lowerPoint;
				}

				// 각 축에 대해 수직인 면과 viewDirection이 만나는 점들의 거리에 따른 in out은 
				// 다른 축에 대한 점들의 in out 조합 중, 가장 좁은 영역의 안에 있거나 모두 밖에 있어야 한다.
				if ( tempMax > maxDistance && tempMin < minDistance )
				{
					continue;
				}
				else if ( tempMax < maxDistance && tempMin > minDistance )
				{
					// 지금 구한 구간이 더 좁은 경우에만 교체
					maxDistance = ( maxDistance > tempMax ) ? tempMax : maxDistance;
					minDistance = ( minDistance < tempMin ) ? tempMin : minDistance;
					intersectionPoint = tempIntersection;
				}
				else
				{
					return false;
				}
			}
		}
		// 여기서 회전 축을 계산하자
		// 스핀 축은 물체의 원점과 intersectionPoint를 잇는 벡터와, ray 벡터에 수직 - 외적
		if ( spinAxis != nullptr )
		{
			D3DXVECTOR3 centerToIntersection = box->m_CenterPos - intersectionPoint;
			D3DXVec3Cross( spinAxis, &centerToIntersection, &viewDirection );
		}
			

		if ( distance != nullptr )
		{
			D3DXVECTOR3 startpointToIntersection = startPoint - intersectionPoint;
			*distance = D3DXVec3Length( &startpointToIntersection );
		}
			

		return true;
	}
	*/

	static std::array<std::tuple<int, int, int>, 12> BoundingBoxTrianglePoints =
	{
		std::make_tuple( 2, 1, 5 ),
		std::make_tuple( 2, 5, 6 ),
		std::make_tuple( 3, 0, 4 ),
		std::make_tuple( 3, 4, 7 ),
		std::make_tuple( 0, 1, 2 ),
		std::make_tuple( 0, 2, 3 ),
		std::make_tuple( 4, 5, 6 ),
		std::make_tuple( 4, 6, 7 ),
		std::make_tuple( 3, 2, 6 ),
		std::make_tuple( 3, 6, 7 ),
		std::make_tuple( 0, 1, 5 ),
		std::make_tuple( 0, 5, 4 )
	};

	static bool IntersectionCheckRayBox( _Inout_ D3DXVECTOR3* spinAxis, _Inout_ float* distance, _Inout_ D3DXVECTOR3* normalVec, const D3DXVECTOR3 &viewDirection, const D3DXVECTOR3 &startPoint, const CollisionBox* box )
	{
		/*
			BOOL D3DXIntersectTri(
				_In_   const D3DXVECTOR3 *p0,
				_In_   const D3DXVECTOR3 *p1,
				_In_   const D3DXVECTOR3 *p2,
				_In_   const D3DXVECTOR3 *pRayPos,
				_In_   const D3DXVECTOR3 *pRayDir,
				_Out_  FLOAT *pU,
				_Out_  FLOAT *pV,
				_Out_  FLOAT *pDist
				);

			V1 + U(V2 - V1) + V(V3 - V1).
		*/
		bool intersectionFlag = false;
		D3DXVECTOR3 intersectionPoint( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 normalVector( 0.0f, 0.0f, 0.0f );
		float intersectionDistance = std::numeric_limits<float>::infinity();

		for ( int i = 0; i < 12; ++i )
		{
			float tempU, tempV, tempDist;
			tempU = tempV = 0.0f;
			tempDist = std::numeric_limits<float>::infinity();

			if ( D3DXIntersectTri(
					&box->m_PointList[std::get<0>( BoundingBoxTrianglePoints[i] )],
					&box->m_PointList[std::get<1>( BoundingBoxTrianglePoints[i] )],
					&box->m_PointList[std::get<2>( BoundingBoxTrianglePoints[i] )],
					&startPoint, &viewDirection,
					&tempU, &tempV, &tempDist ) 
				)
			{
				intersectionFlag = true;

				// 겹치는 점이 있으면 일단 거리 비교
				if ( tempDist < intersectionDistance )
				{
					intersectionDistance = tempDist;
					intersectionPoint = 
						box->m_PointList[std::get<0>( BoundingBoxTrianglePoints[i] )] 
						+ box->m_PointList[std::get<1>( BoundingBoxTrianglePoints[i] )] * tempU
						+ box->m_PointList[std::get<2>( BoundingBoxTrianglePoints[i] )] * tempV;

					normalVector = box->m_AxisDir[i / 4];
				}
			}
		}

		// 교차하지 않음
		if ( !intersectionFlag )
			return false;

		// 스핀 축은 물체의 원점과 intersectionPoint를 잇는 벡터와, ray 벡터에 수직 - 외적
		if ( spinAxis != nullptr )
		{
			D3DXVECTOR3 centerToIntersection = box->m_CenterPos - intersectionPoint;
			D3DXVec3Cross( spinAxis, &centerToIntersection, &viewDirection );
		}

		// 교차점까지의 거리
		if ( distance != nullptr )
		{
			*distance = intersectionDistance;
		}

		// 교차한 면의 수직 벡터
		if ( normalVec != nullptr )
		{
			*normalVec = normalVector;
		}

		return intersectionFlag;
	}

	// 반사벡터를 구해서 반환
	static D3DXVECTOR3 GetReflectionVector( const D3DXVECTOR3& srcVec, const D3DXVECTOR3& normalVec )
	{
		D3DXMATRIX reflectionTransform;
		D3DXMatrixRotationAxis( &reflectionTransform, &normalVec, D3DX_PI );

		D3DXVECTOR4 tempVector;
		D3DXVec3Transform( &tempVector, &srcVec, &reflectionTransform );

		D3DXVECTOR3 reflectionVector = -tempVector;

		return reflectionVector;
	}

	// 인자로 주어진 box가 direction 벡터에 대해서 shelter라는 객체들에 의해서 가려지는지 확인
	static bool IsCovered( const CollisionBox *box, const D3DXVECTOR3& direction, std::vector<const CollisionBox*> shelters )
	{
		// box의 각 점들에 대해서
		for ( int i = 0; i < BOX_POINT_COUNT; ++i )
		{
			bool isCoveredPoint = false;
			std::for_each( shelters.begin(), shelters.end(), [&]( const CollisionBox* each )
			{
				// 가려지는지 파악 - 가려지면 continue
				if ( isCoveredPoint || IntersectionCheckRayBox( nullptr, nullptr, nullptr, direction, box->m_PointList[i], each ) )
				{
					// 가려진다. 다음 것들은 확인 안 해도 된다.
					isCoveredPoint = true;
				}
			} 
			);

			// 가려지지 않는 점이 하나라도 있으면 false 반환
			if ( !isCoveredPoint )
				return false;
		}

		return true;
	}
};


	