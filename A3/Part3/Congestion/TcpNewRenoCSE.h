#ifndef TCPNEWRENOCSE_H
#define TCPNEWRENOCSE_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"

namespace ns3 {

class TcpNewRenoCSE : public TcpNewReno
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create an unbound tcp socket.
   */
  TcpNewRenoCSE (void);

  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpNewRenoCSE (const TcpNewRenoCSE& sock);
  virtual ~TcpNewRenoCSE (void);

  virtual std::string GetName () const;

  virtual Ptr<TcpCongestionOps> Fork ();

protected:
  virtual uint32_t SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

};

} 

#endif 
