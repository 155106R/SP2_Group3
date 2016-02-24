#ifndef AABB_H
#define AABB_H

#include "Vector3.h"


struct AABB{
	Vector3 m_origin;
	Vector3 m_vecMax;
	Vector3 m_vecMin;

	Vector3 m_velocity;

	float m_height;
	float m_length;
	float m_width;

	static AABB generateAABB(Vector3 minValue, Vector3 maxValue){		//"simple" not overloaded function, but this doesn't have enough parameters that i need/want
		AABB generateNewBox;
		generateNewBox.m_vecMin = minValue;
		generateNewBox.m_vecMax = maxValue;
		return generateNewBox;
	}

	static AABB generateAABB(Vector3 objectOrigin, float length, float height, float width, Vector3 objectVelocity){		//this gets a scaling and origin for the box
		AABB generateNewBox;

		generateNewBox.m_origin = objectOrigin;

		generateNewBox.m_velocity = objectVelocity;

		generateNewBox.m_length = length;
		generateNewBox.m_height = height;
		generateNewBox.m_width = width;

		generateNewBox.m_vecMin = Vector3(
			objectOrigin.x - (length * 0.5),
			objectOrigin.y - (height * 0.5), 
			objectOrigin.z - (width * 0.5)
			);
		generateNewBox.m_vecMax = Vector3(
			objectOrigin.x + (length * 0.5), 
			objectOrigin.y + (height * 0.5), 
			objectOrigin.z + (width * 0.5)
			);

		return generateNewBox;
	}

	static void updateAABB(AABB &updateObject){

		updateObject.m_origin += updateObject.m_velocity;

		updateObject.m_vecMin = Vector3(
			(updateObject.m_origin.x - (updateObject.m_length * 0.5)),
			(updateObject.m_origin.y - (updateObject.m_height * 0.5)),
			(updateObject.m_origin.z - (updateObject.m_width * 0.5))
			);

		updateObject.m_vecMax = Vector3(
			(updateObject.m_origin.x + (updateObject.m_length * 0.5)),
			(updateObject.m_origin.y + (updateObject.m_height * 0.5)),
			(updateObject.m_origin.z + (updateObject.m_width * 0.5))
			);

	}

};

static bool collision(const AABB& hitbox, const Vector3& vecPoint){	//for point within the AABB

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

static bool collision(const AABB& lhsBox, const AABB& rhsBox){		//for AABB to AABB collision
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

//Updated 22/2/2016 - Randall