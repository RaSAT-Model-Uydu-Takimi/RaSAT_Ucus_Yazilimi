/*
 * flight_control.c
 *
 */

#include "flight_control.h"

#include "Konfigurasyonlar.h"
#include "pid.h"
#include "motor_mixer.h"
#include "motors.h"

#include <math.h>
#include <stddef.h>


/*=========================================================================
 * PID NESNELERI
 *=========================================================================*/

/* Dikey kontrol PID'leri */
static PID_Controller yukseklik_pid;
static PID_Controller dikey_hiz_pid;

/* Acisal hiz kontrol PID'leri */
static PID_Controller roll_hiz_pid;
static PID_Controller pitch_hiz_pid;
static PID_Controller yaw_hiz_pid;


/*=========================================================================
 * KONTROL HEDEFLERI
 *=========================================================================*/

static float hedef_yukseklik_m =
    HOVER_YUKSEKLIGI_m;

static float hedef_roll_deg = 0.0f;
static float hedef_pitch_deg = 0.0f;
static float hedef_yaw_deg = 0.0f;


/*=========================================================================
 * KONTROL DURUMU
 *=========================================================================*/

static uint32_t onceki_guncelleme_us = 0U;

static uint8_t zaman_baslatildi = 0U;
static uint8_t kontrol_aktif = 0U;
static uint8_t yaw_hedefi_belirlendi = 0U;


/*=========================================================================
 * YARDIMCI FONKSIYONLAR
 *=========================================================================*/

static float Sinirla(float deger,
                     float alt_sinir,
                     float ust_sinir)
{
    if (deger < alt_sinir)
    {
        return alt_sinir;
    }

    if (deger > ust_sinir)
    {
        return ust_sinir;
    }

    return deger;
}


/*
 * Aciyi -180 ile +180 derece arasina getirmek için.
 */
static float Aciyi180AraliginaGetir(
    float aci_deg)
{
    while (aci_deg > 180.0f)
    {
        aci_deg -= 360.0f;
    }

    while (aci_deg < -180.0f)
    {
        aci_deg += 360.0f;
    }

    return aci_deg;
}


/*
 * Bir PID nesnesinin katsayilarini ve
 * cikis sinirlarini ayarlar.
 */
static void PidYapilandir(
    PID_Controller *pid,
    float kp,
    float ki,
    float kd,
    float cikis_min,
    float cikis_max)
{
    if (pid == NULL)
    {
        return;
    }

    *pid = (PID_Controller)
    {
        .Kp = kp,
        .Ki = ki,
        .Kd = kd,

        .integral = 0.0f,
        .prev_error = 0.0f,

        .out_min = cikis_min,
        .out_max = cikis_max,

        .alpha = PID_TUREV_FILTRE_ALPHA,
        .filtered_val = 0.0f,

        .initialized = 0U
    };

    PID_Reset(pid);
}


/*
 * Tum PID gecmislerini sifirlar.
 *
 * Dis aci kontrolleri yalnizca P oldugu icin
 * onlar icin sifirlanacak bir gecmis bulunmaz.
 */
static void TumPidleriSifirla(void)
{
    PID_Reset(&yukseklik_pid);
    PID_Reset(&dikey_hiz_pid);

    PID_Reset(&roll_hiz_pid);
    PID_Reset(&pitch_hiz_pid);
    PID_Reset(&yaw_hiz_pid);
}


#if (MOTOR_MODELI_AKTIF == 1U)

/*
 * Gecerli batarya olcumu varsa onu kullanir.
 * Aksi durumda varsayilan gerilimi dondurur.
 */
static float BataryaGeriliminiBul(
    const DataCenter *veri_merkezi)
{
    if (veri_merkezi == NULL)
    {
        return BATARYA_VARSAYILAN_GERILIM_V;
    }

    if ((veri_merkezi->batt.battVolt.confidence >=
         BATARYA_MIN_GUVEN) &&
        (veri_merkezi->batt.battVolt.filteredValue >
         1.0f))
    {
        return
            veri_merkezi->
            batt.battVolt.filteredValue;
    }

    return BATARYA_VARSAYILAN_GERILIM_V;
}

#endif


/*
 * Hover icin tahmini temel throttle degerini bulur.
 */
