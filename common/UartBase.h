#ifndef UARTBASE_H
#define UARTBASE_H

#include "Module.h"
#include "pico/stdlib.h"

#define UART_BUFFER_SIZE 1000

 typedef struct {
      uart_inst_t *uart;
      uint8_t txPin;
      uint8_t rxPin;
      uint32_t bit_rate;
      uint8_t  stop_bits; ///< 0: 1 stop bit - 1: 1.5 stop bits - 2: 2 stop bits
      uint8_t  parity;    ///< 0: None - 1: Odd - 2: Even - 3: Mark - 4: Space
      uint8_t  data_bits; ///< can be 5, 6, 7, 8 or 16
  } uart_config;

struct uart_data {
      uart_config  *cfg;
      uint8_t TxBuffer[UART_BUFFER_SIZE];
      uint16_t TxIdx;
      uint16_t WriteIdx;
};

#ifdef __cplusplus
#include <list>

class UartBase : Module {
  public:
    void main() override;
    void init(void *) override;
    UartBase(uart_config *conf, char *pName);

    void Reconfigure(uint32_t baud, uint8_t bit, uint8_t stop, uint8_t parity );
    bool writeByteUart(uint8_t b);

  private:
    uart_data data;

  };
#else
  typedef
    struct UartBase
      UartBase;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
  extern UartBase* createInstanceUartBase(uart_hw_t *conf);
#else
// ??? brauch ich das je ????
  //extern void c_function();         /* K&R style */
  //extern Fred* runLoopFromCCode();
#endif
#ifdef __cplusplus
}
#endif
#endif /*UARTBASE_H*/