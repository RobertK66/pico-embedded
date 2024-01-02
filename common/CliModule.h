#ifndef CLIMODULE_H
#define CLIMODULE_H
#include "Module.h"


#define CLI_RXBUFFER_SIZE 128


#ifdef __cplusplus
#include <list>
#include <string>

  struct Cmd;
  class CliModule : public Module {
  public:
    CliModule(void);
    void main() override;
    void init(void *) override;
    void executeCommand(int nr, int cnt, char** par) override;
    int addCommand(const char*p, Module* m, int cnr);
  private:
    std::list<Cmd*> commands;
    bool usbConnected = false;
    char rxBuffer[CLI_RXBUFFER_SIZE];
    int  rxPtrIdx = 55;
    void processLine(void);
    std::string cmdLine;
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