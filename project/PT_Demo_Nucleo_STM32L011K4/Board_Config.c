/******************************************************************************
* File       : Board_Config.c
* Function   : Hardware configurations
* description: To be done.           
* Version    : V1.00
* Author     : Ian
* Date       : 3rd Jul 2016
* History    :  No.  When           Who           What
*               1    22/Nov/2016    Ian           Create
******************************************************************************/
#include "type_def.h"
#include "common_head.h"
#include "project_config.h"
#include "MOE_Core.h"
#include "MOE_Event.h"
#include "MOE_Timer.h"
#include "MOE_Msg.h"
#include "Board_Config.h"
#include "debug.h"
#include "stm32l011xx.h"
#include "core_cm0plus.h"
#include "stm32l0xx_hal.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void GPIO_Init(void);
static void USART2_UART_Init(void);


volatile static uint32 sg_u32SysClk = 0;
UART_HandleTypeDef huart2;
/******************************************************************************
* Name       : void Board_Init(void)
* Function   : Init board includes system clock, GPIOs, Uart and Systick
* Input      : None
* Output:    : None
* Return     : None
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 21st Nov 2016
******************************************************************************/
void Board_Init(void)
{
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    GPIO_Init();
    USART2_UART_Init();

    SysTick_Ms_Int_Init(SYSTICK_CNT_FOR_1_MS);
    return;
}

void SysTick_Ms_Int_Init(uint32_t u32Tick)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;  /* Set clock source = core clock */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;    /* Enable systick interrupt      */
    SysTick->LOAD = u32Tick;                      /* Set tick count for 1ms        */
    SysTick->VAL  = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     /* Enable SysTick                */
    return;   
}

uint32 SysTick_GetSysClk(void)
{
    return sg_u32SysClk;
}

void SysTick_Handler(void)
{
    volatile uint32_t u32Temp;
    u32Temp = SysTick->CTRL;
    sg_u32SysClk++;
    return;
}

void out_char(char u8C)
{
    while(!(USART2->ISR & USART_ISR_TXE_Msk));  
    USART2->TDR = u8C;
}

/** System Clock Configuration*/
static void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
      
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                 |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART2 init function */
static void USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&huart2);
}

/** Configure pins as */
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LD3_Pin */
    GPIO_InitStruct.Pin = LD3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);
}

void LED_On(uint8 u8Ch)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

void LED_Off(uint8 u8Ch)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}


/* End of file */
