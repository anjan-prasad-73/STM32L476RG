#include "stm32l476xx.h"
#include "system_clock.h"
#include "uart_irq.h"

int main(void)
{
    SystemClock_HSI16();
    usart2_irq_init(115200);

    // Send startup message
    const char msg[] = "UART IRQ READY\r\n";
    for (int i = 0; msg[i] != '\0'; i++)
        usart2_write(msg[i]);

    while (1)
    {
        if (uart_byte_received)
        {
            uart_byte_received = 0;

            // Echo last byte back
            uint8_t last = uart_rx_buffer[uart_rx_index - 1];
            usart2_write(last);
        }
    }
}
