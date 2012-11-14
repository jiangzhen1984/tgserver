
#ifndef HEADER_H
#define HEADER_H

#include "Point.h"
#include "Line.h"

class ShmHeader{

public:
	ShmHeader();

	~ShmHeader();
	
	void setPointsCount(int count)
	{
		mPointsCount=count;
	}

	void setLinesCount(int count)
	{
		mLinesCount=count;
	}
	
	void setPointHeader(PPoint * pPoint);

	void setLineHeader(PLine * pLine);
	PPoint * getPHeader() { return pPointsHeader;}
	PLine * getLHeader() { return pLinesHeader;}

	int getPC(){ return mPointsCount;}
	int getLC(){ return mLinesCount;}

private:
	PPoint * pPointsHeader;
	PLine * pLinesHeader;
	int mPointsCount;
	int mLinesCount;

};



#endif

