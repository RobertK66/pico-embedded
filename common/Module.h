/* This header can be read by both C and C++ compilers */

#ifndef MODULE_H
#define MODULE_H
#ifdef __cplusplus
#include <string>
  class Module {
  public:
    Module(char *);
    virtual void main();
    virtual void init(void *);
    virtual void executeCommand(int nr, int cnt, char** par) = 0;
    std::string Name;
    int loopCnt = 1;
  private:
    void *pInitdata;
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
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*MODULE_H*/