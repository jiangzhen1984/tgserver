#include "../tg_util.h"
#include <assert.h>

int main(int argc, char ** argv)
{
	char * source="ssdfsdf/wefw.so";
	int pos=lastFindSeparator(source,"/");
	printf(" %s find '%s'  pos:%d\n",source,"/",pos);
	assert(pos==7);
	
	
	char * source1="ssdfsdf/";
	pos=lastFindSeparator(source1,"/wsfwefwefwewef");
	printf(" %s find '%s'  pos:%d\n",source1,"/wsfwefwefwewef",pos);
	assert(pos==-1);

	char * source2="/ssdfsdf/";
	pos=lastFindSeparator(source2,"/");
	printf(" %s find '%s'  pos:%d\n",source2,"/",pos);
	assert(pos==8);

	char * source3="/";
	pos=lastFindSeparator(source3,"/");
	printf(" %s find '%s'  pos:%d\n",source3,"/",pos);
	assert(pos==0);

	char * source4="/ewfwe/";
	pos=lastFindSeparator(source3,"1231");
	printf(" %s find '%s'  pos:%d\n",source4,"1231",pos);
	assert(pos<0);

	printf("====================================\n");
	char * testName="Sdfsdf/sdf.so";
	char * libName=0;
	getLibName(testName,&libName);
	printf("%s\n",libName);
	assert(strcmp(libName,"sdf.so")==0);


	printf("====================================\n");

	char * source6="ssdfsdf/";
	pos=findSeparator(source6,"/");
	printf(" %s find '%s'  pos:%d\n",source6,"/",pos);
	assert(pos==7);

	char * source7="/ssdfsdf/";
	pos=findSeparator(source7,"/");
	printf(" %s find '%s'  pos:%d\n",source7,"/",pos);
	assert(pos==0);

	char * source10="/";
	pos=findSeparator(source10,"/");
	printf(" %s find '%s'  pos:%d\n",source10,"/",pos);
	assert(pos==0);

	char * source8="/ewfwe/";
	pos=findSeparator(source8,"wsowefewowefj");
	printf(" %s find '%s'  pos:%d\n",source8,"wsowefewowefj",pos);
	assert(pos==-1);

	char * source9="/ewfwe=234";
	pos=findSeparator(source9,"z=");
	printf(" %s find '%s'  pos:%d\n",source9,"z=",pos);
	assert(pos==-1);

}
