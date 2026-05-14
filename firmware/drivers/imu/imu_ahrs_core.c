/**
 * @file    imu_ahrs_core.c
 * @brief   Madgwick AHRS filter implementation for IMU attitude estimation
 * @author  Dinglei Xu
 * @date    2025
 */

#include "imu_ahrs_core.h"
#include <string.h>
#include <math.h>
#include <stdlib.h> // for malloc, free
#include <stdio.h>
/* ------------ �ڲ����� ------------ */
#define GYRO_FSR_DPS 2000
#define ACCEL_FSR_G 16
#define Kp 0.5f
#define Ki 0.001f
#define M_PI 3.1415926535f

// ������������
const float GYRO_SENSITIVITY = 32768.0f / GYRO_FSR_DPS;
const float ACCEL_SENSITIVITY = 32768.0f / ACCEL_FSR_G;

// �ڲ�ʹ�õ�У׼״̬�ṹ��
typedef struct
{
    imu_cal_state_t state;
    float variance_threshold;
    uint16_t buffer_size;
    uint16_t index;
    uint8_t buffer_full;
    float gyro_offset_dps[3];
    double sum[3];
    double sum_sq[3];
    float (*buffer)[3]; // ָ��������������ָ��
} cal_state_t;

// �ڲ����������ľ�̬����
static void madgwick_update(imu_ahrs_handle_t *handle, float gx, float gy, float gz, float ax, float ay, float az);
static void _update_calibration(imu_ahrs_handle_t *handle, const float gyro_dps[3]);
static float inv_sqrt(float x);

/*
 * =================================================================================
 * �� �� �� �� API ʵ ��
 * =================================================================================
 */

int imu_ahrs_init(imu_ahrs_handle_t *handle, const imu_ahrs_config_t *config, const imu_hal_functions_t *funcs)
{
    // 1. �������
    if (!handle || !config || !funcs || !funcs->read_reg || !funcs->write_reg || !funcs->sleep_us || !funcs->get_time_us)
    {
        return -1; // ��Ч����
    }

    memset(handle, 0, sizeof(imu_ahrs_handle_t));
    handle->hal = *funcs;

    // 2. ��ʼ��������У׼״̬�ṹ���ڴ�
    cal_state_t *cal = (cal_state_t *)malloc(sizeof(cal_state_t));
    if (!cal)
        return -10; // �ڴ����ʧ��
    handle->_cal_state = cal;

    cal->buffer = (float (*)[3])malloc(config->cal_buffer_size * sizeof(float[3]));
    if (!cal->buffer)
    {
        free(cal);
        handle->_cal_state = NULL;
        return -10;
    }

    cal->buffer_size = config->cal_buffer_size;
    cal->variance_threshold = config->cal_variance_threshold;
    imu_ahrs_reset_calibration(handle); // ��ʼ��У׼״̬

    // 3. ��� InvenSense ���� transport �ṹ��
    handle->device.transport.read_reg = handle->hal.read_reg;
    handle->device.transport.write_reg = handle->hal.write_reg;
    handle->device.transport.sleep_us = handle->hal.sleep_us;
    handle->device.transport.serif_type = UI_SPI4;

    // 4. ��ʼ��������Ӳ��
    uint8_t who_am_i;
    int status = 0;
    status = inv_imu_soft_reset(&handle->device);
    if (status)
    {
        imu_ahrs_deinit(handle);
        return -2;
    }
    handle->hal.sleep_us(100000);

    status = inv_imu_get_who_am_i(&handle->device, &who_am_i);
    if (status)
    {
        imu_ahrs_deinit(handle);
        return -3;
    }
    if (who_am_i != INV_IMU_WHOAMI)
    {
        imu_ahrs_deinit(handle);
        return -4;
    }

    status |= inv_imu_set_accel_mode(&handle->device, PWR_MGMT0_ACCEL_MODE_LN);
    status |= inv_imu_set_gyro_mode(&handle->device, PWR_MGMT0_GYRO_MODE_LN);
    handle->hal.sleep_us(1000);
    status |= inv_imu_set_accel_frequency(&handle->device, ACCEL_CONFIG0_ACCEL_ODR_1600_HZ);
    status |= inv_imu_set_gyro_frequency(&handle->device, GYRO_CONFIG0_GYRO_ODR_1600_HZ);
    status |= inv_imu_set_accel_fsr(&handle->device, ACCEL_CONFIG0_ACCEL_UI_FS_SEL_16_G);
    status |= inv_imu_set_gyro_fsr(&handle->device, GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS);
    if (status)
    {
        imu_ahrs_deinit(handle);
        return -5;
    }

    // 5. ��ʼ�� AHRS �㷨״̬
    handle->q0 = 1.0f;
    handle->q1 = 0.0f;
    handle->q2 = 0.0f;
    handle->q3 = 0.0f;
    handle->exInt = 0.0f;
    handle->eyInt = 0.0f;
    handle->ezInt = 0.0f;
    handle->last_update_time_us = handle->hal.get_time_us();

    return 0; // ��ʼ���ɹ�
}

