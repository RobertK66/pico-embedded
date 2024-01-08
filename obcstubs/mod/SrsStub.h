#ifndef SRSSTUB_H
#define SRSSTUB_H

#include "../../common/Module.h"


#define SRS_POWER_ADR       0x47
#define SRS_CTRL_ADR        0x0B
#define SRS_BUFFER_SIZE     1050


#ifdef __cplusplus
#include <string>

enum srsbus_status
{
    idle, RxPowerbus, RxRequest,
};


class SrsStub : public Module {
  public:
    SrsStub(uint8_t *txBuffer);
    void main() override;
    void ReceiveByte(uint8_t b, bool isAddress);
    void GenerateAnswer(uint8_t address);

  private:
    uint8_t *pTxData;
    uint8_t RxData[SRS_BUFFER_SIZE];
    uint16_t RxIdx;
    srsbus_status Status;
    //std::ostringstream Message;
    std::string Message;
  };
#else
  typedef
    struct SrsStub
      SrsStub;
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC__) || defined(__cplusplus)
  extern SrsStub* createInstanceSrsStub(uint8_t *txBuffer);
  extern void srs_receive_handler(SrsStub* inst, uint8_t data, bool is_address);
  extern void srs_request_handler(SrsStub* inst, uint8_t address);
//   extern void srs_stop_handler(SrsStub* inst, uint8_t length);

#else
// ??? brauch ich das je ????
  //extern void c_function();         /* K&R style */
  //extern Fred* runLoopFromCCode();
#endif
#ifdef __cplusplus
}
#endif
#endif /*SRSSTUB_H*/