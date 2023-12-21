/* This header can be read by both C and C++ compilers */

#ifndef MODULE_H
#define MODULE_H
#ifdef __cplusplus
#include <string>
  class Module {
  public:
    Module(void * p);
    //Module(std::string name);
    void main();
    void init(void *);
    std::string Name;
    //void init(void *initdata);
  private:
    void *pInitdata;
    int loopCnt = 1;
   
  };
#else
  typedef
    struct Module
      Module;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
    extern Module* createInstanceModule(char*);
  //extern Module* cplusplus_callback_function_m(Module*);
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*MODULE_H*/