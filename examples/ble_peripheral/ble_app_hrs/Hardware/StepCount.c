/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "StepDetector.h"

/*
* 根据StepDetector传入的步点"数"步子
* */

static int count = 0;
static int mCount = 0;
static long timeOfLastPeak = 0;
static long timeOfThisPeak = 0;


/*
* 连续走十步才会开始计步
* 连续走了9步以下,停留超过3秒,则计数清空
* */

void countStep(void) 
{
        timeOfLastPeak = timeOfThisPeak;
        timeOfThisPeak = StepDetector_tick;
        if (timeOfThisPeak - timeOfLastPeak <= 300L){
            if(count<9){
                count++;
            }else if(count == 9){
                count++;
                mCount += count;
                
            }else{
                mCount++;
                NRF_LOG_INFO("mCount = %d", mCount);
                
            }
        }else{//超时
            count = 1;//为1,不是0
        }

}



void setSteps(int initValue) 
{
        mCount = initValue;
        count = 0;
        timeOfLastPeak = 0;
        timeOfThisPeak = 0;      
}


