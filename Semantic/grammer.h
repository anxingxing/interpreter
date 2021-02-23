// ----------------------------- grammer.h ---------------------------------

#include "lexical.h"

//--------------------------------- 语法分析器类中的类型定义
typedef double (* func_ptr)(double);
typedef struct tree_node				// 语法树节点类型
{	enum token_type op_code;			// PLUS, FUNC, CONST_ID, ...
	union
	{	struct { tree_node *left, *right; } tag_op;
		struct { tree_node *child; func_ptr math_func_ptr; } tag_func;
		double tag_const;
		double * tag_parameter;
	} content;
} * tree_node_ptr;

//--------------------------------- 语法分析器类声明
class grammer_class {

protected:
	double    	parameter;	// 参数T的存储空间
	int       	line_color;	// 线条颜色
	token_rec 	token;		// 记号
	tree_node_ptr 	start_ptr,	// 绘图起点表达式的语法树
			end_ptr, 	// 绘图终点表达式的语法树
			step_ptr,	// 步长表达式的语法树
			x_ptr,		// 点的横坐标表达式的语法树
			y_ptr,		// 点的横坐标表达式的语法树
			angle_ptr;	// 旋转角度表达式的语法树
	lexical_class 	lexical;	// 词法分析器对象

	// --------------- 辅助函数
	void fetch_token ();						// 获取记号
	void match_token (enum token_type the_token);			// 匹配记号
	void syntax_error (int case_of);				// 指出语法错误(调用error_msg)
	void print_syntax_tree(tree_node *root, int indent);		// 打印语法树
	tree_node_ptr make_tree_node(enum token_type opcode,...);	// 构造语法树
	virtual void error_msg(int line, char *descrip, char *string);	// 在Semantics中重置为窗口打印形式


	// ---------------非终结符的递归子程序
	void program();
	void statement();
	void color_statement();
	void colors();
	// 下述四个函数在Semantics_class中需重置，以实现语法制导翻译
	virtual void for_statement();	
	virtual void origin_statement();
	virtual void rot_statement();
	virtual void scale_statement();
	tree_node_ptr expression();
	tree_node_ptr term();
	tree_node_ptr factor();
	tree_node_ptr component();
	tree_node_ptr atom();

public:
	grammer_class() 				// 对象声明时需置初值
	{
		parameter = 0;
		line_color= red_color;
		start_ptr = NULL;
		end_ptr   = NULL;
		step_ptr  = NULL;
		x_ptr	  = NULL;
		y_ptr	  = NULL;
	}
	~grammer_class() {}
	void grammer(char * file_name);		// 语法分析器接口

private:
	// 下述函数用于语法分析器的跟踪调试, 在Semantics_class中需重置为不起作用
	virtual void enter(char * x);
    	virtual void back(char * x);
    	virtual void call_match(char * x); 
    	virtual void tree_trace(tree_node_ptr x);
};
