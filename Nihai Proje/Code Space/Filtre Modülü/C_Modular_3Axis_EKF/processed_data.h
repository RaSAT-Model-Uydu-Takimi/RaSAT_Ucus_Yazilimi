/*
 * processed_data.h
 *
 * Tanım:
 * Eski veya test klasörlerinden yapılan çağrıları tek bir doğruluk kaynağı (Single Source of Truth)
 * olan ana dizindeki Processed_Data.h dosyasına yönlendiren sarmalayıcı (wrapper) başlık dosyasıdır.
 *
 * İşlev:
 * Ana dizin altındaki Inc/Processed_Data.h dosyasını doğrudan dahil eder (include). Böylece tüm modüller
 * aynı yapı (ProcessedData_t, SensorChannels_t, EKF_State_t vb.) üzerinden çalışır.
 */

#ifndef PROCESSED_DATA_WRAPPER_H_
#define PROCESSED_DATA_WRAPPER_H_

#include "../Inc/Processed_Data.h"

#endif /* PROCESSED_DATA_WRAPPER_H_ */