void imu_ahrs_deinit(imu_ahrs_handle_t *handle)
{
    if (handle && handle->_cal_state)
    {
        cal_state_t *cal = (cal_state_t *)handle->_cal_state;
        if (cal->buffer)
        {
            free(cal->buffer);
        }
        free(cal);
        handle->_cal_state = NULL;
    }
}

int imu_ahrs_update(imu_ahrs_handle_t *handle)
{
    inv_imu_sensor_data_t sensor_data;
    int status = inv_imu_get_register_data(&handle->device, &sensor_data);
    if (status)
        return status;

    // ת��ԭʼ����Ϊ������λ
    float accel_g[3] = {
        (float)sensor_data.accel_data[0] / ACCEL_SENSITIVITY,
        (float)sensor_data.accel_data[1] / ACCEL_SENSITIVITY,
        (float)sensor_data.accel_data[2] / ACCEL_SENSITIVITY};

    float gyro_dps[3] = {
        (float)sensor_data.gyro_data[0] / GYRO_SENSITIVITY,
        (float)sensor_data.gyro_data[1] / GYRO_SENSITIVITY,
        (float)sensor_data.gyro_data[2] / GYRO_SENSITIVITY};

    // 1. ���в����¶�̬У׼�߼�
    _update_calibration(handle, gyro_dps);

    // 2. Ӧ�����ƫ��
    cal_state_t *cal = (cal_state_t *)handle->_cal_state;
    float gyro_dps_calibrated[3] = {
        gyro_dps[0] - cal->gyro_offset_dps[0],
        gyro_dps[1] - cal->gyro_offset_dps[1],
        gyro_dps[2] - cal->gyro_offset_dps[2]};

    // 3. ��У׼��Ľ��ٶ�ת��Ϊ����/��
    float gyro_rads[3] = {
        gyro_dps_calibrated[0] * (M_PI / 180.0f),
        gyro_dps_calibrated[1] * (M_PI / 180.0f),
        gyro_dps_calibrated[2] * (M_PI / 180.0f)};

    // 4. ʹ�ô����õ����ݸ�����̬
    madgwick_update(handle, gyro_rads[0], gyro_rads[1], gyro_rads[2], accel_g[0], accel_g[1], accel_g[2]);

    return 0;
}

void imu_ahrs_get_gyro_offset(const imu_ahrs_handle_t *handle, float offset_dps[3])
{
    cal_state_t *cal = (cal_state_t *)handle->_cal_state;
    memcpy(offset_dps, cal->gyro_offset_dps, sizeof(float[3]));
}

imu_cal_state_t imu_ahrs_get_cal_state(const imu_ahrs_handle_t *handle)
{
    cal_state_t *cal = (cal_state_t *)handle->_cal_state;
    return cal->state;
}

void imu_ahrs_reset_calibration(imu_ahrs_handle_t *handle)
{
    cal_state_t *cal = (cal_state_t *)handle->_cal_state;
    if (!cal)
        return;

    cal->state = CAL_STATE_NEVER_RUN;
    cal->index = 0;
    cal->buffer_full = 0;
    for (int i = 0; i < 3; ++i)
    {
        cal->sum[i] = 0.0;
        cal->sum_sq[i] = 0.0;
        cal->gyro_offset_dps[i] = 0.0f;
    }
}

void imu_ahrs_get_euler_angles(const imu_ahrs_handle_t *handle, float *roll, float *pitch, float *yaw)
{
    float q0 = handle->q0;
    float q1 = handle->q1;
    float q2 = handle->q2;
    float q3 = handle->q3;

    *yaw = -atan2f(2.0f * q1 * q2 + 2.0f * q0 * q3, -2.0f * q2 * q2 - 2.0f * q3 * q3 + 1.0f) * (180.0f / M_PI);
    *pitch = -asinf(-2.0f * q1 * q3 + 2.0f * q0 * q2) * (180.0f / M_PI);
    *roll = atan2f(2.0f * q2 * q3 + 2.0f * q0 * q1, -2.0f * q1 * q1 - 2.0f * q2 * q2 + 1.0f) * (180.0f / M_PI);
}

void imu_ahrs_get_quaternion(const imu_ahrs_handle_t *handle, float *q0, float *q1, float *q2, float *q3)
{
    *q0 = handle->q0;
    *q1 = handle->q1;
    *q2 = handle->q2;
    *q3 = handle->q3;
}

/*
 * =================================================================================
 * �� �� �� �� �� ��
 * =================================================================================
 */

/**
 * @brief ��̬���У׼�ĺ����߼�
 */
