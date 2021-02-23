// ----------------------------- semantics.cpp ---------------------------------
// 函数绘图类的定义

#include "semantics.h"

// ------------------------ 出错处理
void semantics_class::Errmsg (char *string) { exit (1); }
void semantics_class::error_msg(int line, char * descrip, char * string)
{
	const int buf_len=256;
	char msg[buf_len];

	memset(msg, 0, buf_len);
	sprintf(msg, "Line No %3d:  %s %s !", lexical.line_no, descrip, string);
	MessageBox(NULL, msg, "error!", MB_OK);
	lexical.close_lexical();
	exit(1);
}

// ------------------------ 计算被绘制点的坐标
void semantics_class::cal_coord(tree_node_ptr 	x_ptr,
						tree_node_ptr y_ptr,
						double &x_ret,
						double &y_ret)
{	
	double x_val, x_temp, y_val;

	// 计算表达式的值，得到点的原始坐标
	x_val = get_tree_value(x_ptr);
	y_val = get_tree_value(y_ptr);

	// 比例变换
	x_val *= Scale_x ;
	y_val *= Scale_y ;

	// 旋转变换
	x_temp = x_val*cos(rot_angle) + y_val*sin(rot_angle);
	y_val  = y_val*cos(rot_angle) - x_val*sin(rot_angle);
	x_val  = x_temp;
	
	// 平移变换
	x_val += Origin_x;
	y_val += Origin_y;

	// 返回变换后点的坐标
	x_ret = x_val;
	y_ret = y_val;
}

// ------------------------ 循环绘制点坐标
void semantics_class::draw_loop
			(	double 		start_val,
				double 		end_val,
				double 		step_val,
				tree_node_ptr 	x_ptr,
				tree_node_ptr 	y_ptr)
{
	double x_val, y_val;
	for(parameter = start_val; parameter <= end_val; parameter += step_val)
	{	cal_coord(x_ptr, y_ptr, x_val, y_val);
//		y_val = 460-y_val;			// 修改显示屏的直角坐标系 -----------------------------------------------------------演示
		draw_pixel((unsigned long)x_val, (unsigned long)y_val);
	}
}

// ------------------------ 计算表达式的值
double semantics_class::get_tree_value(tree_node_ptr root)
{	
	if (root == NULL) return 0.0;
	switch (root -> op_code)
	{	case PLUS  :
			return get_tree_value(root->content.tag_op.left ) + get_tree_value(root->content.tag_op.right) ;
		case MINUS :
			return get_tree_value(root->content.tag_op.left ) - get_tree_value(root->content.tag_op.right) ;
		case MUL   :
			return get_tree_value(root->content.tag_op.left ) * get_tree_value(root->content.tag_op.right) ;
		case DIV   :
			return get_tree_value(root->content.tag_op.left ) / get_tree_value(root->content.tag_op.right) ;
		case POWER :
			return pow(get_tree_value(root->content.tag_op.left ), get_tree_value(root->content.tag_op.right) );
		case FUNC  :
			return (* root->content.tag_func.math_func_ptr)	(get_tree_value(root->content.tag_func.child) );
		case CONST_ID :
			return root->content.tag_const ;
		case T  :
			return *(root->content.tag_parameter);
		default :
			return 0.0 ;
	}
}

// ------------------------ 删除一棵语法树
void semantics_class::delete_tree(tree_node_ptr root)
{	
	if (root == NULL) return;
	switch (root -> op_code)
	{   case PLUS  :			// 两个孩子的内部节点
	    case MINUS :
	    case MUL   :
	    case DIV   :
	    case POWER :
				delete_tree (root->content.tag_op.left ) ;
				delete_tree (root->content.tag_op.right) ;
				break ;
	    case FUNC  : 			// 一个孩子的内部节点
				delete_tree (root->content.tag_func.child) ;
				break ;
	    default    : 			// 叶子节点
				break ;
	}
	delete(root) ;				// 删除节点
}

// ------------------------ 绘制一个点
void semantics_class::draw_pixel(unsigned long x, unsigned long y)
{
	if (line_color==red_color) {
		SetPixel(hDC, x, y, red());
		SetPixel(hDC, x+1, y, red());
		SetPixel(hDC, x, y+1, red());
		SetPixel(hDC, x+1, y+1, red());
	}
	else {
		SetPixel(hDC, x, y, black());
		SetPixel(hDC, x+1, y, black());
		SetPixel(hDC, x, y+1, black());
		SetPixel(hDC, x+1, y+1, black());
	}
}

// ------------------------ origin_statement的递归子程序
void semantics_class::origin_statement()
{
	grammer_class::origin_statement();		// 语法分析获取语法树（由grammer中的属性保存）

	Origin_x = get_tree_value(x_ptr);		// 根据语法树计算横坐标的平移距离
	delete_tree(x_ptr);

	Origin_y = get_tree_value(y_ptr);		// 根据语法树计算纵坐标的平移距离
	delete_tree(y_ptr);
}

// ------------------------ scale_statement的递归子程序
void semantics_class::scale_statement () 
{
	grammer_class::scale_statement();		// 语法分析获取语法树（由grammer中的属性保存）

	Scale_x = get_tree_value(x_ptr);		// 根据语法树计算横坐标的比例因子
	delete_tree(x_ptr);

	Scale_y = get_tree_value(y_ptr);		// 根据语法树计算纵坐标的比例因子
	delete_tree(y_ptr);
}

// ------------------------ rot_statement的递归子程序
void semantics_class::rot_statement () 
{	
	grammer_class::rot_statement();			// 语法分析获取语法树（由grammer中的属性保存）
	rot_angle = get_tree_value(angle_ptr);		// 根据语法树计算旋转角度
	delete_tree(angle_ptr);
}

// ------------------------ for_statement 的递归子程序
void semantics_class::for_statement () 
{	
	double start_val, end_val, step_val;		// 绘图起点、终点、步长

	grammer_class::for_statement();			// 语法分析获取各表达式的语法树（由grammer中的属性保存）
	
	start_val = get_tree_value(start_ptr);		// 计算起点表达式的值
	end_val = get_tree_value(end_ptr);		// 计算终点表达式的值
	step_val = get_tree_value(step_ptr);		// 计算步长表达式的值

	draw_loop (start_val, end_val, step_val, x_ptr, y_ptr); // 绘图

	delete_tree(start_ptr);					// 释放空间
	delete_tree(end_ptr);
	delete_tree(step_ptr);
	delete_tree(x_ptr);	
	delete_tree(y_ptr);	
}

// ---------------------- 用于语法分析器中的跟踪调试, 此处不再起作用
void semantics_class::enter() {}
void semantics_class::back() {}
void semantics_class::call_match() {} 
void semantics_class::tree_trace() {}

