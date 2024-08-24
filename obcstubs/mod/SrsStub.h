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

struct srsdata {
  uint64_t PosixTime;
  uint32_t intervalFgDos = 100;
  uint32_t intervalRadFet = 1000;
  uint32_t intervalSram = 10042;
  uint8_t  commonStatus[12]    = {0xcc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b};
  uint8_t  extendedStatus[30]  = {0xee, 0x10, 0x11, 0x12};  //... all filled with 0
};


class SrsStub : public Module {
  public:
    SrsStub(uint8_t *txBuffer);
    void main() override;
    void ReceiveByte(uint8_t b, bool isAddress);
    void GenerateAnswer(uint8_t address);

  private:
    uint8_t CalculateCrc(uint8_t *p, int len);
    uint8_t *pTxData;
    uint8_t RxData[SRS_BUFFER_SIZE];
    uint16_t RxIdx;
    srsbus_status Status;
    std::string Message;

    srsdata data;
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