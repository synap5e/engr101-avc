//
//    FILE: RunningMedian.cpp
//  AUTHOR: Rob dot Tillaart at gmail dot com  
// VERSION: 0.1.02
// PURPOSE: RunningMedian library for Arduino
//
// HISTORY:
// 0.1.00 - 2011-02-16 initial version
// 0.1.01 - 2011-02-22 added remarks from CodingBadly
// 0.1.02 - 2012-03-15 
//
// Released to the public domain
//

#include "RunningMedian.h"

RunningMedian::RunningMedian(uint8_t size)
{
	_size = constrain(size, MEDIAN_MIN, MEDIAN_MAX);
	clear();
}

RunningMedian::RunningMedian()
{
	_size = 5; // default size
	clear();
}

// resets all counters
void RunningMedian::clear()
{ 
	_cnt = 0;
	_idx = 0;
}

// adds a new value to the data-set
// or overwrites the oldest if full.
void RunningMedian::add(float value)
{
	_ar[_idx++] = value;
	if (_idx >= _size) _idx = 0; // wrap around
	if (_cnt < _size) _cnt++;
}

float RunningMedian::getMedian()
{
	if (_cnt > 0)
	{
		sort();
		return _as[_cnt/2];
	}
	return NAN;
}


float RunningMedian::getHighest()
{
	if (_cnt > 0)
	{
		sort();
		return _as[_cnt-1];
	}
	return NAN;
}

float RunningMedian::getLowest()
{
	if (_cnt > 0)
	{	sort();
		return _as[0];
	}
	return NAN;
}

float RunningMedian::getAverage()
{
	if (_cnt > 0)
	{
		float sum = 0;
		for (uint8_t i=0; i< _cnt; i++) sum += _ar[i];
		return sum / _cnt;

	}
	return NAN;
}

void RunningMedian::sort()
{
	// copy
	for (uint8_t i=0; i< _cnt; i++) _as[i] = _ar[i];

	// sort all
	for (uint8_t i=0; i< _cnt-1; i++) 
	{
		uint8_t m = i;
		for (uint8_t j=i+1; j< _cnt; j++)
		{
			if (_as[j] < _as[m]) m = j;
		}
		if (m != i)
		{
			long t = _as[m];
			_as[m] = _as[i];
			_as[i] = t;
		}
	}
}
// END OF FILE