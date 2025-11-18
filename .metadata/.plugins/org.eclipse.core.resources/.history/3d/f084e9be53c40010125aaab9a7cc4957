#include "stm32l476xx.h"
#include "system_clock.h"
#include "i2c_irq.h"

volatile uint8_t done_flag = 0;

int main(void)
{
    SystemClock_HSI16();
    i2c1_irq_init();

    uint8_t dev_addr = 0x68; // any address (no sensor needed)

    // Generate START in write mode
    i2c1_irq_start(dev_addr, I2C_WRITE);

    // Wait for STOP interrupt
    while (!i2c_transfer_done);

    done_flag = 1; // check in debugger

    while(1);
}
