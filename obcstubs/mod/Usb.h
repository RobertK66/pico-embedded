#ifndef USB_H
#define USB_H
#include "../../common/Module.h"


#ifdef __cplusplus
  class Usb : public Module {
  public:
    Usb(void);
    void main() override;
    void init(void *) override;
    void executeCommand(int nr, int cnt, char** par) override;
  
  private:
    //int rval, gval, bval;
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
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*USB_H*/