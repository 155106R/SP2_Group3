#ifndef AABB_H
#define AABB_H

#include "Vector3.h"


struct AABB{
	Vector3 m_vecMax;
	Vector3 m_vecMin;

	static AABB generateAABB(Vector3 minValue, Vector3 maxValue){
		AABB generateNewBox;
		generateNewBox.m_vecMin = minValue;
		generateNewBox.m_vecMax = maxValue;
		return generateNewBox;
	}

}testBox, testBox1;

bool collision(const AABB& hitbox, const Vector3& vecPoint){

	//Check if the point is less than max and greater than min
	if (vecPoint.x > hitbox.m_vecMin.x && vecPoint.x < hitbox.m_vecMax.x &&
		vecPoint.y > hitbox.m_vecMin.y && vecPoint.y < hitbox.m_vecMax.y &&
		vecPoint.z > hitbox.m_vecMin.z && vecPoint.z < hitbox.m_vecMax.z){
		return true;
	} else{
	//If not, then return false
		return false;
	}
}

bool AABBtoAABB(const AABB& lhsBox, const AABB& rhsBox){
	//Check if lhsBox's max is greater than rhsBox's min and lhsBox's min is less than rhsBox's max
	if (lhsBox.m_vecMax.x > rhsBox.m_vecMin.x &&
		lhsBox.m_vecMin.x < rhsBox.m_vecMax.x &&
		lhsBox.m_vecMax.y > rhsBox.m_vecMin.y &&
		lhsBox.m_vecMin.y < rhsBox.m_vecMax.y &&
		lhsBox.m_vecMax.z > rhsBox.m_vecMin.z &&
		lhsBox.m_vecMin.z < rhsBox.m_vecMax.z){
		return true;
	}
	else{
		//If not, then return false
		return false;
	}
}


#endif;