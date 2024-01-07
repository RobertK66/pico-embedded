#ifndef USB_H
#define USB_H
#include "../../common/Module.h"

//#include "UsbBase.h"

#include "../../common/UartBase.h"


#define BUFFER_SIZE 1500

#define READ_NODATA -1

#ifdef __cplusplus
  #include <queue>
  #include <array>

  struct usb_buffer {
      bool con;
      UartBase *pUart;
      uint8_t RxBuffer[BUFFER_SIZE];
      uint16_t RxIdx;
      uint16_t ReadIdx;
  };

  class Usb : public Module {
  public:
    Usb();
    void main() override;
    void init(void *) override;
    void executeCommand(int nr, int cnt, char** par) override;
    int readNextRxByte(uint8_t nr);

    void SetUartProxy(UartBase *pCdc1Uart, UartBase *pCdc2Uart);
  private:
    //mutex_t    rxMutex;
    usb_buffer usbdata[2];
  };
#else
  typedef
    struct Usb
      Usb;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
    extern Usb* createInstanceUsb(void);
    extern void SetUartProxy(Usb* inst, UartBase *pCdc1Uart, UartBase *pCdc2Uart);
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*USB_H*/