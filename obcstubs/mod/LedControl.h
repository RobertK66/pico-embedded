#ifndef LEDCONTROL_H
#define LEDCONTROL_H
#include "../../common/Module.h"

#define CMD_LED_RGB   1
#define CMD_LED_FLASH 2

#ifdef __cplusplus
  class LedControl : public Module {
  public:
    LedControl(void);
    void main() override;
    void init(void *) override;
    void executeCommand(int nr, int cnt, char** par) override;
  
  private:
    int rval, gval, bval;
  };
#else
  typedef
    struct LedControl
      LedControl;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
    extern LedControl* createInstanceLedControl(void);
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*LEDCONTROL_H*/