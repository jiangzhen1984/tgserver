

#ifndef PLUGIN_LINE_H
#define PLUGIN_LINE_H


class PLine{

public:

	PLine(int x,int y,int endx,int endy)
	{
		mX=x;
		mY=y;
		mEndY=endx;
		mEndY=endy;
	}

	~PLine()
	{
		mX=0;
		mY=0;
		mEndX=0;
		mEndY=0;
	}

	int getX()
	{
		return mX;
	}

	int getY()
	{
		return mY;
	}
	int getEndX()
	{
		return mEndX;
	}
	int getEndY()
	{
		return mEndY;
	}
	void setX(int x) { mX=x;}

	void setY(int y) { mY=y;}

	void setEndX(int x) { mEndX=x;}

	void setEndY(int y) { mEndY=y;}

private:
	int mX;
	int mY;
	int mEndX;
	int mEndY;

};

#endif