static float TemelThrottleHesapla(
    const DataCenter *veri_merkezi)
{
#if (MOTOR_MODELI_AKTIF == 1U)

    float batarya_gerilimi_v;
    float motor_basina_itki_n;
    float gerekli_rpm;
    float maksimum_rpm;
    float temel_throttle;

    batarya_gerilimi_v =
        BataryaGeriliminiBul(veri_merkezi);

    motor_basina_itki_n =
        (ARAC_KUTLESI_KG *
         YERCEKIMI_IVMESI_MPS2) /
        4.0f;

    if ((MOTOR_ITKI_KATSAYISI_N_RPM2 <= 0.0f) ||
        (batarya_gerilimi_v <= 0.0f))
    {
        return Sinirla(
            TEMEL_THROTTLE,
            ORTAK_THROTTLE_MIN,
            ORTAK_THROTTLE_MAX
        );
    }

    /*
     * F = kF * RPM^2
     *
     * RPM = sqrt(F / kF)
     */
    gerekli_rpm =
        sqrtf(
            motor_basina_itki_n /
            MOTOR_ITKI_KATSAYISI_N_RPM2
        );

    /*
     * RPM = yuk_faktoru * KV * V * throttle
     */
    maksimum_rpm =
        MOTOR_YUK_FAKTORU *
        MOTOR_KV_RPM_V *
        batarya_gerilimi_v;

    if (maksimum_rpm <= 0.0f)
    {
        return Sinirla(
            TEMEL_THROTTLE,
            ORTAK_THROTTLE_MIN,
            ORTAK_THROTTLE_MAX
        );
    }

    temel_throttle =
        gerekli_rpm /
        maksimum_rpm;

    return Sinirla(
        temel_throttle,
        ORTAK_THROTTLE_MIN,
        ORTAK_THROTTLE_MAX
    );

#else

    (void)veri_merkezi;

    return Sinirla(
        TEMEL_THROTTLE,
        ORTAK_THROTTLE_MIN,
        ORTAK_THROTTLE_MAX
    );

#endif
}


/*
 * Mixer duzeltmeleri icin motorlarda yeterli
 * alt ve ust throttle payi birakir.
 */
static float OrtakThrottlePayiniKoru(
    float ortak_throttle,
    float roll_duzeltmesi,
    float pitch_duzeltmesi,
    float yaw_duzeltmesi)
{
    float gerekli_pay;
    float alt_sinir;
    float ust_sinir;

    gerekli_pay =
        fabsf(roll_duzeltmesi) +
        fabsf(pitch_duzeltmesi) +
        fabsf(yaw_duzeltmesi);

    alt_sinir =
        ORTAK_THROTTLE_MIN +
        gerekli_pay;

    ust_sinir =
        ORTAK_THROTTLE_MAX -
        gerekli_pay;

    /*
     * Duzeltme payi kullanilabilir throttle
     * araligindan buyukse normal sinirlar uygulanir.
     */
    if (alt_sinir > ust_sinir)
    {
        return Sinirla(
            ortak_throttle,
            ORTAK_THROTTLE_MIN,
            ORTAK_THROTTLE_MAX
        );
    }

    return Sinirla(
        ortak_throttle,
        alt_sinir,
        ust_sinir
    );
}


/*=========================================================================
 * PUBLIC FONKSIYONLAR
 *=========================================================================*/

