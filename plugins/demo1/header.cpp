


#include "header.h"

#include <stdlib.h>

ShmHeader::ShmHeader()
{
	pPointsHeader=NULL;
	pLinesHeader=NULL;
	mPointsCount=0;
	mLinesCount=0;
}

ShmHeader::~ShmHeader()
{
	mPointsCount=0;
	mLinesCount=0;
}

void ShmHeader::setPointHeader(PPoint * pPoint)
{
	pPointsHeader=pPoint;
}

void ShmHeader::setLineHeader(PLine * pLine)
{
	pLinesHeader=pLine;
}




