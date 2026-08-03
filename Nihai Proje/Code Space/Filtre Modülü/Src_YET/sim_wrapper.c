#include "Filter_Core.h"
#include <stdlib.h>

__declspec(dllexport) Filter_System_t* Filter_Create() {
    Filter_System_t* sys = (Filter_System_t*)malloc(sizeof(Filter_System_t));
    if (sys) {
        Filter_Init(sys);
    }
    return sys;
}

__declspec(dllexport) void Filter_SetSensors(Filter_System_t* sys, 
    float ax, float ay, float az,
    float gx, float gy, float gz,
    float mx, float my, float mz,
    float baro_p, float baro_t,
    double gps_lat, double gps_lon, float gps_alt, float gps_course, float gps_speed,
    uint32_t time_us) 
{
    if (!sys) return;
    
    sys->dataC.acc.x.rawValue = ax;
    sys->dataC.acc.y.rawValue = ay;
    sys->dataC.acc.z.rawValue = az;
    sys->dataC.acc.UpdateTime = time_us;

    sys->dataC.gyro.x.rawValue = gx;
    sys->dataC.gyro.y.rawValue = gy;
    sys->dataC.gyro.z.rawValue = gz;
    sys->dataC.gyro.UpdateTime = time_us;

    sys->dataC.mag.x.rawValue = mx;
    sys->dataC.mag.y.rawValue = my;
    sys->dataC.mag.z.rawValue = mz;
    sys->dataC.mag.UpdateTime = time_us;

    sys->dataC.baro.press.rawValue = baro_p;
    sys->dataC.baro.temp.rawValue = baro_t;
    sys->dataC.baro.UpdateTime = time_us;

    sys->dataC.gps.x.rawValue = gps_lat;
    sys->dataC.gps.y.rawValue = gps_lon;
    sys->dataC.gps.z.rawValue = gps_alt;
    sys->dataC.gps.course.rawValue = gps_course;
    sys->dataC.gps.speed.rawValue = gps_speed;
    sys->dataC.gps.UpdateTime = time_us;
}

__declspec(dllexport) void Filter_Step(Filter_System_t* sys, uint32_t time_us) {
    if (sys) {
        Filter_Update(sys, time_us);
    }
}

__declspec(dllexport) void Filter_GetAttitude(Filter_System_t* sys, float* pitch, float* roll, float* yaw) {
    if (sys) {
        if (pitch) *pitch = sys->dataC.estimated.pitch.value;
        if (roll) *roll = sys->dataC.estimated.roll.value;
        if (yaw) *yaw = sys->dataC.estimated.yaw.value;
    }
}
