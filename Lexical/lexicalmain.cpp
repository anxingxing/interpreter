// ----------------------------- lexicalmain.cpp 词法分析主函数 ---------------------------------
// 词法分析器测试主程序

#include "lexical.h"


char *TOKEN_TYPE[] =  {	"RESERVED_WORD", "RESERVED_WORD",	"RESERVED_WORD", "RESERVED_WORD", "RESERVED_WORD" ,			// 保留字
	"RESERVED_WORD" , "RESERVED_WORD" , "RESERVED_WORD" ,	"RESERVED_WORD" ,			                    // 保留字
	"RESERVED_WORD" , "RESERVED_WORD",	"RESERVED_WORD",				                        // 保留字
	"PARAMETER",    // 参数
	"SEMICO", "L_BRACKET", "R_BRACKET", "COMMA",		// 分隔符号
	"PLUS", "MINUS", "MUL", "DIV", "POWER",			    // 运算符
	"FUNC",						// 函数
	"CONST_ID",					// 常数
	"NONTOKEN",					// 空记号
	"ERRTOKEN"
};


int main(int argc, char *argv[])
{
	lexical_class  lexical;
	token_rec token;
	if (argc < 2)
	{	
		printf("please input Source File !\n" );	
		return 0;
	}
    argv[1]="test1.txt";
	if(lexical.init_lexical(argv[1])!=1)            //初始化词法分析器
	{	
		printf("Open Source File Error ! \n");		
		return 0;
	}
	printf("****************词法分析结果表*******************\n");
	printf("   记号类别       字符串        常数值	      指针\n");
    printf("________________________________________________________\n");
	while(1)
	{	
		token = lexical.get_token();			//获得记号
		if(token.type != NONTOKEN)			//打印记号的内容
		{
			printf("|%13s |%10s |%12f |%12x |\n",TOKEN_TYPE[token.type], token.lexeme, token.value, token.func_ptr); 
			//printf("|___________|___________|_____________|_____________|\n");
		}
			
		else	
			break;
	};
		printf("________________________________________________________\n");
	lexical.close_lexical();
	return 0;
}
