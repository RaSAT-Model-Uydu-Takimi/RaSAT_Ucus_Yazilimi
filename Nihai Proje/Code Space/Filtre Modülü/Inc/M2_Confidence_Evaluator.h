# ifndef INC_CONFIDENCE_EVALUATOR_H_
# define INC_CONFIDENCE_EVALUATOR_H_

#include "Data.h"
#include "Filter_Config.h"

#ifdef __cplusplus
extern "C" {

/*
* TANIM : Bir önceki sağlıklı ölçümü belleğe alır
* İŞLEV : Spike tespiti için belleğe alıyoruz daha sonra [?] 
* fonksiyonunda kullanacağız
* NOT : GPS ve Manyotometre sıçramaları önemsizdir. 
* İşlemciye türevsel sıçrama kontrolüne gerek görülmedi
*/
typedef struct {
    float last_acc_x, last_acc_y, last_acc_z;
    float last_gyro_x, last_gyro_y, last_gyro_z;
    float last_baro_press;
} Confidence_Evaluator_t;


/*
* TANIM : Kurulum
* İŞLEV : Güven Katasyısını Kurar
* 
*/
void CE_Init(Confidence_Evaluator_t *eval);


/*
* TANIM : Güncelleme
* İŞLEV : 
* 1.    Güven Değerlendirme aracını DataCenterdan çektiği verilerle doldurur
* 2.    DataCenterdan güven değeri 
*/
void CE_Update
(
    Confidence_Evaluator_t *eval , 
    DataCenter *dataC , 
    uint32_t curr_time
);


#ifdef __cplusplus
}
#endif

#endif /* INC_CONFIDENCE_EVALUATOR_H_ */