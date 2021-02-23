// ----------------------------- grammer.cpp ---------------------------------
// 语法分析器类的定义

#include "grammer.h"

// ------------------------ 通过词法分析器接口get_token获取一个记号-------
void grammer_class::fetch_token()
{	
	token = lexical.get_token () ;
	if (token.type == ERRTOKEN)	
		syntax_error(1); //词法错误
}

// ------------------------ 匹配记号---------------------------
void grammer_class::match_token (enum token_type the_token) 
{	
	if (token.type != the_token) 
		syntax_error(2);  //语法错误
	fetch_token();
}

// ------------------------ 语法错误处理--------------------------
void grammer_class::syntax_error (int case_of) 
{
	switch(case_of)
	{	case 1:	error_msg (lexical.line_no," 非法记号 ", token.lexeme) ;
				break;
		case 2: error_msg (lexical.line_no, token.lexeme, " 不是预期记号") ;
				break;
	}
}

// ------------------------ 打印错误信息---------------
void grammer_class::error_msg(int line, char *descrip, char *string)
{	
	cout << "Line No " << line << ": " << descrip << string << endl;
	lexical.close_lexical();
	exit(1);
}

// ------------------------ 先序遍历并打印表达式的语法树----------
void grammer_class::print_syntax_tree(tree_node_ptr root, int indent)	
{	//将节点分为3类
	int	temp;

	for (temp=1; temp<=indent; temp++) 
		cout << "	";	// 缩进
	switch(root->op_code)					// 打印根节点
	{	case PLUS:      cout << "+" << endl;	break;
		case MINUS:     cout << "-" << endl;	break;
		case MUL:       cout << "*" << endl;	break;
		case DIV:       cout << "/" << endl;	break;
		case POWER:     cout << "**" << endl;	break;
		case FUNC:      cout << root->content.tag_func.math_func_ptr << endl;	break;
		case CONST_ID:  cout <<root->content.tag_const << endl;			break;
		case T:         cout << "T" << endl;					break;
		default:        cout << "非法的树节点!" << endl;			exit(0);
	}
	if(root->op_code == CONST_ID || root->op_code == T) 
		return;		// 叶子节点返回
	if(root->op_code == FUNC)						// 递归打印一个孩子的节点
		print_syntax_tree(root->content.tag_func.child,indent+1);
	else									// 递归打印两个孩子的节点
	{	
		print_syntax_tree(root->content.tag_op.left,  indent+1);
		print_syntax_tree(root->content.tag_op.right, indent+1);
	}
}

// ------------------------ 绘图语言解释器入口（与主程序的外部接口）-----------
void grammer_class::grammer(char * file_name) 
{	
	enter("grammer");
	if(!(lexical.init_lexical(file_name)))		// 初始化词法分析器
	{	
		cout << "打开文件错误 !" << endl;	
		return;	
	}
	fetch_token();					// 获取第一个记号
	program();					// 进入Program递归子程序，递归下降分析
	lexical.close_lexical();			// 关闭词法分析器
	back("grammer");
	return;
}

// ------------------------ program 的递归子程序---------------------
void grammer_class::program ()
{	
	enter("program");
	while (token.type != NONTOKEN)
	{	statement();	
		match_token(SEMICO) ;
	}
	back("program");
}

// ------------------------ statement 的递归子程序---------------------
void grammer_class::statement() //statement有四个候选项
{
	enter("statement");
	switch (token.type)
	{	case ORIGIN :	origin_statement();	break;
		case SCALE  :	scale_statement();	break;
		case ROT    :	rot_statement();	break;
		case FOR    :	for_statement();	break;
		case COLOR  :	color_statement();  	break;
		default     :	syntax_error(2);		
	}
	back("statement");
}

// ------------------------ origin_statement 的递归子程序--------------
void grammer_class::origin_statement()
{
	enter("origin_statement");
	call_match("ORIGIN");		
	match_token (ORIGIN);
	call_match("IS");		
	match_token (IS);
	call_match("(");		
	match_token (L_BRACKET);
				
	x_ptr = expression();  //获取横坐标点平移距离
	call_match(",");		
	match_token (COMMA);
					
	y_ptr = expression();   //获取纵坐标点平移距离
	call_match(")");		
	match_token (R_BRACKET);
	back("origin_statement");
}

