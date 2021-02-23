// ----------------------------- lexical.cpp词法分析中的函数定义 ---------------------------------
// 词法分析器类的定义

#include "lexical.h" 

// ---------------初始化词法分析器
int lexical_class::init_lexical(const char *file_name)
{
	file_name="test2.txt";
	//file_name="test5.txt";
	in_flie = fopen(file_name, "r"); //只读形式打开文件
	if (in_flie != NULL)  
		return 1;        //打开成功
	else                  
		return 0;        //打开失败
}

// ---------------关闭词法分析器
void lexical_class::close_lexical(void)
{
	if (in_flie != NULL) 
		fclose (in_flie) ;        //关闭文件
}

// ---------------从输入源程序中读入一个字符
char lexical_class::get_char(void)
{
	int next_char = getc(in_flie);   //逐个字符读
	return toupper(next_char);       //全部转换为大写，然后return
}

// ---------------把预读的字符退回到输入源程序中
void lexical_class::back_char(char next_char)
{
	if (next_char != EOF)   
		ungetc(next_char, in_flie);
}

// ---------------加入字符到记号缓冲区
void lexical_class::add_in_token_str(char next_char)
{
	int token_len = strlen (token_buf);
	if (token_len + 1 >= sizeof (token_buf))    //数组越界
		return;
	token_buf[token_len]   = next_char;      //写入字符到缓冲区
	token_buf[token_len+1] = str_end;        //加入字符串结束标志“\0”
}

// ----------------- 清空记号缓冲区
void lexical_class::empty_token_str ()
{	
	memset(token_buf, 0, token_len);
} 

// ----------------- 判断所给的字符串是否在符号表中
token_rec lexical_class::check_token(const char * c_str)
{	
	int count;
	token_rec err_token;
	//sizeof(token_table):表示这个数组一共占了多少字节数;sizeof(token_table[0])：表示一个元素所占的字节数
	//两者相除，表述数组中一共有多少个元素
	for (count=0; count<sizeof(token_table)/sizeof(token_table[0]); count++)
	{ 
		//遍历TokenTab表
		if (strcmp(token_table[count].lexeme, c_str)==0) 
			return token_table[count];        //判断在字符表中就返回该记号
	
	}
	memset(&err_token, 0, sizeof(token_rec));  //先将errortoken置空
	err_token.type = ERRTOKEN;    //然后填入出错记号       
	return err_token;         //返回出错记号
}

// ---------------获取一个记号
token_rec lexical_class::get_token(void)
{	
	token_rec token;
	int next_char ;
	
	memset(&token, 0, sizeof(token_rec));     //token置空
	empty_token_str();            //清空记号缓冲区
	token.lexeme = token_buf;
	for (;;)	// 过滤源程序中的空格、TAB、回车等，遇到文件结束符返回空记号
	{	
		next_char = get_char() ;     //从源程序读字符
		if (next_char == EOF)      //读出错
		{	token.type = NONTOKEN;
			return token;
		}
		if (next_char == '\n')	
			line_no ++ ;      //行号+1
		if (!isspace(next_char))     	//遇到空格该记号肯定已经完成，退出循环
			break ;
	} // end of for 
	add_in_token_str (next_char);	// 若不是空格、TAB、回车、文件结束符等，
					// 则先加入到记号的字符缓冲区中
	if(isalpha(next_char))		// 若char是A-Za-z，则它一定是函数、关键字、PI、E等
	{	for (;;)
		{	next_char = get_char () ;
			if ( isalnum(next_char) )	//如果是字母或数字
				add_in_token_str (next_char) ;  //加入缓冲区
			else				
				break ;
		}
		back_char (next_char) ;   //退回缓冲区
		token = check_token (token_buf);  //判断是否在记号表中
		token.lexeme = token_buf;
		return token;
	}	
	else if(isdigit(next_char))		// 若是一个数字，则一定是常量
	{	for (;;)
		{	next_char = get_char () ;
			if (isdigit(next_char))	add_in_token_str (next_char) ;
			else			break ;
		}
		if (next_char == '.')
		{	add_in_token_str (next_char) ;
			for (;;)
			{	next_char = get_char() ;
				if (isdigit(next_char))	
					add_in_token_str (next_char) ;
				else			
					break ;
			}
			
		} // end of if (next_char == '.')
		back_char (next_char) ;
		token.type = CONST_ID;
		token.value = atof (token_buf);
		return token;
	}
	else	// 不是字母和数字，则一定是符号
	{	switch (next_char)
		{	case ';' : token.type = SEMICO    ; break;
			case '(' : token.type = L_BRACKET ; break;
			case ')' : token.type = R_BRACKET ; break;
			case ',' : token.type = COMMA     ; break;
			case '+' : token.type =	PLUS      ; break;
			case '-' :
				next_char = get_char();
				if (next_char =='-')
				{	while (next_char != '\n' && next_char != EOF) next_char = get_char();
					back_char(next_char);
					return get_token();
				}
				else 
				{	back_char(next_char);
					token.type = MINUS;
					break;
				}
			case '/' :
				next_char = get_char();
				if (next_char =='/') 
				{	while (next_char != '\n' && next_char != EOF) next_char = get_char();
					back_char(next_char);
					return get_token();
				}
				else
				{	back_char(next_char);
					token.type = DIV;
					break;
				}
			case '*' :
				next_char = get_char() ;
				if (next_char == '*') 
				{   token.type = POWER ;
				    add_in_token_str (next_char);
				}
				else 
				{	back_char (next_char);	
					token.type = MUL; 
				}
				break;
			default: 
				token.type = ERRTOKEN; 
		} // end of switch
	} // end of else(不是字母和数字，则一定是符号)
	return token;
} // end of get_token
