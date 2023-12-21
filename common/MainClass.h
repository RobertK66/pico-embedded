#ifndef MAINCLASS_H
#define MAINCLASS_H

#include "Module.h"

#ifdef __cplusplus
#include <list>

  class MainClass {
  public:
    MainClass(std::list<Module>);
    void runLoop(void);
    void addModule(Module*);
  private:
    std::list<Module> modules;
  };
#else
  typedef
    struct MainClass
      MainClass;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
  //extern void c_function(MainClass*);   /* ANSI C prototypes */
  extern MainClass* createInstanceMainClass(void);
  extern void runLoopMainClass(MainClass*);
  extern void addModuleMainClass(MainClass*, Module*);
#else
  //extern void c_function();         /* K&R style */
  //extern Fred* runLoopFromCCode();
#endif
#ifdef __cplusplus
}
#endif
#endif /*MAINCLASS_H*/