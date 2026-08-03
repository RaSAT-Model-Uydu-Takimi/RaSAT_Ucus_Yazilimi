using System;

namespace FilterModule
{
    public struct Confidence_Evaluator_t
    {
        public float last_acc_x, last_acc_y, last_acc_z;
        public float last_gyro_x, last_gyro_y, last_gyro_z;
        public float last_mag_x, last_mag_y, last_mag_z;
        public float last_baro_press;
    }

    public static class M2_Confidence_Evaluator
    {
        /* -------------------------------------------------------------------------- */
        /*  Dahili: Tek kanal güven değerlendirmesi                                   */
        /* -------------------------------------------------------------------------- */
        private static void EvaluateChannel(float curr_val, ref float last_val, ref float curr_conf,
                                            uint update_time, uint curr_time, float max_spike)
        {
            /* 1. ZAMAN AŞIMI KONTROLÜ */
            if ((curr_time - update_time) > Filter_Config.CONFIDENCE_TIMEOUT_US)
            {
                curr_conf = 0.0f;
                return;
            }

            /* 2. FİZİKSEL SIÇRAMA (SPIKE) KONTROLÜ */
            float delta = Math.Abs(curr_val - last_val);
            if (delta > max_spike)
            {
                /* İmkansız sıçrama → güveni cezalandır */
                curr_conf = curr_conf * Filter_Config.CONFIDENCE_SPIKE_PENALTY_FACTOR;
            }
            else
            {
                /* Veri sağlıklı → güveni toparla */
                curr_conf += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                if (curr_conf > 1.0f)
                {
                    curr_conf = 1.0f;
                }
                last_val = curr_val;
            }

            /* 3. MİNİMUM GÜVEN EŞİĞİ */
            if (curr_conf < Filter_Config.CONFIDENCE_MIN_VALID)
            {
                curr_conf = 0.0f;
            }
        }

        /* ========================================================================== */
        /*  CE_Init                                                                   */
        /* ========================================================================== */
        public static void CE_Init(ref Confidence_Evaluator_t eval)
        {
            eval.last_acc_x = 0.0f;
            eval.last_acc_y = 0.0f;
            eval.last_acc_z = Filter_Config.GRAVITY_MPS2;   /* Statik durumda Z ≈ 9.81 m/s² */
            eval.last_gyro_x = 0.0f;
            eval.last_gyro_y = 0.0f;
            eval.last_gyro_z = 0.0f;
            eval.last_mag_x = 0.0f;
            eval.last_mag_y = 0.0f;
            eval.last_mag_z = 0.0f;
            eval.last_baro_press = Filter_Config.FILTER_SEA_LEVEL_PA_DEFAULT;
        }

        /* ========================================================================== */
        /*  CE_Update                                                                 */
        /*  calibratedValue birimleri: ivme=m/s², gyro=rad/s, baro=Pa                 */
        /* ========================================================================== */
        public static void CE_Update(ref Confidence_Evaluator_t eval, ref DataCenter dataC, uint curr_time)
        {
            /* --- 1. İVMEÖLÇER KANALLARI (m/s²) --- */
            EvaluateChannel(dataC.acc.x.calibratedValue, ref eval.last_acc_x,
                            ref dataC.acc.x.confidence, dataC.acc.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_ACC_MPS2);
            EvaluateChannel(dataC.acc.y.calibratedValue, ref eval.last_acc_y,
                            ref dataC.acc.y.confidence, dataC.acc.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_ACC_MPS2);
            EvaluateChannel(dataC.acc.z.calibratedValue, ref eval.last_acc_z,
                            ref dataC.acc.z.confidence, dataC.acc.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_ACC_MPS2);

            /* --- 2. JİROSKOP KANALLARI (rad/s) --- */
            EvaluateChannel(dataC.gyro.x.calibratedValue, ref eval.last_gyro_x,
                            ref dataC.gyro.x.confidence, dataC.gyro.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
            EvaluateChannel(dataC.gyro.y.calibratedValue, ref eval.last_gyro_y,
                            ref dataC.gyro.y.confidence, dataC.gyro.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
            EvaluateChannel(dataC.gyro.z.calibratedValue, ref eval.last_gyro_z,
                            ref dataC.gyro.z.confidence, dataC.gyro.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_GYRO_RADPS);

            /* --- 3. BAROMETRE KANALI (Pa) --- */
            EvaluateChannel(dataC.baro.press.calibratedValue, ref eval.last_baro_press,
                            ref dataC.baro.press.confidence, dataC.baro.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_BARO_PA);

            /* --- 4. MANYETOMETRE (Spike + Timeout kontrolü) --- */
            /* Manyetometre motor girişimi yüzünden anlık sıçramalar (spike) yapabilir. */
            EvaluateChannel(dataC.mag.x.calibratedValue, ref eval.last_mag_x,
                            ref dataC.mag.x.confidence, dataC.mag.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_MAG_UT);
            EvaluateChannel(dataC.mag.y.calibratedValue, ref eval.last_mag_y,
                            ref dataC.mag.y.confidence, dataC.mag.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_MAG_UT);
            EvaluateChannel(dataC.mag.z.calibratedValue, ref eval.last_mag_z,
                            ref dataC.mag.z.confidence, dataC.mag.UpdateTime,
                            curr_time, Filter_Config.CONFIDENCE_MAX_SPIKE_MAG_UT);

            /* --- 5. GPS (Timeout + Fix kontrolü) --- */
            if ((curr_time - dataC.gps.UpdateTime) > Filter_Config.CONFIDENCE_TIMEOUT_US || dataC.gps.fixQuality == 0)
            {
                dataC.gps.x.confidence = 0.0f;
                dataC.gps.y.confidence = 0.0f;
                dataC.gps.z.confidence = 0.0f;
                dataC.gps.speed.confidence = 0.0f;
                dataC.gps.course.confidence = 0.0f;
            }
            else
            {
                /* GPS fix var → güveni toparla */
                dataC.gps.x.confidence += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                dataC.gps.y.confidence += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                dataC.gps.z.confidence += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                dataC.gps.speed.confidence += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                dataC.gps.course.confidence += Filter_Config.CONFIDENCE_RECOVERY_RATE;
                if (dataC.gps.x.confidence > 1.0f)
                {
                    dataC.gps.x.confidence = 1.0f;
                    dataC.gps.y.confidence = 1.0f;
                    dataC.gps.z.confidence = 1.0f;
                    dataC.gps.speed.confidence = 1.0f;
                    dataC.gps.course.confidence = 1.0f;
                }
            }
        }
    }
}
