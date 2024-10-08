#include "pwm.h"
#include "conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

// Software PWM using timer (TIM1) and dma (DMA2 Stream 4 and 6)
// (Pin with no dedicated timer output)
#define PWM_PIN_IO_BASE GPIOE
#define PWM_PIN_ON      ((uint32_t)GPIO_PIN_3)
#define PWM_PIN_OFF     ((uint32_t)GPIO_PIN_3 << 16U)

#define DEFAULT_PWM_FREQ    200u
#define DEFAULT_PWM_DUTY    70u

#define PWM_MIN_FREQ    1u
#define PWM_MAX_FREQ    999999u
#define PERCENT         100u

#define SYS_CLOCK       100000000u

static TIM_HandleTypeDef timer =
{
    .Instance = TIM1,
    .Init.ClockDivision = 0u,
    .Init.CounterMode = TIM_COUNTERMODE_UP,
    .Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE,
};

static DMA_HandleTypeDef dma_s1 =
{
    .Instance = DMA2_Stream6,
    .Init.Channel  = DMA_CHANNEL_0,
    .Init.Direction = DMA_MEMORY_TO_PERIPH,
    .Init.PeriphInc = DMA_PINC_DISABLE,
    .Init.MemInc = DMA_MINC_DISABLE,
    .Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .Init.MemDataAlignment = DMA_MDATAALIGN_WORD,
    .Init.Mode = DMA_CIRCULAR,
    .Init.Priority = DMA_PRIORITY_LOW,
    .Init.FIFOMode = DMA_FIFOMODE_DISABLE,
    .Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL,
    .Init.MemBurst = DMA_MBURST_SINGLE,
    .Init.PeriphBurst = DMA_PBURST_SINGLE,
};

static DMA_HandleTypeDef dma_s2 =
{
    .Instance = DMA2_Stream4,
    .Init.Channel  = DMA_CHANNEL_6,
    .Init.Direction = DMA_MEMORY_TO_PERIPH,
    .Init.PeriphInc = DMA_PINC_DISABLE,
    .Init.MemInc = DMA_MINC_DISABLE,
    .Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .Init.MemDataAlignment = DMA_MDATAALIGN_WORD,
    .Init.Mode = DMA_CIRCULAR,
    .Init.Priority = DMA_PRIORITY_LOW,
    .Init.FIFOMode = DMA_FIFOMODE_DISABLE,
    .Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL,
    .Init.MemBurst = DMA_MBURST_SINGLE,
    .Init.PeriphBurst = DMA_PBURST_SINGLE,
};

static uint32_t dma_s1_src = PWM_PIN_ON;
static uint32_t dma_s2_src = PWM_PIN_OFF;

static bool _immediate_update = false;

static void _calc_timer_period(const uint32_t freq)
{
    // SYS_CLOCK / 65536u ~1536u (0x600)
    uint32_t prescaler = 1536u / freq;
    timer.Init.Prescaler = prescaler;
    timer.Init.Period = SYS_CLOCK / ((prescaler + 1u) * freq);
}

static uint32_t _calc_timer_pulse(const uint32_t duty)
{
    return (timer.Init.Period * duty) / PERCENT;
}

static void _update_timer(const uint32_t duty)
{
    timer.Instance->CCR4 = _calc_timer_pulse(duty);
    timer.Instance->ARR = timer.Init.Period;
    timer.Instance->PSC = timer.Init.Prescaler;
}

// -- Public Interface --
void pwm_init(void)
{
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    if (conf_is_valid() == false) {
        app_conf.c.pwm.freq = DEFAULT_PWM_FREQ;
        app_conf.c.pwm.duty = DEFAULT_PWM_DUTY;
        app_conf.c.pwm.inv = 0u;
    }

    if (0u != app_conf.c.pwm.inv) {
        dma_s1_src = PWM_PIN_OFF;
        dma_s2_src = PWM_PIN_ON;
    }

    _calc_timer_period(app_conf.c.pwm.freq);
    HAL_TIM_OC_Init(&timer);

    TIM_OC_InitTypeDef ch_conf;
    ch_conf.OCMode = TIM_OCMODE_ACTIVE;
    ch_conf.OCPolarity = TIM_OCPOLARITY_HIGH;

    ch_conf.Pulse = 0u;
    HAL_TIM_OC_ConfigChannel(&timer, &ch_conf, TIM_CHANNEL_1);

    ch_conf.Pulse = _calc_timer_pulse(app_conf.c.pwm.duty);
    HAL_TIM_OC_ConfigChannel(&timer, &ch_conf, TIM_CHANNEL_4);

    HAL_DMA_Init(&dma_s1);
    HAL_DMA_Start(&dma_s1, (uint32_t)&dma_s1_src, (uint32_t)&PWM_PIN_IO_BASE->BSRR, 1u);

    HAL_DMA_Init(&dma_s2);
    HAL_DMA_Start(&dma_s2, (uint32_t)&dma_s2_src, (uint32_t)&PWM_PIN_IO_BASE->BSRR, 1u);
}

void pwm_start(void)
{
    __HAL_TIM_ENABLE_DMA(&timer, TIM_DMA_CC1); // Enable only DMA trigger
    HAL_TIM_OC_Start(&timer, TIM_CHANNEL_1);

    __HAL_TIM_ENABLE_DMA(&timer, TIM_DMA_CC4); // Enable only DMA trigger
    HAL_TIM_OC_Start(&timer, TIM_CHANNEL_4);
}

void pwm_stop(const bool output_high)
{
    HAL_TIM_OC_Stop(&timer, TIM_CHANNEL_1);
    HAL_TIM_OC_Stop(&timer, TIM_CHANNEL_4);

    PWM_PIN_IO_BASE->BSRR = (true == output_high) ? dma_s1_src : dma_s2_src;
}

void pwm_invert_out(void)
{
    app_conf.c.pwm.inv ^= 1u;

    if (true == _immediate_update) {
        if (0u == app_conf.c.pwm.inv) {
            dma_s1_src = PWM_PIN_ON;
            dma_s2_src = PWM_PIN_OFF;
        } else {
            dma_s1_src = PWM_PIN_OFF;
            dma_s2_src = PWM_PIN_ON;
        }
    }
    conf_save();
}

bool pwm_toggle_immediate_update(void)
{
    _immediate_update = !_immediate_update;
    return _immediate_update;
}

bool pwm_set_freq(const uint32_t freq)
{
    if ((freq > PWM_MAX_FREQ) || (freq < PWM_MIN_FREQ)) {
        return false;
    }

    if (true == _immediate_update) {
        _calc_timer_period(freq);
        _update_timer(app_conf.c.pwm.duty);
    }

    app_conf.c.pwm.freq = freq;
    conf_save();

    return true;
}

uint32_t pwm_get_freq(void)
{
    return app_conf.c.pwm.freq;
}

bool pwm_set_duty(const uint32_t duty)
{
    if (duty > PERCENT){
        return false;
    } else if (true == _immediate_update) {
        if (PERCENT == duty)  {
            pwm_stop(true);
        } else if (0u == duty) {
            pwm_stop(false);
        } else {
            _update_timer(duty);
            pwm_start();
        }
    }

    app_conf.c.pwm.duty = duty;
    conf_save();

    return true;
}

uint32_t pwm_get_duty(void)
{
    return app_conf.c.pwm.duty;
}
