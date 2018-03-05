#ifndef _STEP_DETECTOR_H
#define _STEP_DETECTOR_H


#include <stdbool.h>
#include <stdint.h>

extern long   StepDetector_tick;

void  StepDetector_Init(void);
void  onSensorChanged(int16_t xValue, int16_t yValue, int16_t zValue);
void  detectorNewStep(float values) ;
bool  detectorPeak(float newValue, float oldValue); 
float peakValleyThread(float value);
float averageValue(float value[], int n);


#endif /* _SPI_DRIVER_H */


