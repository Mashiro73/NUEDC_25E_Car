/**
 * @file    main.c
 * @brief   NUEDC 2025 Problem E - Line-following smart car firmware
 * @author  Dinglei Xu
 * @date    2025
 */

#include "main.h"
#include <stdarg.h>
// �������� UART ��ӡ�Ļ�������С
#define UART_PRINT_BUFFER_SIZE 128
#define STM32_UART UART_1_INST
// ================== ?????? ==================//
// volatile PathState_e g_path_state = S_CALIBRATE; // ���Ҫ����ǰ����������
volatile PathState_e g_path_state = S0_SELECT_LAPS; // ???????
volatile int g_laps_to_run = 1;                     // ???? (???,???1?)
volatile int g_laps_completed = 0;                  // ?????
volatile int g_corner_count = 1;                    //

// ???????????????
volatile int64_t g_encoder_l_total = 0;
volatile int64_t g_encoder_r_total = 0;

volatile uint32_t g_systick_count = 0;
volatile unsigned int g_delay_times = 0;
uint32_t line_speed = BASE_SPEED;

LineSensor_t myTracker;
Key_t my_keys[NUM_KEYS]; // ????????
float Kspeed_Dowwn = 1;

// #define KEY_START my_keys[0]

// ================== �������ظ������� ==================
void update_lcd_display(void);
void square_path_fsm(void);
static void action_stop(void);
static void action_line_following(int base_speed, float kp);
static bool action_go_straight(int speed, int32_t target_count);
static bool action_turn_left(int speed, int32_t target_count, float kp);
static void reset_encoders(void);
static float calculate_line_error(uint8_t sensor_data);
void uart_printf(UART_Regs *uart, const char *format, ...);
static void action_turn(float kp);
static bool action_start(float kp);

// ===============================================
//                ??? (main)
// ===============================================
int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
    NVIC_EnableIRQ(E1_INT_IRQN);
    NVIC_EnableIRQ(E2_INT_IRQN);

    //__enable_irq();

    LCD_Init();
    motor_init();
    key_init(&KEY_RESET, key_s1_PORT, key_s1_PIN, 0);   // S1
    key_init(&KEY_CONFIRM, key_s2_PORT, key_s2_PIN, 0); // S2

    GPIO_Regs *trackerPorts[LINE_SENSOR_COUNT] = {
        GPIO_SENSOR_0_PORT, GPIO_SENSOR_1_PORT, GPIO_SENSOR_2_PORT, GPIO_SENSOR_3_PORT,
        GPIO_SENSOR_4_PORT, GPIO_SENSOR_5_PORT, GPIO_SENSOR_6_PORT, GPIO_SENSOR_7_PORT};
    uint32_t trackerPins[LINE_SENSOR_COUNT] = {
        GPIO_SENSOR_0_PIN, GPIO_SENSOR_1_PIN, GPIO_SENSOR_2_PIN, GPIO_SENSOR_3_PIN,
        GPIO_SENSOR_4_PIN, GPIO_SENSOR_5_PIN, GPIO_SENSOR_6_PIN, GPIO_SENSOR_7_PIN};
    line_sensor_init(&myTracker, trackerPorts, trackerPins);
    LCD_Clear(BLACK);
    PID_Init();
    g_path_state = S0_SELECT_LAPS;
    while (1)
    {
        static uint16_t cnt = 0;
        update_lcd_display();
        // uart_printf(UART_0_INST, "Forward\0");
        static int print_counter = 0;
        if (++print_counter >= 5)
        {
            print_counter = 0;

            // // ʹ������ E00_TEST_demo.c �ж���� uart_printf ����
            // // L_S: ����Ŀ���ٶ�, L_A: ����ʵ���ٶ�
            // // R_S: ����Ŀ���ٶ�, R_A: ����ʵ���ٶ�
            // // �����Ը�����Ҫѡ���ӡ�� UART_0_INST �� STM32_UART
            // uart_printf(UART_0_INST, "%.1f,%.1f,%.1f,%.1f\n",
            //             PID_Motor_L.SetValue,
            //             PID_Motor_L.ActualValue,
            //             PID_Motor_R.SetValue,
            //             PID_Motor_R.ActualValue);
        }
        //			cnt = 0;
        //        }
        //        if (cnt++ % 100 != 0) { // �ӿ�ˢ�����Ա��"��
        // ���޸ġ���ѭ���еİ��������߼�
        switch (g_path_state)
        {
        case S_CALIBRATE:
            if (key_get_state(&KEY_RESET) == KEY_STATE_PRESSED)
            {
                //reset_encoders(); // S1���ñ�����
            }
            if (key_get_state(&KEY_CONFIRM) == KEY_STATE_PRESSED)
            {
                g_path_state = S0_SELECT_LAPS; // S2ȷ�ϣ�������һ״̬
            }
            break;

        case S0_SELECT_LAPS:
            if (key_get_state(&KEY_SELECT) == KEY_STATE_PRESSED)
            {
                g_laps_to_run++;
                if (g_laps_to_run > 5)
                    g_laps_to_run = 1;
                // ѡ��Ȧ��
            }
            if (key_get_state(&KEY_CONFIRM) == KEY_STATE_PRESSED)
            {
                g_path_state = S0_WAIT_FOR_START;
                // ����
            }
            break;

        case S0_WAIT_FOR_START:
            if (key_get_state(&KEY_CONFIRM) == KEY_STATE_PRESSED)
            {
                g_corner_count = 0;
                //reset_encoders();
                line_speed = BASE_SPEED;
                // g_path_state = S1_START_TO_GO;
				uart_printf(STM32_UART, "CarGo!");
                g_path_state = S1_LINE_FOLLOWING;
            }
            break;
        default:
            break;
        }
        // delay_ms(10);
    }
}

