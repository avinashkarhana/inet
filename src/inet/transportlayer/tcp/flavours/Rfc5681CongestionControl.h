//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef __INET_RFC5681CONGESTIONCONTROL_H
#define __INET_RFC5681CONGESTIONCONTROL_H

#include "inet/transportlayer/tcp/flavours/TcpClassicAlgorithmBase.h"
#include "inet/transportlayer/tcp/ITcpCongestionControl.h"
#include "inet/transportlayer/tcp/TcpConnection.h"

namespace inet {
namespace tcp {

/**
 * Implements RFC 5681: TCP Congestion Control.
 */
class INET_API Rfc5681CongestionControl : public ITcpCongestionControl
{
  protected:
    TcpClassicAlgorithmBaseStateVariables *state = nullptr;
    TcpConnection *conn = nullptr;

  protected:
    virtual void slowStart(uint32_t numBytesAcked);
    virtual void congestionAvoidance(uint32_t numBytesAcked);

  public:
    Rfc5681CongestionControl(TcpStateVariables *state, TcpConnection *conn) : state(check_and_cast<TcpClassicAlgorithmBaseStateVariables *>(state)), conn(conn) { }

    virtual void receivedAckForUnackedData(uint32_t numBytesAcked) override;
};

} // namespace tcp
} // namespace inet

#endif

