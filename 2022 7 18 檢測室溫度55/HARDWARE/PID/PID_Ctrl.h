#ifndef _PID_CTRL_H_	
#define _PID_CTRL_H_

extern unsigned char PID_Flag;
extern unsigned char PID_Start;
void PID_Init(void);
float PID_Calc(void);


#endif