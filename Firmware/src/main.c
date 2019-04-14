/**
 * @file       main.c
 * @ingroup    SNESoIP
 * @defgroup   SNESoIP
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <FreeRTOS.h>
#include <task.h>

static void _GPIO_Init(void);
static void _vTaskMain(void* pArg);

int main(void)
{
    _GPIO_Init();

    xTaskCreate(_vTaskMain, (const char*)"Main Task", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}

static void _GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void _vTaskMain(void* pArg)
{
    while (1)
    {
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief   Reports the name of the source file and the source line number
 *          where the assert_param error has occurred.
 * @param   file: pointer to the source file name
 * @param   line: assert_param error line source number
 * @retval  None
 */
void assert_failed(uint8_t* pu8File, uint32_t u32Line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif
