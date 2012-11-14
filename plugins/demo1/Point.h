

#ifndef PLUGIN_POINT_H
#define PLUGIN_POINT_H


class PPoint{

public:
	PPoint(int x,int y)
	{
		mX=x;
		mY=y;
	}

	~PPoint()
	{
		mX=0;
		mY=0;
	}

	int getX()
	{
		return mX;
	}

	int getY()
	{
		return mY;
	}
	
	void setX(int x){ mX=x;}
	void setY(int y){ mY=y;}

private:
	int mX;
	int mY;

};

#endif
