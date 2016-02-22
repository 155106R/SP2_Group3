#ifndef AABB_H
#define AABB_H

#include "Vector3.h"


struct AABB{
	Vector3 m_origin;
	Vector3 m_vecMax;
	Vector3 m_vecMin;

	float m_height;
	float m_length;
	float m_width;

	static AABB generateAABB(Vector3 minValue, Vector3 maxValue){
		AABB generateNewBox;
		generateNewBox.m_vecMin = minValue;
		generateNewBox.m_vecMax = maxValue;
		return generateNewBox;
	}

	static AABB generateAABB(Vector3 objectOrigin, float length, float height, float width){		//this gets a scaling and origin for the box
		AABB generateNewBox;

		generateNewBox.m_height = height;
		generateNewBox.m_length = length;
		generateNewBox.m_width = width;
		generateNewBox.m_origin = objectOrigin;
		generateNewBox.m_vecMin = Vector3((objectOrigin.x - length), (objectOrigin.y - height), (objectOrigin.z - width) * 0.5);
		generateNewBox.m_vecMax = Vector3((objectOrigin.x + length), (objectOrigin.y + height), (objectOrigin.z + width) * 0.5);
		return generateNewBox;
	}

}spaceshipHitbox, testBox1, testBox2;

bool collision(const AABB& hitbox, const Vector3& vecPoint){	//for point within the AABB

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

bool collision(const AABB& lhsBox, const AABB& rhsBox){		//for AABB to AABB collision
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