// ------------------------ scale_statement 的递归子程序---------------
void grammer_class::scale_statement () 
{
	enter("scale_statement");
	call_match("SCALE");		
	match_token (SCALE);
	call_match("IS");		
	match_token (IS);
	call_match("(");		
	match_token (L_BRACKET);
					
	x_ptr = expression();  //获取横坐标的比例因子
	call_match(",");		
	match_token (COMMA);
					
	y_ptr = expression(); //获取纵坐标的比例因子
	call_match(")");		
	match_token (R_BRACKET);
	back("scale_statement");
}

// ------------------------ rot_statement 的递归子程序-----------------------
void grammer_class::rot_statement () 
{	
	enter("rot_statement");
	call_match("ROT");		
	match_token (ROT);
	call_match("IS");		
	match_token (IS);
					
	angle_ptr = expression();  //获得旋转角度
	back("rot_statement");
}

// ------------------------ color_statement 的递归子程序
void grammer_class::color_statement () 
{	
	enter("color_statement");
	match_token (COLOR);
	match_token (IS);
    	colors();
	back("color_statement");
}

// ------------------------ color_statement 的递归子程序
void grammer_class::colors() 
{	
	enter("colors");
//	printf("red=%d, black=%d, token_type=%d\n", RED, BLACK, token.type);
	if (token.type==RED) line_color = red_color; 
	else if (token.type==BLACK)  line_color = black_color;
	else syntax_error(2);
//	printf("black_color=%d, red_color=%d, line_color=%d\n", black_color, red_color, line_color);
	match_token (token.type);
	back("colors");
}

// ------------------------ for_statement 的递归子程序
//对右部文法符号的展开->遇到终结符号直接匹配，遇到非终结符就调用相应子程序
//ForStatement中唯一的非终结符是Expression，出现在5个不同位置,分别代表循环的起始、终止、步长、横坐标、纵坐标，需要5个树节点指针保存这5棵语法树

void grammer_class::for_statement () 
{	
	enter("for_statement");
	call_match("FOR");			
	match_token (FOR);
    call_match("T");			
	match_token(T);
	call_match("FROM");	
	
	match_token (FROM);				
	start_ptr = expression();	// 构造参数起始表达式语法树
    call_match("TO");			
	match_token (TO);
						
	end_ptr = expression();		// 构造参数终结表达式语法树
    call_match("STEP");			
	match_token (STEP);
						
	step_ptr = expression();	// 构造参数步长表达式语法树
	call_match("DRAW");			
	match_token (DRAW);
	call_match("(");			
	match_token (L_BRACKET);
						
	x_ptr = expression();		// 构造横坐标表达式语法树
	call_match(",");			
	match_token (COMMA);
						
	y_ptr = expression(); 		// 构造纵坐标表达式语法树
	call_match(")");			
	match_token (R_BRACKET);
	back("for_statement");
}

// ------------------------ expression 的递归子程序

//（二元加减运算）表达式Expression的递归子程序，与上边不太相同的是，表达式需要为其构造语法树
//把函数设计为语法树节点的指针，在函数内引进2个语法树节点的指针变量，分别作为Expression左右操作数（Term）的语法树节点指针
//表达式应该是由正负号或无符号开头、由若干个项以加减号连接而成。
tree_node_ptr grammer_class::expression()
{	
	tree_node_ptr left, right;			// 左右子树节点的指针
	token_type token_tmp;				// 当前记号

    enter("expression");
	left = term();					// 分析左操作数且得到其语法树
	while (token.type==PLUS || token.type==MINUS)
	{	token_tmp = token.type;
		match_token(token_tmp);
		right = term();				// 分析右操作数且得到其语法树
		left = make_tree_node(token_tmp, left, right);
							// 构造运算的语法树，结果为左子树
	}
	tree_trace(left);				// 打印表达式的语法树
    back("expression");
	return left;					// 返回最终表达式的语法树
}

// ------------------------ term 的递归子程序
//乘除运算表达式Term的递归子程序
//项是由若干个因子以乘除号连接而成
tree_node_ptr  grammer_class::term()
{	
	tree_node_ptr left, right;
	token_type token_tmp;

    left = factor();
	while (token.type==MUL || token.type==DIV)
	{	token_tmp = token.type;
		match_token(token_tmp);
		right = factor();
		left = make_tree_node(token_tmp, left, right);
	}
	return left;
}

