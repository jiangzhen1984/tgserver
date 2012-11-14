

#define DEFAULT_SERVICE_COUNT 1

#include <assert.h>

#include "../tg_service.h"

void testLoadService()
{
	char * pwd;
	pwd=getenv("PWD");
	char path1[255];
	strcpy(path1,pwd);
	strcat(path1,"/../plugins/log/libbaseLog1.so");

	char path2[255];
	strcpy(path2,pwd);
	strcat(path2,"/../plugins/log/libbaseLog2.so");

	char path3[255];
	strcpy(path3,pwd);
	strcat(path3,"/../plugins/log/libbaseLog3.so");

	char path4[255];
	strcpy(path4,pwd);
	strcat(path4,"/../plugins/log/libbaseLog4.so");

	char path5[255];
	strcpy(path5,pwd);
	strcat(path5,"/testlib/libtest.so");
	
	loadService(path1);
	loadService(path2);
	loadService(path3);
	loadService(path4);
	loadService(path5);
	
	struct TGService * pTG=findServiceI(3);
	assert(pTG!=0);
	
	finish();

	printf("testLoadService() is ok!\n");
	
}

void testUnLoadService()
{
	
	prepare();
	unLoadService();
	assert(g_services==0);

	printf("-------testunloadService() is ok!\n");
}



void testFindServiceI()
{
	printf("[[[[[[[[[[[[[[[[[[testFindServiceI()\n");
	prepare();

	struct TGService * pTG=findServiceI(g_services[0]->sID);
	assert(pTG!=0);

	struct TGService * pTG1=findServiceI(g_services[0]->sID);
	assert(pTG1!=0);


	finish();

	printf("---------------");
	struct TGService * pTG2=findServiceI(1);
	assert(pTG2==0);

	printf("testFindServiceIService() is ok!\n");
}

void testFindServiceN()
{
	printf("[[[[[[[[[[[[[[[[[[testFindServiceN()\n");
	prepare();

	struct TGService * pTG=findServiceN("libbaseLog2.so");
	printf("assert find dddd 'libbaseLog2.so\n");
	assert(pTG!=0);

	struct TGService * pTG1=findServiceN("libbaseLog2.so");
	printf("assert find 'libbaseLog2.so\n");
	assert(pTG1!=0);

	struct TGService * pTG3=findServiceN("libbaseLog.so");
	printf("assert find 'libbaseLog.so\n");
	assert(pTG3==0);

	finish();
	struct TGService * pTG2=findServiceN("wewf");
	printf("assert find 'wewf\n");
	assert(pTG2==0);

	printf("testFindServiceNService() is ok!\n");

}

void testSetServiceCreateParamtersByN()
{
	printf("---------------------------testSetServiceCreateParamterByN()--\n");
	finish();
	prepare();

	struct TGService * pSer=g_services[0];
	struct TGService * pSer2=g_services[1];


	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==1);
	
	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==2);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==3);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==4);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==5);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==6);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==7);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==8);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==9);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==10);

	setServiceCreateParametersByN("libbaseLog1.so","ssss");
	assert(pSer->parCounts==11);

	setServiceCreateParametersByN("libbaseLog2.so","ssss");
	assert(pSer2->parCounts==1);

	setServiceCreateParametersByN("libbasqweqw","ssss");
	assert(pSer2->parCounts==1);
	
	printf("xxxxxxxxxxxxxxxxx\n");
	finish();
	printf("testSetServiceCreateParametersbyN() is ok!\n");
}

void testSetServiceCreateParamters()
{
	printf(" testSetServiceCreateParamters()\n");
	finish();
	prepare();


	struct TGService * pSer=findServiceI(g_services[1]->sID);

	setServiceCreateParameters(pSer,"ssss");
	assert(pSer->parCounts==1);

	setServiceCreateParameters(pSer,"ssss1");
	assert(pSer->parCounts==2);
	setServiceCreateParameters(pSer,"ssss2");
	assert(pSer->parCounts==3);
	setServiceCreateParameters(pSer,"ssss3");
	assert(pSer->parCounts==4);
	setServiceCreateParameters(pSer,"ssss4");
	assert(pSer->parCounts==5);
	setServiceCreateParameters(pSer,"ssss5");
	assert(pSer->parCounts==6);
	setServiceCreateParameters(pSer,"ssss6");
	assert(pSer->parCounts==7);
	setServiceCreateParameters(pSer,"ssss7");
	assert(pSer->parCounts==8);
	setServiceCreateParameters(pSer,"ssss8");
	assert(pSer->parCounts==9);
	setServiceCreateParameters(pSer,"ssss9");
	assert(pSer->parCounts==10);
	setServiceCreateParameters(pSer,"ssss");
	assert(pSer->parCounts==11);
	setServiceCreateParameters(pSer,"ssss");
	assert(pSer->parCounts==12);
	setServiceCreateParameters(pSer,"ssss");
	assert(pSer->parCounts==13);
	finish();
	printf("testSetServiceCreateParameters() is ok!\n");
}

// prepare data for testing
void prepare()
{
	char * pwd;
	pwd=getenv("PWD");
	char path1[255];
	strcpy(path1,pwd);
	strcat(path1,"/../plugins/log/libbaseLog1.so");

	char path2[255];
	strcpy(path2,pwd);
	strcat(path2,"/../plugins/log/libbaseLog2.so");

	char path3[255];
	strcpy(path3,pwd);
	strcat(path3,"/../plugins/log/libbaseLog3.so");

	char path4[255];
	strcpy(path4,pwd);
	strcat(path4,"/../plugins/log/libbaseLog4.so");

	
	loadService(path1);
	loadService(path2);
	loadService(path3);
	loadService(path4);
}




void finish()
{
	unLoadService();
}



int main(int argc,char ** argv)
{
	if(argc<2)
	{
		printf(" usage ./services_test current path\n");
		exit(0);
	}
	setenv("PWD",argv[1]);
	setenv("LOG_LEVEL","3");

	testLoadService();	

	testUnLoadService();	

	testFindServiceI();

	testFindServiceN();

	testSetServiceCreateParamters();

	testSetServiceCreateParamtersByN();
}

