#include "TPoint.h"
#include "TString.h"


/**
 * Method: TPoint::TPoint
 * Purpose: Default constructor
 * Parameters: void
 * Returns: New TPoint Object
 */

TPoint::TPoint()
{
	x = y = 0.0f;
}

/**
 * Method: TPoint::TPoint
 * Purpose: Value Constructor
 * Parameters: float x - x-coordinate of the point
 *				float y - ycoordinae of the point
 * Returns: New TPoint Object
 */
TPoint::TPoint(float x, float y)
{
	this->x = x;
	this->y = y;
}

TPoint::TPoint(double x, double y)
{
	this->x = x;
	this->y = y;
}

/**
 * Method: TPoint::TPoint
 * Purpose: Value Constructor
 * Parameters: float x - x-coordinate of the point
 *				float y - ycoordinae of the point
 * Returns: New TPoint Object
 */
TPoint::TPoint(const TPoint& point)
{
	x = point.x;
	y = point.y;
}

/**
 * Method: TPoint::TPoint
 * Purpose: Copy Contructor
 * Parameters:const TPoint& point - reference to the Point to copy from
 * Returns: New TPoint Object
 */
TPoint::TPoint(const TPoint* point)
{
	if (point)
	{
		x = point->x;
		y = point->y;
	}
	else
		x = y = 0.0f;
}

/**
 * Method: TPoint::TPoint
 * Purpose:
 * Parameters: const TPoint* point - pointer to the point to copy from
 * Returns: New TPOint object
 */
TPoint TPoint::operator=(const TPoint& point)
{
	x = point.x;
	y = point.y;
	return *this;
}
