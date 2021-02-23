// ----------------------------- semantics.h ---------------------------------

//#include <windows.h>
#include <windows.h>
#include <wingdi.h>
#include "grammer.h"

extern HDC hDC;

//---------------- 函数绘图类的声明, 它是grammer_class的派生类
class semantics_class : public grammer_class {	
protected:
	double 	Origin_x, 
		Origin_y,		// 横、纵平移距离
		Scale_x,  
		Scale_y, 		// 横、纵比例因子
		rot_angle;			// 旋转角度
public:
	semantics_class()			// 声明对象时置下述初值
	{	Origin_x  = 0;	
	    Origin_y = 0;	// 原点是(0, 0)
		Scale_x   = 1;	
		Scale_y  = 1;	// 横、纵坐标比例是1:1
		rot_angle = 0;			// 旋转角度是0
	};
	~semantics_class() {};

private:
	// 线条色彩设定
	const red()   
	{ 
		return RGB(60,179,113);
	};		// 线条颜色：道奇蓝
	const black() 
	{ 
		return RGB(255, 69, 0);
	};			// 黑色

	// 重置基类grammer_class中的方法，以在语法分析的基础上实现语法制导翻译
	void Errmsg (char *string);				// 出错处理
	void error_msg (int line, char *descrip, char *string);	// 出错处理
	void for_statement ();
	void origin_statement();
	void rot_statement();
	void scale_statement();
	void colors();
	// 下述函数在semantics_class中被重置为不起作用，以屏蔽语法分析时的跟踪测试
	void enter();
   	void back();
    void call_match(); 
    void tree_trace();

	// semantics_class中的方法，它们是语法制导翻译中所需的语义函数
    double get_tree_value(tree_node_ptr root);			// 获得表达式的值
	void cal_coord(	tree_node_ptr hor_ptr,				// 计算点的坐标
					tree_node_ptr ver_ptr,
					double &hor_val,
					double &ver_val);
	void draw_pixel(unsigned long x, unsigned long y);		// 绘制一个点
	void draw_loop(	double start_val,				// 图形绘制
					double end_val,
					double step_val,
					tree_node_ptr x_ptr,
					tree_node_ptr y_ptr);
	void delete_tree(tree_node_ptr root);				// 删除一棵树
};
