/* Includes ------------------------------------------------------------------*/
#include "spi_driver.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include <string.h>
#include "math.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "StepDetector.h"
#include "StepCount.h"

/*
* 定时器 为计步算法提供时间基准：
* 1、两个 波峰 到波峰的 时间差 应大于 250ms, 即步频小于 4 Hz
* 2、连续走了9步以下,停留超过3秒,则计数清空
*/

#define STEPDETECTOR_MEAS_INTERVAL                APP_TIMER_TICKS(10) 
APP_TIMER_DEF(m_StepDetector_timer_id);           /**< StepDetector timer. */
long   StepDetector_tick = 0;

/* 重力加速度常量 */
#define  GRAVITY_EARTH         9.80665f 
/* ±2 G 分辨率 */
#define  GSENSOR_RESOLUTION    16384

/*
* 算法的主要部分,检测是否是步点
* 
*/

//存放三轴数据的加速度，单位g 
float oriValues[3] = {0}  ;
int ValueNum = 4;
//用于存放计算阈值的波峰波谷差值
float tempValue[4] = {0};
int tempCount = 0;
//是否上升的标志位
bool isDirectionUp = false;
//持续上升次数
int continueUpCount = 0;
//上一点的持续上升的次数，为了记录波峰的上升次数
int continueUpFormerCount = 0;
//上一点的状态，上升还是下降
bool lastStatus = false;
//波峰值
float peakOfWave = 0;
//波谷值
float valleyOfWave = 0;
//此次波峰的时间
long timeOfThisPeak = 0;
//上次波峰的时间
long timeOfLastPeak = 0;
//当前的时间
long timeOfNow = 0;
//当前传感器的值
float gravityNew = 0;
//上次传感器的值
float gravityOld = 0;
//动态阈值需要动态的数据，这个值用于这些动态数据的阈值
float InitialValue = (float) 1.3;
//初始阈值
float ThreadValue = (float) 2.0;
//波峰波谷时间差
int TimeInterval = 25;  // 250ms





static void StepDetector_tick_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    StepDetector_tick++;
}


void StepDetector_Init(void)
{
    ret_code_t err_code;
    // Create timers.
    err_code = app_timer_create(&m_StepDetector_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                StepDetector_tick_timeout_handler);
    APP_ERROR_CHECK(err_code);
  
    // Start application timers.
    err_code = app_timer_start(m_StepDetector_timer_id, STEPDETECTOR_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

}



void onSensorChanged(int16_t xValue, int16_t yValue, int16_t zValue)
{ 
        oriValues[0]= (xValue /GSENSOR_RESOLUTION)*GRAVITY_EARTH;    
			  oriValues[1]= (yValue /GSENSOR_RESOLUTION)*GRAVITY_EARTH;   
			  oriValues[2]= (zValue /GSENSOR_RESOLUTION)*GRAVITY_EARTH;
  
        gravityNew = (float) sqrt(oriValues[0] * oriValues[0]
                + oriValues[1] * oriValues[1] + oriValues[2] * oriValues[2]);
        detectorNewStep(gravityNew);
        
}


/*
* 检测步子，并开始计步
* 1.传入sersor中的数据
* 2.如果检测到了波峰，并且符合时间差以及阈值的条件，则判定为1步
* 3.符合时间差条件，波峰波谷差值大于initialValue，则将该差值纳入阈值的计算中
* */
void detectorNewStep(float values) 
{
        if (gravityOld == 0) {
            gravityOld = values;
        } else {
            if (detectorPeak(values, gravityOld)) {
                timeOfLastPeak = timeOfThisPeak;
                timeOfNow = StepDetector_tick;
                if (timeOfNow - timeOfLastPeak >= TimeInterval
                        && (peakOfWave - valleyOfWave >= ThreadValue)) 
                {
                    timeOfThisPeak = timeOfNow;
                    /*
                     * 更新界面的处理，不涉及到算法
                     * 一般在通知更新界面之前，增加下面处理，为了处理无效运动：
                     * 1.连续记录10才开始计步
                     * 2.例如记录的9步用户停住超过3秒，则前面的记录失效，下次从头开始
                     * 3.连续记录了9步用户还在运动，之前的数据才有效
                     * */
                    //mStepListeners.countStep();
                    countStep();
                }
                if (timeOfNow - timeOfLastPeak >= TimeInterval
                        && (peakOfWave - valleyOfWave >= InitialValue)) 
                {
                    timeOfThisPeak = timeOfNow;
                    ThreadValue = peakValleyThread(peakOfWave - valleyOfWave);
                }
            }
        }
        gravityOld = values;
}

/*
* 检测波峰
* 以下四个条件判断为波峰：
* 1.目前点为下降的趋势：isDirectionUp为false
* 2.之前的点为上升的趋势：lastStatus为true
* 3.到波峰为止，持续上升大于等于2次
* 4.波峰值大于1.2g (11.76),小于2g (19.6)
* 记录波谷值
* 1.观察波形图，可以发现在出现步子的地方，波谷的下一个就是波峰，有比较明显的特征以及差值
* 2.所以要记录每次的波谷值，为了和下次的波峰做对比
* */
bool detectorPeak(float newValue, float oldValue) 
{
        lastStatus = isDirectionUp;
        if (newValue >= oldValue) {
            isDirectionUp = true;
            continueUpCount++;
        } else {
            continueUpFormerCount = continueUpCount;
            continueUpCount = 0;
            isDirectionUp = false;
        }

        if (!isDirectionUp && lastStatus
                && (continueUpFormerCount >= 2 && (oldValue >= 10.78 && oldValue < 19.6))) {
            peakOfWave = oldValue;
            return true;
        } else if (!lastStatus && isDirectionUp) {
            valleyOfWave = oldValue;
            return false;
        } else {
            return false;
        }
}

/*
* 阈值的计算
* 1.通过波峰波谷的差值计算阈值
* 2.记录4个值，存入tempValue[]数组中
* 3.在将数组传入函数averageValue中计算阈值
* */
float peakValleyThread(float value) 
{
        float tempThread = ThreadValue;
        if (tempCount < ValueNum) {
            tempValue[tempCount] = value;
            tempCount++;
        } else {
            tempThread = averageValue(tempValue, ValueNum);
            for (int i = 1; i < ValueNum; i++) {
                tempValue[i - 1] = tempValue[i];
            }
            tempValue[ValueNum - 1] = value;
        }
        return tempThread;

}

/*
* 梯度化阈值
* 1.计算数组的均值
* 2.通过均值将阈值梯度化在一个范围里
* */
float averageValue(float value[], int n) 
{
        float ave = 0;
        for (int i = 0; i < n; i++) {
            ave += value[i];
        }
        ave = ave / ValueNum;
        if (ave >= 8)
            ave = (float) 4.3;
        else if (ave >= 7 && ave < 8)
            ave = (float) 3.3;
        else if (ave >= 4 && ave < 7)
            ave = (float) 2.3;
        else if (ave >= 3 && ave < 4)
            ave = (float) 2.0;
        else {
            ave = (float) 1.3;
        }
        return ave;
}


