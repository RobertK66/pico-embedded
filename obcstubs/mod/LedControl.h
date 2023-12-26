#ifndef LEDCONTROL_H
#define LEDCONTROL_H
#include "../../common/Module.h"

#ifdef __cplusplus
  class LedControl : public Module {
  public:
    LedControl(void);
    void main() override;
    void init(void *) override;
  private:
    // int i = 42;    
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