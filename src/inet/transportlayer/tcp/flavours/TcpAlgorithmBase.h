//
// Copyright (C) 2004 OpenSim Ltd.
// Copyright (C) 2009-2010 Thomas Reschka
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef __INET_TCPALGORITHMBASE_H
#define __INET_TCPALGORITHMBASE_H

#include "inet/transportlayer/tcp/flavours/TcpAlgorithmBaseState_m.h"
#include "inet/transportlayer/tcp/TcpAlgorithm.h"

namespace inet {
namespace tcp {

/**
 * Includes basic TCP algorithms: adaptive retransmission, PERSIST timer,
 * keep-alive, delayed acks -- EXCLUDING congestion control. Congestion
 * control is implemented in subclasses such as TCPTahoeAlg or TCPRenoAlg.
 *
 * Implements:
 *   - delayed ACK algorithm (RFC 1122)
 *   - Jacobson's and Karn's algorithms for adaptive retransmission
 *   - Nagle's algorithm (RFC 896) to prevent silly window syndrome
 *   - Increased Initial Window (RFC 3390)
 *   - PERSIST timer
 *
 * To be done:
 *   - KEEP-ALIVE timer
 *
 * Note: currently the timers and time calculations are done in double
 * and NOT in Unix (200ms or 500ms) ticks. It's possible to write another
 * TcpAlgorithm which uses ticks (or rather, factor out timer handling to
 * separate methods, and redefine only those).
 *
 * Congestion window is set to SMSS when the connection is established,
 * and not touched after that. Subclasses may redefine any of the virtual
 * functions here to add their congestion control code.
 */
class INET_API TcpAlgorithmBase : public TcpAlgorithm
{
  protected:
    TcpAlgorithmBaseStateVariables *& state; // alias to TcpAlgorithm's 'state'
    ITcpCongestionControl *congestionControl = nullptr;
    ITcpRecovery *recovery = nullptr;

    cMessage *rexmitTimer;
    cMessage *persistTimer;
    cMessage *delayedAckTimer;
    cMessage *keepAliveTimer;

  protected:
    /** @name Process REXMIT, PERSIST, DELAYED-ACK and KEEP-ALIVE timers */
    //@{
    virtual void processRexmitTimer(TcpEventCode& event);
    virtual void processPersistTimer(TcpEventCode& event);
    virtual void processDelayedAckTimer(TcpEventCode& event);
    virtual void processKeepAliveTimer(TcpEventCode& event);
    //@}

    /**
     * Start REXMIT timer and initialize retransmission variables
     */
    virtual void startRexmitTimer();

    /**
     * Update state vars with new measured RTT value. Passing two simtime_t's
     * will allow rttMeasurementComplete() to do calculations in double or
     * in 200ms/500ms ticks, as needed)
     */
    virtual void rttMeasurementComplete(simtime_t tSent, simtime_t tAcked);

    /**
     * Converting uint32_t echoedTS to simtime_t and calling rttMeasurementComplete()
     * to update state vars with new measured RTT value.
     */
    virtual void rttMeasurementCompleteUsingTS(uint32_t echoedTS) override;

    /**
     * Send data, observing Nagle's algorithm and congestion window
     */
    virtual bool sendData(bool sendCommandInvoked);

    virtual void receivedDuplicateAck();

    /** Utility function */
    cMessage *cancelEvent(cMessage *msg) { return conn->cancelEvent(msg); }

  public:
    /**
     * Ctor.
     */
    TcpAlgorithmBase();

    /**
     * Virtual dtor.
     */
    virtual ~TcpAlgorithmBase();

    /**
     * Create timers, etc.
     */
    virtual void initialize() override;

    virtual void established(bool active) override;

    virtual void connectionClosed() override;

    /**
     * Process REXMIT, PERSIST, DELAYED-ACK and KEEP-ALIVE timers.
     */
    virtual void processTimer(cMessage *timer, TcpEventCode& event) override;

    virtual void sendCommandInvoked() override;

    virtual void receivedOutOfOrderSegment() override;

    virtual void receiveSeqChanged() override;

    virtual void receivedAckForAlreadyAckedData(const TcpHeader *tcpHeader, uint32_t payloadLength) override;

    virtual void receivedAckForUnackedData(uint32_t firstSeqAcked) override;

    virtual void receivedAckForUnsentData(uint32_t seq) override;

    virtual void ackSent() override;

    virtual void dataSent(uint32_t fromseq) override;

    virtual void segmentRetransmitted(uint32_t fromseq, uint32_t toseq) override;

    virtual void restartRexmitTimer() override;

    virtual bool shouldMarkAck() override;

    virtual void processEcnInEstablished() override;

    virtual ITcpCongestionControl *getCongestionControl() override { return congestionControl; }

    virtual ITcpRecovery *getRecovery() override { return recovery; }
};

} // namespace tcp
} // namespace inet

#endif