// ------------------------ factor 的递归子程序
//一元加减运算Factor的递归子程序
//因子可能是一个标识符或一个数字，或是一个以括号括起来的子表达式
tree_node_ptr grammer_class::factor ()
{	
	tree_node_ptr left, right;

	if(token.type == PLUS) 			// 匹配一元加运算
	{	match_token(PLUS);
		right = factor();		//  表达式退化为仅有右操作数的表达式
	}
	else if(token.type == MINUS)		// 匹配一元减运算
	{   match_token(MINUS); 		// 表达式转化为二元减运算的表达式
		right = factor();
		left = new tree_node;
		left->op_code = CONST_ID;
		left->content.tag_const = 0.0;
		right = make_tree_node(MINUS, left, right);
	}
	else right = component();		// 匹配非终结符component
	return right;
}

// ------------------------ component 的递归子程序
//幂运算表达式Component的递归子程序
tree_node_ptr grammer_class::component()
{	
	tree_node_ptr left, right;

	left = atom();
	if(token.type == POWER)
	{	match_token(POWER);
		right = component();	// 递归调用component以实现POWER的右结合性质
		left  = make_tree_node(POWER, left, right);
	}
	return left;
}

// ------------------------ atom 的递归子程序
//原子表达式Atom的递归子程序，包括分隔符 函数 常数 参数
tree_node_ptr grammer_class::atom()	
{   
	struct token_rec t=token;
	tree_node_ptr t_ptr, tmp;

	switch (token.type)
	{	case CONST_ID :
			match_token (CONST_ID) ;
			t_ptr = make_tree_node(CONST_ID,t.value);
			break;
		case T:
			match_token(T);
			t_ptr = make_tree_node(T);
			break;
		case FUNC :
			match_token (FUNC);
			match_token (L_BRACKET);
			tmp = expression ();
			t_ptr = make_tree_node(FUNC,t.func_ptr,tmp);
			match_token (R_BRACKET);
			break ;
		case L_BRACKET :
			match_token (L_BRACKET);
			t_ptr = expression ();
			match_token (R_BRACKET);
			break ;
		default :
			syntax_error (2);
	}
	return t_ptr;
}

// ----------------- 生成语法树的一个节点，接收可变参数列表
tree_node_ptr grammer_class::make_tree_node(enum token_type opcode, ...)
{	
	tree_node_ptr t_ptr = new (struct tree_node);		// 分配节点存储空间
	t_ptr->op_code = opcode;				// 接收记号的类别
	va_list arg_ptr ;                    //指向可变函数的参数的指针
	va_start (arg_ptr, opcode);          //初始化va_list变量，第一个参数也就是固定参数为opcode
	switch(opcode)						// 根据记号的类别构造不同的节点
	{	
	   case CONST_ID:					// 常数节点
			t_ptr->content.tag_const = (double)va_arg(arg_ptr, double);
			break;
		case T:						// 参数T节点
			t_ptr->content.tag_parameter = &parameter;
			break;
		case FUNC:					// 函数调用节点
			t_ptr->content.tag_func.math_func_ptr = (func_ptr)va_arg(arg_ptr, func_ptr);
			t_ptr->content.tag_func.child 
					= tree_node_ptr va_arg (arg_ptr, tree_node_ptr);
			break;
		default:					// 二元运算节点
			t_ptr->content.tag_op.left 
					= tree_node_ptr va_arg (arg_ptr, tree_node_ptr);
			t_ptr->content.tag_op.right
					= tree_node_ptr va_arg (arg_ptr, tree_node_ptr);
			break;
	}
	va_end(arg_ptr);        //结束可变参数的获取
	return t_ptr;
}

// ---------------------- 用于语法分析器中的跟踪调试
void grammer_class::enter(char * x)		
{
	cout << "enter in  "    << x << endl; 
}
void grammer_class::back(char * x)		
{ 
	cout << "exit from  "   << x << endl; 
}
void grammer_class::call_match(char * x)		
{ 
	cout << "matchtoken	" << x << endl; 
} 
void grammer_class::tree_trace(tree_node_ptr x)
{
	print_syntax_tree(x, 1); 
}
