#ifndef CLIMODULE_H
#define CLIMODULE_H
#include "Module.h"



#ifdef __cplusplus
#include <list>
  struct Cmd;
  class CliModule : public Module {
  public:
    CliModule(void);
    void main() override;
    void init(void *) override;
    int addCommand(const char*p, Module* m, int cnr);
  private:
    std::list<Cmd*> commands;
  };
#else
  typedef
    struct CliModule
      CliModule;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
    extern CliModule* createInstanceCliModule(void);
    extern int cliRegisterCommand(CliModule* c, const char*p, Module* m, int cnr);
#else
  
  //extern Fred* cplusplus_callback_function();
#endif
#ifdef __cplusplus
}
#endif
#endif /*CLIMODULE_H*/