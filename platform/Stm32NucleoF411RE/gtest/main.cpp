#include "stm32f4xx_hal.h"

#include "gtest/gtest.h"

#include <array>
#include <cstdio>

static UART_HandleTypeDef UartHandle;

static void BSP_LED2_Init();
static void BSP_LED2_DeInit();
static void BSP_LED2_On();
static void BSP_LED2_Off();

static void SystemClock_Config(void);
static void Error_Handler(void);

template <typename... Args>
static auto to_argv(Args... str)
{
    static_assert(std::conjunction_v<std::is_convertible<Args, char*>...>, "Args must be convertible to non-const char*");
    std::array argv { static_cast<char*>(str)..., static_cast<char*>(nullptr) };
    return argv;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    BSP_LED2_Init();

    UartHandle.Instance = USART2;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        Error_Handler();
    }

    char arg0[] = "gtest_main_on_nucleo";
    char arg1[] = "--gtest_color=yes";
    auto argv = to_argv(arg0, arg1);
    int argc = argv.size() - 1;

    puts("\r\n\r\n");
    testing::InitGoogleTest(&argc, argv.data());
    auto error = RUN_ALL_TESTS();

    if (error) {
        Error_Handler();
    }

    BSP_LED2_DeInit();

    while (1) {
    }
}

/**
  * @brief  Configure LED2 GPIO.
  */
static void BSP_LED2_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    BSP_LED2_Off();
}

/**
  * @brief  DeInit LED2.
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  */
static void BSP_LED2_DeInit()
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Turn off LED */
    BSP_LED2_Off();
    /* DeInit the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN_5;
    HAL_GPIO_DeInit(GPIOA, gpio_init_structure.Pin);
}

/**
  * @brief  Turn LED2 On.
  */
static void BSP_LED2_On()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

/**
  * @brief  Turn LED2 Off.
  */
static void BSP_LED2_Off()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

static void Error_Handler(void)
{
    BSP_LED2_On();
    while (1) {
    }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 400
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 0x10;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 400;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }
}

extern "C" {

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&UartHandle, (uint8_t*)&ch, 1, 0xFFFF);
    return ch;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_getchar(void)
{
    HAL_StatusTypeDef status = HAL_BUSY;
    uint8_t ch;
    while (status != HAL_OK) {
        status = HAL_UART_Receive(&UartHandle, &ch, 1, 10);
    }
    return ch;
}

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Enable USARTx clock */
    __HAL_RCC_USART2_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    Error_Handler();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    Error_Handler();
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    Error_Handler();
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    Error_Handler();
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief Stubbed getcwd for GTest always returning "./" unless size is too small.
 */
char* getcwd(char* buf, size_t size)
{
    const char cwd[] = "./";
    if (sizeof(cwd) > size) {
        return NULL;
    }
    strncpy(buf, "./", size);
    return buf;
}

/**
 * @brief Stubbed mkdir for GTest always returning a "permission denied" error.
 */
int mkdir(const char* /*path*/, mode_t /*mode*/)
{
    errno = EPERM;
    return -1;
}

#include <sys/time.h>

/**
 * Definition of POSIX syscalls for a STM32 bare-metal target.
 * See https://sourceware.org/newlib/libc.html#Syscalls for more information.
 */

register char* stack_ptr asm("sp");

caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char* heap_end;
    char* prev_heap_end;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;

    if (heap_end + incr > stack_ptr) {
        //		write(1, "Heap and stack collision\n", 25);
        //		abort();
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    /* Return fixed data for the timezone.  */
    if (tzp) {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    if (tp) {
        tp->tv_sec = 0;
        tp->tv_usec = HAL_GetTick() * 1000;
    }

    return 0;
}
void initialise_monitor_handles()
{
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{
    _kill(status, -1);
    while (1) { }
}

int _write(int file, char* ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _read(int file, char* ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = __io_getchar();
    }

    return len;
}

int _open(char* path, int flags, ...)
{
    /* Pretend like we always fail */
    return -1;
}

int _wait(int* status)
{
    errno = ECHILD;
    return -1;
}

int _unlink(char* name)
{
    errno = ENOENT;
    return -1;
}

int _times(struct tms* buf)
{
    return -1;
}

int _stat(char* file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _link(char* oldpath, char* newpath)
{
    errno = EMLINK;
    return -1;
}

int _fork(void)
{
    errno = EAGAIN;
    return -1;
}

int _execve(char* name, char** argv, char** env)
{
    errno = ENOMEM;
    return -1;
}
}