// ===============================================
//           ?????????
// ===============================================
void square_path_fsm(void)
{
	static uint8_t delay_cnt = 0;
	if(delay_cnt >0)
	{
		delay_cnt--;
	}
    uint8_t sensor_data;
    switch (g_path_state)
    {
    case S_CALIBRATE:
    case S0_SELECT_LAPS:
    case S0_WAIT_FOR_START:
        break;

    case S1_LINE_FOLLOWING:
		if(g_corner_count !=0)
		{
		    uart_printf(UART_0_INST,"@D:%.2f#\r\n",((float)(g_encoder_l_total+g_encoder_r_total))/2*100/2970+10);
			uart_printf(STM32_UART, "@D:%.2f#\0", ((float)(g_encoder_l_total + g_encoder_r_total)) / 2 * 100 / 2970 + 10);
		}
        sensor_data = line_sensor_read(&myTracker);
		if(delay_cnt == 0)
		{
			line_speed = BASE_SPEED;
		}
        if ((sensor_data & 0b11000000) == 0b11000000 && (sensor_data & 0b00000011) == 0)
        {
            g_corner_count++;

            if (g_corner_count >= ((g_laps_to_run * 4) + 1))
            {
                g_path_state = S3_STOPPED;
            }
            else
            {
                //reset_encoders();
                g_path_state = S2_PREPARE_TURN_FORWARD;
            }
        }
        break;
    case S2_PREPARE_TURN_FORWARD:
        static uint8_t trans = 1;
        if (trans)
        {
            uart_printf(STM32_UART, "Turnin\0");
            uart_printf(UART_0_INST,"Turning\0");
            trans = 0;
        }
        // ??????,????true,?????????
        //reset_encoders(); // ???????,??????
        trans = 1;
        g_path_state = S2_TURNING_LEFT;
        break;

    case S2_TURNING_LEFT:
        // ??????,????true,?????????
        // line_speed = TURN_SPEED; // ����ת���ٶ�
		if(g_corner_count %4==1)
		{
			uart_printf(STM32_UART, "@D:%.2lf#\0", ((float)(g_encoder_l_total + g_encoder_r_total)) / 2 * 100 / 2970 + 10);
			uart_printf(UART_0_INST, "@D:%.2lf#\0", ((float)(g_encoder_l_total + g_encoder_r_total)) / 2 * 100 / 2970 + 10);

		}
		else if(g_corner_count %4==0)
		{
			reset_encoders();
		}
        static uint8_t flagfirst = 1;
        if (flagfirst)
        {
            Kspeed_Dowwn = 0.68;
			reset_encoders();

            flagfirst = 0;
        }

        action_turn(LINE_KP);
        if (action_turn_left(TURN_SPEED, ENCODER_COUNT_TURN, LINE_KP))
        {
            uart_printf(STM32_UART, "Forward\0");
            uart_printf(UART_0_INST,"Forward\0");
            //reset_encoders();
            line_speed = TURN_SPEED;
			delay_cnt = 12;
            Kspeed_Dowwn = 1;                 // �ָ��ٶ�
            g_path_state = S1_LINE_FOLLOWING; // ????,????
        }
        break;

    case S3_STOPPED:
        // uart_printf(STM32_UART, "Turnin\0");
        g_path_state = S0_SELECT_LAPS;
        break;
    }
    if (g_path_state != S0_WAIT_FOR_START && g_path_state != S0_SELECT_LAPS && g_path_state != S2_TURNING_LEFT && g_path_state != S1_START_TO_GO)
    {
        action_line_following(line_speed, LINE_KP);
    }
    else if (g_path_state == S0_WAIT_FOR_START || g_path_state == S0_SELECT_LAPS)
    {
        action_stop(); // ��ѡ��ģʽ��ֹͣ���
    }
}

// ===============================================
//           ?????????
// ===============================================

/** @brief ??:?? */
static void action_stop(void)
{
    PID_Motor_L.SetValue = 0;
    PID_Motor_R.SetValue = 0;
}

/** @brief ??:????? */
static void action_line_following(int base_speed, float kp)
{
    uint8_t sensor_data = line_sensor_read(&myTracker);
    float error = calculate_line_error(sensor_data);
    int turn_adjustment = (int)(error * kp);

    PID_Motor_L.SetValue = base_speed - turn_adjustment;
    PID_Motor_R.SetValue = base_speed + turn_adjustment;
}
static void action_turn(float kp)
{
    float turnspeed = TURN_SPEED;
    static uint8_t flagfirst = 1;
    PID_Motor_L.SetValue = (turnspeed - 4.5) * Kspeed_Dowwn;
    PID_Motor_R.SetValue = (turnspeed + 4.5) * Kspeed_Dowwn;
}

/** * @brief ??:??????????
 * @return bool - true:????, false:???
 */
static bool action_go_straight(int speed, int32_t target_count)
{
    // motor_set_pulse(MOTOR_A, speed);
    // motor_set_pulse(MOTOR_B, speed);

    // // ???????????????,?????
    // if ((g_encoder_l_total + g_encoder_r_total) / 2 >= target_count) {
    //     return true;
    // }
    return true;
}

/** * @brief ??:?????????????
 * @return bool - true:????, false:???
 */
static bool action_turn_left(int speed, int32_t target_count, float kp)
{
    // motor_set_pulse(MOTOR_A, -speed); // ????
    // motor_set_pulse(MOTOR_B, speed);  // ????

    uint8_t sensor_data = line_sensor_read(&myTracker);
    float error = calculate_line_error(sensor_data);
    // int turn_adjustment = (int)(error * kp);

    // motor_set_pulse(MOTOR_A, speed - turn_adjustment);
    // motor_set_pulse(MOTOR_B, speed + turn_adjustment);

    // ??????????????????,?????
    static uint8_t cnt = 0;
    // if (sensor_data == CORNER_DETECT_MASK4 || sensor_data == CORNER_DETECT_MASK5 || sensor_data == 0b00001000 || sensor_data == 0b00011000|| sensor_data == 0b00110000)
    if ((sensor_data & 0b11000000) == 0 && (sensor_data & 0b00111100) != 0 && (sensor_data & 0b00000011) == 0)
    {

        if (cnt++ >= 1)
        {
            cnt = 0;
            return true;
        }
    }
    return false;
}
static bool action_start(float kp)
{
    uint8_t sensor_data = line_sensor_read(&myTracker);
    float error = calculate_line_error(sensor_data);
    int turn_adjustment = (int)(error * kp);
    static float current_speed_l = 0;
    static float current_speed_r = 0;
    const float acc = 1.6;
    if (current_speed_l < TURN_SPEED - turn_adjustment)
    {
        current_speed_l += acc;
    }
    else
    {
        current_speed_l = TURN_SPEED - turn_adjustment;
    }
    if (current_speed_r < TURN_SPEED + turn_adjustment)
    {
        current_speed_r += acc;
    }
    else
    {
        current_speed_r = TURN_SPEED + turn_adjustment;
    }
    PID_Motor_L.SetValue = current_speed_l;
    PID_Motor_R.SetValue = current_speed_r;
}

// ================== ?????? ==================

