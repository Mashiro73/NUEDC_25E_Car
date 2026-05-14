/**
 * @file    imu_ahrs_core.h
 * @brief   Madgwick AHRS filter type and function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __IMU_AHRS_CORE_H
#define __IMU_AHRS_CORE_H

#include <stdint.h>
#include "inv_imu_driver.h"

// ǰ���������ľ���ṹ��
struct imu_ahrs_handle;

/**
 * @brief Ӳ������㺯��ָ��ṹ�� (��֮ǰ�汾��ͬ)
 */
typedef struct {
    int (*write_reg)(uint8_t reg, const uint8_t *buf, uint32_t len);
    int (*read_reg)(uint8_t reg, uint8_t *buf, uint32_t len);
    void (*sleep_us)(uint32_t us);
    uint32_t (*get_time_us)(void);
} imu_hal_functions_t;

/**
 * @brief ��̬У׼��״̬
 */
typedef enum {
    CAL_STATE_COLLECTING, // ���ڲɼ���
    CAL_STATE_STATIONARY, // �Ѽ�⵽��ֹ�����һ��У׼
    CAL_STATE_NEVER_RUN   // ��δ���л�������
} imu_cal_state_t;


/**
 * @brief ���Ŀ�ĳ�ʼ������
 */
typedef struct {
    uint16_t cal_buffer_size;       // ���ڼ��㷽����������� (���� 100)
    float    cal_variance_threshold;  // �жϾ�ֹ�ķ�����ֵ (���� 0.02f)
} imu_ahrs_config_t;


/**
 * @brief ���Ŀ�ľ����Handle���������������ڲ�״̬
 */
typedef struct imu_ahrs_handle {
    // �ײ�����ʵ��
    inv_imu_device_t device; 
    
    // AHRS �㷨״̬
    float q0, q1, q2, q3;
    float exInt, eyInt, ezInt;
    uint32_t last_update_time_us;

    // Ӳ���ӿں���
    imu_hal_functions_t hal;

    // ��̬У׼�����״̬ (���û�͸��)
    void* _cal_state; // ָ���ڲ�У׼״̬�ṹ��

} imu_ahrs_handle_t;


/**
 * @brief ��ʼ�� IMU ���Ŀ�
 * @param handle    ָ���û������ imu_ahrs_handle_t �ṹ��ָ��
 * @param config    ָ���û���������ýṹ��ָ��
 * @param funcs     ָ���û�ʵ�ֵ�Ӳ������㺯���ṹ��ָ��
 * @return          0 ��ʾ�ɹ���������ʾʧ��
 */
int imu_ahrs_init(imu_ahrs_handle_t *handle, const imu_ahrs_config_t *config, const imu_hal_functions_t *funcs);

/**
 * @brief �ͷ��� imu_ahrs_init �������Դ
 * @param handle    ���Ŀ���
 */
void imu_ahrs_deinit(imu_ahrs_handle_t *handle);


/**
 * @brief ������̬���㣨������̬У׼�߼���
 * @param handle    ���Ŀ���
 * @return          0 ��ʾ�ɹ���������ʾʧ��
 */
int imu_ahrs_update(imu_ahrs_handle_t *handle);


/**
 * @brief ��ȡ��ǰ���������ƫ����
 * @param handle    ���Ŀ���
 * @param offset_dps ָ��һ��float[3]���飬���ڴ��X,Y,Z���ƫ����(dps)
 */
void imu_ahrs_get_gyro_offset(const imu_ahrs_handle_t *handle, float offset_dps[3]);

/**
 * @brief ��ȡ��ǰ��̬У׼��״̬
 * @param handle    ���Ŀ���
 * @return          imu_cal_state_t У׼״̬
 */
imu_cal_state_t imu_ahrs_get_cal_state(const imu_ahrs_handle_t *handle);


/**
 * @brief ���ö�̬У׼����
 * @note  ���ô˺����󣬿�����¿�ʼ�ռ������Լ����µ����ƫ��
 * @param handle    ���Ŀ���
 */
void imu_ahrs_reset_calibration(imu_ahrs_handle_t *handle);


/* ------------ ���º�����֮ǰ�汾��ͬ ------------ */
void imu_ahrs_get_euler_angles(const imu_ahrs_handle_t *handle, float *roll, float *pitch, float *yaw);
void imu_ahrs_get_quaternion(const imu_ahrs_handle_t *handle, float *q0, float *q1, float *q2, float *q3);

#endif // __IMU_AHRS_CORE_H