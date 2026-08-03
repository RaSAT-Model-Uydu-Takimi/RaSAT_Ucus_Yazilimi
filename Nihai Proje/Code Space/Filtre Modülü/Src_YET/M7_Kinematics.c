#include "M7_Kinematics.h"
#include "Filter_Config.h"
#include <math.h>
#include <stddef.h>

void Kinematics_BodyToEarth(DataCenter *dc) {
    if (dc == NULL) return;

    float ax = dc->acc.x.calibratedValue;
    float ay = dc->acc.y.calibratedValue;
    float az = dc->acc.z.calibratedValue;

    float pitch_rad = dc->estimated.pitch.value * DEG2RAD;
    float roll_rad  = dc->estimated.roll.value  * DEG2RAD;
    float yaw_rad   = dc->estimated.yaw.value   * DEG2RAD;

    float sp = sinf(pitch_rad), cp = cosf(pitch_rad);
    float sr = sinf(roll_rad),  cr = cosf(roll_rad);
    float sy = sinf(yaw_rad),   cy = cosf(yaw_rad);

    /* ZYX Euler rotasyon matrisi ile NED (Dünya) eksenlerine çevirim */
    
    /* X Bileşeni (Kuzey - North) */
    float earth_ax = ax * (cp * cy)
                   + ay * (sr * sp * cy - cr * sy)
                   + az * (cr * sp * cy + sr * sy);

    /* Y Bileşeni (Doğu - East) */
    float earth_ay = ax * (cp * sy)
                   + ay * (sr * sp * sy + cr * cy)
                   + az * (cr * sp * sy - sr * cy);

    /* Z Bileşeni (Aşağı - Down/Up). Burada yaw'dan bağımsız saf pitch ve roll yatırması kullanılır. */
    float earth_az = -ax * sp + ay * sr * cp + az * cr * cp;

    /* Hesaplanan değerleri DataCenter içine kaydet */
    dc->estimated.earth_a_x.value = earth_ax;
    dc->estimated.earth_a_y.value = earth_ay;
    dc->estimated.earth_a_z.value = earth_az;
    
    /* Confidence değerlerini İvme ve Attitude (Oryantasyon) güvenlerinin minimumu olarak belirleyebiliriz */
    float min_acc_conf = dc->acc.x.confidence;
    if (dc->acc.y.confidence < min_acc_conf) min_acc_conf = dc->acc.y.confidence;
    if (dc->acc.z.confidence < min_acc_conf) min_acc_conf = dc->acc.z.confidence;
    
    float att_conf = dc->estimated.pitch.confidence;
    float final_conf = (min_acc_conf < att_conf) ? min_acc_conf : att_conf;
    
    dc->estimated.earth_a_x.confidence = final_conf;
    dc->estimated.earth_a_y.confidence = final_conf;
    dc->estimated.earth_a_z.confidence = final_conf;
}
