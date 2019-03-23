#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct pidTag pid;

extern pid * pidNew(float Kp, float Ki, float Kd, float SetPoint);

extern float pidAdjustOutput(pid * pPID, float Input);

extern void pidSet_SetPoint(pid * pPID, float SetPoint);
extern void pidSet_Kp(pid * pPID, float Kp);
extern void pidSet_Ki(pid * pPID, float Ki);
extern void pidSet_Kd(pid * pPID, float Kd);

#endif
