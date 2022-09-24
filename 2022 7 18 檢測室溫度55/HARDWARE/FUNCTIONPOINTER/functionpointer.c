#include "functionpointer.h"


//构建一个枚举体
enum{
	OPER_ADD = 0,
	OPER_SUB,
	OPER_MUL,
	OPER_DIV,
	OPER_NUM			
};

int add(int a,int b)
{	
	return (a + b);
}

int sub(int a,int b)
{
	return (a - b);
}

int mul(int a,int b)
{
	return (a * b);
}

int div(int a,int b)
{
	return (int)(a / b);
}

static const fun_t oper_table[OPER_NUM] = {
	add,
	sub,
	mul,
	div
};

int main1(int argc ,char **argv)
{	
	int a , b , result;
	a = 100;
	b = 20;
	
	/* use table operation : Add */
	result = oper_table[OPER_ADD](a,b);
	
	
	/* use table operation : Sub */
	result = oper_table[OPER_SUB](a,b);
	//printf("Table operation : %d + %d = %d\n" , a , b , result);
	
	/* use table operation : Multiply */
	result = oper_table[OPER_MUL](a,b);
	//printf("Table operation : %d + %d = %d\n" , a , b , result);
	
	/* use table operation : Divide */
	result = oper_table[OPER_DIV](a,b);
//	printf("Table operation : %d + %d = %d\n" , a , b , result);
	
	return 0;
}
//App_OS_IdleTaskHook  ,空闲任务;        OS_AppIdleTaskHookPtr  空闲任务函数指针
int fun(int a ,int b)
{
	 int c;
   c=a+b;	
	return c;
}
void  functionA(void)
{
	int A,B;int d;
	 p=fun;
	 d=(p)(A,B);
}



//数值函数