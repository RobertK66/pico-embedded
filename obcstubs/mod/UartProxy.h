#ifndef UARTPROXY_H
#define UARTPROXY_H

#include "../../common/UartBase.h"
#include "Usb.h"


#ifdef __cplusplus
#include <list>

class UartProxy : public UartBase {
  public:
    UartProxy(uart_config *conf, char *pname, int cdc, Usb *usb);
    void main() override;
    //void Reconfigure(uint32_t baud, uint8_t bit, uint8_t stop, uint8_t parity );

  private:
    int CdcNr;
    Usb *pUsb;
    int StuckByte;
  };
#else
  typedef
    struct UartProxy
      UartProxy;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
  extern UartProxy* createInstanceUartProxy(uart_config *conf, char* pNamem,int cdc, Usb *usb);

  //extern void ReconfigureUp(uint32_t baud, uint8_t bit, uint8_t stop, uint8_t parity );
#else
// ??? brauch ich das je ????
  //extern void c_function();         /* K&R style */
  //extern Fred* runLoopFromCCode();
#endif
#ifdef __cplusplus
}
#endif
#endif /*UARTPROXY_H*/