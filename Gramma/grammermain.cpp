// grammermain.cpp
// 语法分析器测试主程序

#include "grammer.h"

int main(int argc, char *argv[])
{
	grammer_class  grammer;
	if (argc < 2)
	{	
		printf("please input Source File !\n" );	
		return 0;
	}
	grammer.grammer(argv[1]);
}