void FlightControl_Init(void)
{
    /*
     * Yukseklik hatasi
     *      ->
     * Hedef dikey hiz
     */
    PidYapilandir(
        &yukseklik_pid,
        YUKSEKLIK_PID_KP,
        YUKSEKLIK_PID_KI,
        YUKSEKLIK_PID_KD,
        YUKSEKLIK_PID_CIKIS_MIN_MPS,
        YUKSEKLIK_PID_CIKIS_MAX_MPS
    );

    /*
     * Dikey hiz hatasi
     *      ->
     * Ortak throttle duzeltmesi
     */
    PidYapilandir(
        &dikey_hiz_pid,
        DIKEY_HIZ_PID_KP,
        DIKEY_HIZ_PID_KI,
        DIKEY_HIZ_PID_KD,
        DIKEY_HIZ_PID_CIKIS_MIN,
        DIKEY_HIZ_PID_CIKIS_MAX
    );

    /*
     * Roll hedef hizi - Gyro Y
     *      ->
     * Mixer roll duzeltmesi
     */
    PidYapilandir(
        &roll_hiz_pid,
        ROLL_HIZ_PID_KP,
        ROLL_HIZ_PID_KI,
        ROLL_HIZ_PID_KD,
        ROLL_HIZ_PID_CIKIS_MIN,
        ROLL_HIZ_PID_CIKIS_MAX
    );

    /*
     * Pitch hedef hizi - Gyro X
     *      ->
     * Mixer pitch duzeltmesi
     */
    PidYapilandir(
        &pitch_hiz_pid,
        PITCH_HIZ_PID_KP,
        PITCH_HIZ_PID_KI,
        PITCH_HIZ_PID_KD,
        PITCH_HIZ_PID_CIKIS_MIN,
        PITCH_HIZ_PID_CIKIS_MAX
    );

    /*
     * Yaw hedef hizi - Gyro Z
     *      ->
     * Mixer yaw duzeltmesi
     */
    PidYapilandir(
        &yaw_hiz_pid,
        YAW_HIZ_PID_KP,
        YAW_HIZ_PID_KI,
        YAW_HIZ_PID_KD,
        YAW_HIZ_PID_CIKIS_MIN,
        YAW_HIZ_PID_CIKIS_MAX
    );

    hedef_yukseklik_m =
        HOVER_YUKSEKLIGI_m;

    hedef_roll_deg = 0.0f;
    hedef_pitch_deg = 0.0f;
    hedef_yaw_deg = 0.0f;

    onceki_guncelleme_us = 0U;

    zaman_baslatildi = 0U;
    kontrol_aktif = 0U;
    yaw_hedefi_belirlendi = 0U;

    Motors_Disarm();
}


void FlightControl_Enable(void)
{
    TumPidleriSifirla();

    onceki_guncelleme_us = 0U;
    zaman_baslatildi = 0U;

    /*
     * Disaridan bir yaw hedefi verilmediyse
     * ilk Update turunda mevcut yaw kaydedilir.
     */
    yaw_hedefi_belirlendi = 0U;

    Motors_Arm();

    if (Motors_IsArmed() == 0U)
    {
        kontrol_aktif = 0U;
        return;
    }

    kontrol_aktif = 1U;
}


void FlightControl_Disable(void)
{
    kontrol_aktif = 0U;

    onceki_guncelleme_us = 0U;
    zaman_baslatildi = 0U;
    yaw_hedefi_belirlendi = 0U;

    TumPidleriSifirla();

    Motors_Disarm();
}


uint8_t FlightControl_IsEnabled(void)
{
    return kontrol_aktif;
}


void FlightControl_SetTargetAltitude(
    float yeni_hedef_yukseklik_m)
{
    hedef_yukseklik_m =
        yeni_hedef_yukseklik_m;

    /*
     * Onceki hedefin yukseklik PID gecmisini
     * yeni hedefe tasimiyoruz.
     *
     * Dikey hiz PID sifirlanmaz. Onun integrali
     * hover throttle hatasini telafi ediyor olabilir.
     */
    PID_Reset(&yukseklik_pid);
}


void FlightControl_SetTargetAttitude(
    float yeni_hedef_roll_deg,
    float yeni_hedef_pitch_deg,
    float yeni_hedef_yaw_deg)
{
    hedef_roll_deg =
        Aciyi180AraliginaGetir(
            yeni_hedef_roll_deg
        );

    hedef_pitch_deg =
        Aciyi180AraliginaGetir(
            yeni_hedef_pitch_deg
        );

    hedef_yaw_deg =
        Aciyi180AraliginaGetir(
            yeni_hedef_yaw_deg
        );

    yaw_hedefi_belirlendi = 1U;

    /*
     * Dis aci kontrolculeri yalnizca P'dir.
     * Bu nedenle sifirlanacak integral veya
     * turev gecmisi bulunmaz.
     *
     * Ic hiz PID integralleri motor dengesini
     * telafi edebilecegi icin korunur.
     */
}