/** @brief ????????????? */
static void reset_encoders(void)
{
    g_encoder_l_total = 0;
    g_encoder_r_total = 0;
}

/** @brief ??????? */
static float calculate_line_error(uint8_t sensor_data)
{
    const static int weights[] = {-24, -12, -6, -4, 4, 6, 12, 24};
    float error_sum = 0;
    uint8_t active_sensors_count = 0;
    static float last_error = 0;

    for (int i = 0; i < 8; i++)
    {
        if ((sensor_data >> i) & 0x01)
        {
            error_sum += weights[i];
            active_sensors_count++;
        }
    }

    if (active_sensors_count == 0)
    {
        return last_error > 0 ? 40 : -40;
    }
    else
    {
        last_error = error_sum / active_sensors_count;
        return last_error;
    }
}

void update_lcd_display(void)
{
    char buf[40];
    // LCD_Clear(BLACK);
    static PathState_e last_state = -1; // ��¼��һ�ε�״̬
    if (g_path_state != last_state)
    {
        LCD_Clear(BLACK);
        last_state = g_path_state;
    }

    switch (g_path_state)
    {
    case S_CALIBRATE:
        // "Encoder Calibration" ��Ϊ������ʾ����������Y����˳��
        LCD_ShowString(10, 10, (uint8_t *)"Encoder", YELLOW, BLACK, 16, 0);     // <-- �޸�
        LCD_ShowString(10, 30, (uint8_t *)"Calibration", YELLOW, BLACK, 16, 0); // <-- �޸�

        sprintf(buf, "L: %ld", (long)g_encoder_l_total);
        LCD_ShowString(10, 50, (uint8_t *)buf, WHITE, BLACK, 16, 0); // <-- Y��������
        sprintf(buf, "R: %ld", (long)g_encoder_r_total);
        LCD_ShowString(10, 70, (uint8_t *)buf, WHITE, BLACK, 16, 0); // <-- Y��������

        // "S1: Reset Counters" ��Ϊ������ʾ
        LCD_ShowString(10, 90, (uint8_t *)"S1: Reset", WHITE, BLACK, 16, 0);     // <-- �޸�
        LCD_ShowString(34, 110, (uint8_t *)"Counters", WHITE, BLACK, 16, 0);     // <-- �޸� (������)
        LCD_ShowString(10, 130, (uint8_t *)"S2: Continue", GREEN, BLACK, 16, 0); // <-- Y��������
        break;

    case S0_SELECT_LAPS:
        LCD_ShowString(10, 10, (uint8_t *)"Select Laps:", YELLOW, BLACK, 16, 0);
        sprintf(buf, "%d", g_laps_to_run);
        LCD_ShowString(110, 10, (uint8_t *)buf, WHITE, BLACK, 16, 0);

        // "S1:Change S2:Confirm" ���Ϊ����������ָ�������
        LCD_ShowString(10, 50, (uint8_t *)"S1: Change", WHITE, BLACK, 16, 0);  // <-- �޸�
        LCD_ShowString(10, 70, (uint8_t *)"S2: Confirm", WHITE, BLACK, 16, 0); // <-- �޸�
        break;

    case S0_WAIT_FOR_START:
        sprintf(buf, "Run for %d lap(s)?", g_laps_to_run);
        LCD_ShowString(10, 30, (uint8_t *)buf, YELLOW, BLACK, 16, 0);

        // "Press S2 to Start" ��Ϊ������ʾ
        LCD_ShowString(10, 50, (uint8_t *)"Press S2", GREEN, BLACK, 16, 0); // <-- �޸�
        LCD_ShowString(10, 70, (uint8_t *)"to Start", GREEN, BLACK, 16, 0); // <-- �޸�
        break;

    case S1_LINE_FOLLOWING:
    case S1_START_TO_GO:
    case S2_PREPARE_TURN_FORWARD:
    case S2_TURNING_LEFT:
        sprintf(buf, "L:%d/%d C:%d", (g_corner_count) / 4, g_laps_to_run, g_corner_count);
        LCD_ShowString(10, 10, (uint8_t *)buf, WHITE, BLACK, 16, 0);

        // ������ State ��Ϣ��ͳһΪ���У����ֽ���һ����
        if (g_path_state == S1_LINE_FOLLOWING)
        {
            LCD_ShowString(10, 30, (uint8_t *)"State:", YELLOW, BLACK, 16, 0);    // <-- �޸�
            LCD_ShowString(10, 50, (uint8_t *)"Following", YELLOW, BLACK, 16, 0); // <-- �޸�
        }
        if (g_path_state == S2_PREPARE_TURN_FORWARD)
        {
            LCD_ShowString(10, 30, (uint8_t *)"State:", CYAN, BLACK, 16, 0);     // <-- �޸�
            LCD_ShowString(10, 50, (uint8_t *)"Go Forward", CYAN, BLACK, 16, 0); // <-- �޸�
        }
        if (g_path_state == S2_TURNING_LEFT)
        {
            LCD_ShowString(10, 30, (uint8_t *)"State:", CYAN, BLACK, 16, 0);  // <-- �޸�
            LCD_ShowString(10, 50, (uint8_t *)"Turning", CYAN, BLACK, 16, 0); // <-- �޸�
        }
        if (g_path_state == S1_START_TO_GO)
        {
            LCD_ShowString(10, 30, (uint8_t *)"State:", CYAN, BLACK, 16, 0); // <-- �޸�
            LCD_ShowString(10, 50, (uint8_t *)"Start", CYAN, BLACK, 16, 0);  // <-- �޸�
        }
        break;

    case S3_STOPPED:
        LCD_ShowString(10, 30, (uint8_t *)"Finished!", GREEN, BLACK, 24, 0);
        break;
    }
}
// ===============================================
//           SysTick ??????
// ===============================================
void SysTick_Handler(void)
{
    // ���ڷ�ֹͣ״̬���ۼƱ�����ֵ
    if (g_path_state == S1_LINE_FOLLOWING || ( (g_path_state == S2_TURNING_LEFT) && (g_corner_count % 4 == 1)) )
    {
        // Speed_L��Speed_R��motor.c��GROUP1_IRQHandler�б�����
        // SysTickÿ1ms�ж�һ�Σ�����ֱ���ۼ�˲ʱ������
        g_encoder_l_total += Speed_L;
        g_encoder_r_total += Speed_R;

        // motor.c�е�GetSpeed����������Speed_L/R��������Ҫ�����������
        // ��Ϊ�˲�����PID����������ֱ������������
        // Speed_L = 0;
        // Speed_R = 0;
    }

    if (g_delay_times > 0)
        g_delay_times--;

    for (int i = 0; i < NUM_KEYS; i++)
    {
        key_tick_handler(&my_keys[i]);
    }

    g_systick_count++;

    // ÿ20msִ��һ��״̬��
    if (g_systick_count % 20 == 0)
    {
        square_path_fsm();
        motor_speed_pid_update();
    }
}

