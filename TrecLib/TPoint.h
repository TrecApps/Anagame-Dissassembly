#pragma once
#include "TObject.h"
#include "TrecLib.h"

/**
 * Class: TPoint
 * Purpose: Basic class holding information about a 2D point
 * 
 * SuperClass: TObject - allows it to be used by Environment Scripts
 */
class _TREC_LIB_DLL TPoint :
	public TObject
{
public:
	/**
	 * x: the x-coordinate
	 * y: the y-coordinate
	 */
	double x, y;

	TPoint();

	TPoint(float x, float y);

	TPoint(double x, double y);

	TPoint(const TPoint& point);

	TPoint(const TPoint* point);

	TPoint operator=(const TPoint& point);
};