void FlightControl_Update(
    const DataCenter *veri_merkezi,
    uint32_t mevcut_zaman_us)
{
    uint32_t gecen_sure_us;

    float gecen_sure_s;

    float mevcut_yukseklik_m;
    float mevcut_dikey_hiz_mps;

    float mevcut_roll_deg;
    float mevcut_pitch_deg;
    float mevcut_yaw_deg;

    float mevcut_roll_hizi_dps;
    float mevcut_pitch_hizi_dps;
    float mevcut_yaw_hizi_dps;

    float hedef_dikey_hiz_mps;

    float roll_hatasi_deg;
    float pitch_hatasi_deg;
    float yaw_hatasi_deg;

    float hedef_roll_hizi_dps;
    float hedef_pitch_hizi_dps;
    float hedef_yaw_hizi_dps;

    float temel_throttle;
    float dikey_hiz_duzeltmesi;
    float ortak_throttle;

    float roll_duzeltmesi;
    float pitch_duzeltmesi;
    float yaw_duzeltmesi;

    MotorOutputs_t motor_cikislari;


    if ((veri_merkezi == NULL) ||
        (kontrol_aktif == 0U))
    {
        return;
    }


    if (Motors_IsArmed() == 0U)
    {
        kontrol_aktif = 0U;
        return;
    }


    /*
     * Ilk turda yalnizca zaman ve otomatik
     * yaw hedefi kaydedilir.
     */
    if (zaman_baslatildi == 0U)
    {
        onceki_guncelleme_us =
            mevcut_zaman_us;

        zaman_baslatildi = 1U;

        if (yaw_hedefi_belirlendi == 0U)
        {
            hedef_yaw_deg =
                veri_merkezi->
                estimated.yaw.value;

            yaw_hedefi_belirlendi = 1U;
        }

        return;
    }


    gecen_sure_us =
        mevcut_zaman_us -
        onceki_guncelleme_us;


    /*
     * 100 Hz kontrol icin yeterli zaman
     * gecmediyse bu tur hesap yapilmaz.
     */
    if (gecen_sure_us <
        FLIGHT_CONTROL_PERIYOT_US)
    {
        return;
    }


    onceki_guncelleme_us =
        mevcut_zaman_us;

    gecen_sure_s =
        (float)gecen_sure_us *
        SISTEM_TIMERI_TIMESTEP;


    /*
     * Uzun gecikme sonrasi PID integral ve
     * turev sicramalarini engeller.
     */
    if ((gecen_sure_s <= 0.0f) ||
        (gecen_sure_s >
         FLIGHT_CONTROL_MAX_DT_S))
    {
        TumPidleriSifirla();
        return;
    }


    /*---------------------------------------------------------------------
     * KESTIRIM VERILERI
     *---------------------------------------------------------------------*/

    mevcut_yukseklik_m =
        veri_merkezi->
        estimated.pos_z.value;

    mevcut_dikey_hiz_mps =
        veri_merkezi->
        estimated.vel_z.value;

    mevcut_roll_deg =
        veri_merkezi->
        estimated.roll.value;

    mevcut_pitch_deg =
        veri_merkezi->
        estimated.pitch.value;

    mevcut_yaw_deg =
        veri_merkezi->
        estimated.yaw.value;


    /*
     * Bizim eksen tanimimiz:
     *
     * Roll  -> Y ekseni -> Gyro Y
     * Pitch -> X ekseni -> Gyro X
     * Yaw   -> Z ekseni -> Gyro Z
     */
    mevcut_roll_hizi_dps =
        veri_merkezi->
        gyro.y.filteredValue;

    mevcut_pitch_hizi_dps =
        veri_merkezi->
        gyro.x.filteredValue;

    mevcut_yaw_hizi_dps =
        veri_merkezi->
        gyro.z.filteredValue;


    /*---------------------------------------------------------------------
     * DIKEY KONTROL
     *---------------------------------------------------------------------*/

    /*
     * Dis dongu:
     *
     * Yukseklik hatasi
     *      ->
     * Hedef dikey hiz
     */
    hedef_dikey_hiz_mps =
        compute_pid(
            &yukseklik_pid,
            hedef_yukseklik_m,
            mevcut_yukseklik_m,
            gecen_sure_s
        );


    /*
     * Ic dongu:
     *
     * Dikey hiz hatasi
     *      ->
     * Throttle duzeltmesi
     */
    dikey_hiz_duzeltmesi =
        compute_pid(
            &dikey_hiz_pid,
            hedef_dikey_hiz_mps,
            mevcut_dikey_hiz_mps,
            gecen_sure_s
        );


    temel_throttle =
        TemelThrottleHesapla(
            veri_merkezi
        );

    ortak_throttle =
        temel_throttle +
        dikey_hiz_duzeltmesi;


    /*---------------------------------------------------------------------
     * ROLL KONTROLU
     *---------------------------------------------------------------------*/

    /*
     * Dis P kontrolu:
     *
     * Roll aci hatasi
     *      ->
     * Hedef roll acisal hizi
     */
    roll_hatasi_deg =
        Aciyi180AraliginaGetir(
            hedef_roll_deg -
            mevcut_roll_deg
        );

    hedef_roll_hizi_dps =
        ROLL_ACI_KP *
        roll_hatasi_deg;

    hedef_roll_hizi_dps =
        Sinirla(
            hedef_roll_hizi_dps,
            -ROLL_HEDEF_HIZ_MAX_DPS,
             ROLL_HEDEF_HIZ_MAX_DPS
        );


    /*
     * Ic PID:
     *
     * Hedef roll hizi - Gyro Y
     *      ->
     * Mixer roll duzeltmesi
     */
    roll_duzeltmesi =
        compute_pid(
            &roll_hiz_pid,
            hedef_roll_hizi_dps,
            mevcut_roll_hizi_dps,
            gecen_sure_s
        );


    /*---------------------------------------------------------------------
     * PITCH KONTROLU
     *---------------------------------------------------------------------*/

    pitch_hatasi_deg =
        Aciyi180AraliginaGetir(
            hedef_pitch_deg -
            mevcut_pitch_deg
        );

    hedef_pitch_hizi_dps =
        PITCH_ACI_KP *
        pitch_hatasi_deg;

    hedef_pitch_hizi_dps =
        Sinirla(
            hedef_pitch_hizi_dps,
            -PITCH_HEDEF_HIZ_MAX_DPS,
             PITCH_HEDEF_HIZ_MAX_DPS
        );


    /*
     * Pitch X ekseni etrafindadir.
     * Bu nedenle Gyro X kullanilir.
     */
    pitch_duzeltmesi =
        compute_pid(
            &pitch_hiz_pid,
            hedef_pitch_hizi_dps,
            mevcut_pitch_hizi_dps,
            gecen_sure_s
        );


    /*---------------------------------------------------------------------
     * YAW KONTROLU
     *---------------------------------------------------------------------*/

    yaw_hatasi_deg =
        Aciyi180AraliginaGetir(
            hedef_yaw_deg -
            mevcut_yaw_deg
        );

    hedef_yaw_hizi_dps =
        YAW_ACI_KP *
        yaw_hatasi_deg;

    hedef_yaw_hizi_dps =
        Sinirla(
            hedef_yaw_hizi_dps,
            -YAW_HEDEF_HIZ_MAX_DPS,
             YAW_HEDEF_HIZ_MAX_DPS
        );


    yaw_duzeltmesi =
        compute_pid(
            &yaw_hiz_pid,
            hedef_yaw_hizi_dps,
            mevcut_yaw_hizi_dps,
            gecen_sure_s
        );


    /*---------------------------------------------------------------------
     * THROTTLE PAYI
     *---------------------------------------------------------------------*/

    ortak_throttle =
        OrtakThrottlePayiniKoru(
            ortak_throttle,
            roll_duzeltmesi,
            pitch_duzeltmesi,
            yaw_duzeltmesi
        );


    /*---------------------------------------------------------------------
     * MOTOR MIXER
     *---------------------------------------------------------------------*/

    MotorMixer_Calculate(
        ortak_throttle,
        roll_duzeltmesi,
        pitch_duzeltmesi,
        yaw_duzeltmesi,
        &motor_cikislari
    );


    /*---------------------------------------------------------------------
     * MOTOR CIKISLARI
     *---------------------------------------------------------------------*/

    Motors_SetOutputs(
        &motor_cikislari
    );
}
