// ----------------------------- lexical.h ---------------------------------
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
using namespace std;

//--------------------------------- 词法分析器类中的类型定义与常量声明
enum token_type						// 记号种类
{	ORIGIN, SCALE,	ROT, IS,  TO,			// 保留字
	STEP,   DRAW,	FOR,	FROM,			// 保留字
	COLOR,	RED,	BLACK,				// 保留字
	T,						// 参数
	SEMICO,L_BRACKET, R_BRACKET, COMMA,		// 分隔符号
	PLUS, MINUS, MUL, DIV, POWER,			// 运算符
	FUNC,						// 函数
	CONST_ID,					// 常数
	NONTOKEN,					// 空记号
	ERRTOKEN					// 出错记号
};
enum color_type { red_color, black_color };		// 线条颜色
struct token_rec					// 记号与符号表结构
{	token_type  	type;				// 记号的类别
	char		* lexeme;			// 构成记号的字符串
	double		value;				// 若为常数，则是常数的值
	double		(* func_ptr)(double);		// 若为函数，则是函数的指针
};
static token_rec token_table[] =			// 符号表内容
{	{CONST_ID,	"PI",		3.1415926,	NULL},
	{CONST_ID,	"E",		2.71828,	NULL},
	{T,		"T",		0.0,		NULL},
	{FUNC,		"SIN",		0.0,		sin},
	{FUNC,		"COS",		0.0,		cos},
	{FUNC,		"TAN",		0.0,		tan},
	{FUNC,		"LN",		0.0,		log},
	{FUNC,		"EXP",		0.0,		exp},
	{FUNC,		"SQRT",		0.0,		sqrt},
	{ORIGIN,	"ORIGIN",	0.0,		NULL},
	{SCALE,		"SCALE",	0.0,		NULL},
	{COLOR,		"COLOR",	0.0,		NULL},
	{RED,		"RED",		0.0,		NULL},
	{BLACK,		"BLACK",	0.0,		NULL},
	{ROT,		"ROT",		0.0,		NULL},
	{IS,		"IS",		0.0,		NULL},
	{FOR,		"FOR",		0.0,		NULL},
	{FROM,		"FROM",		0.0,		NULL},
	{TO,		"TO",		0.0,		NULL},
	{STEP,		"STEP",		0.0,		NULL},
	{DRAW,		"DRAW",		0.0,		NULL}
};
const int  token_len = 100;		// 记号最大长度
const char str_end   = '\0';		// 字符串结束标志

//--------------------------------- 词法分析器类的声明
class lexical_class {							
protected:
	FILE *in_flie;					// 输入文件流
	char token_buf[token_len];			// 记号字符缓冲

public:
	unsigned int line_no;				// 跟踪记号所在源文件行号
	lexical_class()	{ line_no = 1; }
	~lexical_class() {}
	int init_lexical(const char *FileName);		// 初始化词法分析器
	void close_lexical(void);			// 关闭词法分析器
	token_rec get_token(void);			// 获取记号函数

private:
	char get_char(void);				// 从输入源程序中读入一个字符
	void back_char(char next_char);			// 把预读的字符退回到输入源程序中
	void add_in_token_str(char next_char);		// 加入字符到记号缓冲区
	void empty_token_str();				// 清空记号缓冲区
	token_rec check_token(const char *c_str);	// 判断所给的字符串是否在符号表中
};