void uart_printf(UART_Regs *uart, const char *format, ...)
{
    // ��̬�����������ڴ洢��ʽ������ַ���
    static char buffer[UART_PRINT_BUFFER_SIZE];

    // va_list ���ڴ����ɱ����
    va_list args;

    // �� format ������ʼ��ʼ�� va_list
    va_start(args, format);

    // ʹ�� vsnprintf ��ȫ�ؽ���ʽ������д�뻺����
    // ���᷵��ʵ��д����ַ��� (������ĩβ�� '\0')
    // ����ַ���̫���������Զ��ض��Է�ֹ���
    vsnprintf(buffer, sizeof(buffer), format, args);

    // ���� va_list
    va_end(args);
    // ���ַ�ͨ�� UART ����
    for (char *p = buffer; *p; p++)
    {
        // ========================[ ���ǹؼ������� ]========================
        // �ڷ�����һ���ֽ�ǰ��ѭ���ȴ���ֱ�����ͻ�����Ϊ�ա�
        // DL_UART_INTERRUPT_TX_EMPTY ��־λ��Ӳ����1ʱ����ʾ���Է����ˡ�
        // =================================================================
        //        while (!(DL_UART_getInterruptStatus(uart, DL_UART_INTERRUPT_TX_EMPTY)))
        //        {
        //            // �ȴ�...
        //        }
        //
        //        // ���ڿ��԰�ȫ�ط�����һ���ֽ���
        //        DL_UART_transmitData(uart, (uint8_t)*p);

        while (DL_UART_Main_isTXFIFOFull(uart) == true)
            ;

        // ���ַ����ͳ�ȥ
        DL_UART_Main_transmitData(uart, (uint8_t)*p);
    }
}