static void _update_calibration(imu_ahrs_handle_t *handle, const float gyro_dps[3])
{
    cal_state_t *cal = (cal_state_t *)handle->_cal_state;

    // ����������������ȼ�ȥ��ɵ�����
    if (cal->buffer_full)
    {
        cal->sum[0] -= cal->buffer[cal->index][0];
        cal->sum[1] -= cal->buffer[cal->index][1];
        cal->sum[2] -= cal->buffer[cal->index][2];
        cal->sum_sq[0] -= cal->buffer[cal->index][0] * cal->buffer[cal->index][0];
        cal->sum_sq[1] -= cal->buffer[cal->index][1] * cal->buffer[cal->index][1];
        cal->sum_sq[2] -= cal->buffer[cal->index][2] * cal->buffer[cal->index][2];
    }

    // ����������
    cal->buffer[cal->index][0] = gyro_dps[0];
    cal->buffer[cal->index][1] = gyro_dps[1];
    cal->buffer[cal->index][2] = gyro_dps[2];

    // �ۼ�������
    cal->sum[0] += gyro_dps[0];
    cal->sum[1] += gyro_dps[1];
    cal->sum[2] += gyro_dps[2];
    cal->sum_sq[0] += gyro_dps[0] * gyro_dps[0];
    cal->sum_sq[1] += gyro_dps[1] * gyro_dps[1];
    cal->sum_sq[2] += gyro_dps[2] * gyro_dps[2];

    cal->index++;
    if (cal->index >= cal->buffer_size)
    {
        cal->index = 0;
        cal->buffer_full = 1;
    }

    // ���������û����������ռ�����
    if (!cal->buffer_full)
    {
        cal->state = CAL_STATE_COLLECTING;
        return;
    }

    // ���������������㷽��
    float variance[3];
    double N = cal->buffer_size;
    for (int i = 0; i < 3; i++)
    {
        // ���ʽ: Var(X) = E(X^2) - [E(X)]^2
        variance[i] = (float)((cal->sum_sq[i] / N) - ((cal->sum[i] / N) * (cal->sum[i] / N)));
    }

    // �ж��Ƿ�ֹ
    if (variance[0] < cal->variance_threshold &&
        variance[1] < cal->variance_threshold &&
        variance[2] < cal->variance_threshold)
    {
        // �����ֹ���������ƫ�ƣ��������㷨��������
        cal->state = CAL_STATE_STATIONARY;
        cal->gyro_offset_dps[0] = (float)(cal->sum[0] / N);
        cal->gyro_offset_dps[1] = (float)(cal->sum[1] / N);
        cal->gyro_offset_dps[2] = (float)(cal->sum[2] / N);

        handle->exInt = 0.0f;
        handle->eyInt = 0.0f;
        handle->ezInt = 0.0f;
    }
    else
    {
        // ������˶�������Ϊ�ɼ���
        cal->state = CAL_STATE_COLLECTING;
    }
}

/**
 * @brief ���ٿ�������
 */
static float inv_sqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}
/**
 * @brief Madgwick �㷨����ʵ��
 */
static void madgwick_update(imu_ahrs_handle_t *handle, float gx, float gy, float gz, float ax, float ay, float az)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float halfT;

    // ������ٶ�������ֵΪ0���������˲����£���ֹ�����������
    if ((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))
    {
        return;
    }

    // �������θ���֮���ʱ���
    uint32_t now_us = handle->hal.get_time_us();

    /* =========================================================================
     * ?? ���ʵ�� ??
     * ������һ�д��룬����C�����޷����������������ԣ�
     * ����ͬʱ��ȷ����������������͡���ʱ����������������
     * ����ȫ��Ч����д�� if/else �ṹ��������ࡢ����Ч�������׳�����
     * =========================================================================
     */
    uint32_t delta_us = now_us - handle->last_update_time_us;

    // (printf �����ڵ��ԣ����Ա�����ɾ��)
    //printf("now_us:%u, last_us:%u, delta_us:%u\r\n", now_us, handle->last_update_time_us, delta_us);

    // ����ʱ����һ�� (��λ����)
    halfT = (float)delta_us / 2000000.0f;

    handle->last_update_time_us = now_us;

    float q0 = handle->q0;
    float q1 = handle->q1;
    float q2 = handle->q2;
    float q3 = handle->q3;

    // ���ٶȼ����ݹ�һ��
    norm = inv_sqrt(ax * ax + ay * ay + az * az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    // ����Ԫ��������������
    vx = 2.0f * (q1 * q3 - q0 * q2);
    vy = 2.0f * (q0 * q1 + q2 * q3);
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    // �������������ʵ�����������ٶȼƲ���ֵ�������
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    // ������
    handle->exInt += ex * Ki * halfT * 2.0f;
    handle->eyInt += ey * Ki * halfT * 2.0f;
    handle->ezInt += ez * Ki * halfT * 2.0f;

    // PI���ڣ���������������
    gx += Kp * ex + handle->exInt;
    gy += Kp * ey + handle->eyInt;
    gz += Kp * ez + handle->ezInt;

    // һ�����������������Ԫ��
    q0 += (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 += (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 += (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 += (q0 * gz + q1 * gy - q2 * gx) * halfT;

    // ��Ԫ����һ��
    norm = inv_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    handle->q0 = q0 * norm;
    handle->q1 = q1 * norm;
    handle->q2 = q2 * norm;
    handle->q3 = q3 * norm;
}
