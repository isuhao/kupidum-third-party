/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_VIDEO_CODING_SESSION_INFO_H_
#define WEBRTC_MODULES_VIDEO_CODING_SESSION_INFO_H_

#include "typedefs.h"
#include "module_common_types.h"
#include "packet.h"

namespace webrtc
{

enum { kMaxVP8Partitions = 9 };

class VCMSessionInfo
{
public:
    VCMSessionInfo();
    virtual ~VCMSessionInfo();

    VCMSessionInfo(const VCMSessionInfo& rhs);

    void UpdateDataPointers(const WebRtc_UWord8* frame_buffer,
                            const WebRtc_UWord8* prev_buffer_address);

    WebRtc_Word32 ZeroOutSeqNum(WebRtc_Word32* list,
                                WebRtc_Word32 numberOfSeqNum);
    // Hybrid version: Zero out seq num for NACK list
    // apply a score based on the packet location and the external rttScore
    WebRtc_Word32 ZeroOutSeqNumHybrid(WebRtc_Word32* list,
                                      WebRtc_Word32 numberOfSeqNum,
                                      float rttScore);
    virtual void Reset();

    WebRtc_Word64 InsertPacket(const VCMPacket& packet,
                               WebRtc_UWord8* ptrStartOfLayer);
    WebRtc_Word32 InformOfEmptyPacket(const WebRtc_UWord16 seqNum);

    virtual bool IsSessionComplete();

    // Builds fragmentation headers for VP8, each fragment being a decodable
    // VP8 partition. Returns the total number of bytes which are decodable. Is
    // used instead of MakeDecodable for VP8.
    int BuildVP8FragmentationHeader(WebRtc_UWord8* frame_buffer,
                                    int frame_buffer_length,
                                    RTPFragmentationHeader* fragmentation);

    // Makes the frame decodable. I.e., only contain decodable NALUs. All
    // non-decodable NALUs will be deleted and packets will be moved to in
    // memory to remove any empty space.
    // Returns the number of bytes deleted from the session.
    WebRtc_UWord32 MakeDecodable(WebRtc_UWord8* ptrStartOfLayer);

    WebRtc_UWord32 GetSessionLength();
    bool HaveLastPacket();
    void ForceSetHaveLastPacket();
    bool IsRetransmitted();
    webrtc::FrameType FrameType() const { return _frameType; }

    virtual WebRtc_Word32 GetHighestPacketIndex();
    virtual void UpdatePacketSize(WebRtc_Word32 packetIndex,
                                  WebRtc_UWord32 length);

    void SetStartSeqNumber(WebRtc_UWord16 seqNumber);

    bool HaveStartSeqNumber();

    WebRtc_Word32 GetLowSeqNum() const;
    // returns highest seqNum, media or empty
    WebRtc_Word32 GetHighSeqNum() const;

    WebRtc_UWord32 PrepareForDecode(WebRtc_UWord8* ptrStartOfLayer,
                                    VideoCodecType codec);

    void SetPreviousFrameLoss() { _previousFrameLoss = true; }
    bool PreviousFrameLoss() const { return _previousFrameLoss; }

protected:
    // Finds the packet index of the next VP8 partition. If none is found
    // _highestPacketIndex + 1 is returned.
    int FindNextPartitionBeginning(int packet_index) const;
    // Finds the packet index of the end of the partition with index
    // partitionIndex.
    int FindPartitionEnd(int packet_index) const;
    static bool InSequence(WebRtc_UWord16 seqNum, WebRtc_UWord16 prevSeqNum);
    WebRtc_UWord32 InsertBuffer(WebRtc_UWord8* ptrStartOfLayer,
                                WebRtc_Word32 packetIndex,
                                const VCMPacket& packet);
    void FindNaluBorder(WebRtc_Word32 packetIndex,
                        WebRtc_Word32& startIndex,
                        WebRtc_Word32& endIndex);
    WebRtc_UWord32 DeletePackets(WebRtc_UWord8* ptrStartOfLayer,
                                 WebRtc_Word32 startIndex,
                                 WebRtc_Word32 endIndex);
    void UpdateCompleteSession();
    // If we have inserted a packet with markerbit into this frame
    bool               _markerBit;
    // If this session has been NACKed by JB
    bool               _sessionNACK;
    bool               _completeSession;
    webrtc::FrameType  _frameType;
    bool               _previousFrameLoss;
    // Lowest/Highest packet sequence number in a session
    WebRtc_Word32      _lowSeqNum;
    WebRtc_Word32      _highSeqNum;

    // Highest packet index in this frame
    WebRtc_UWord16     _highestPacketIndex;
    // Packets in this frame.
    // TODO(holmer): Replace this with a std::list<VCMPacket*>. Doing that will
    //               make it possible to get rid of _markerBit, _lowSeqNum,
    //               _highSeqNum, _highestPacketIndex, etc.
    VCMPacket          _packets[kMaxPacketsInJitterBuffer];
    WebRtc_Word32      _emptySeqNumLow;
    WebRtc_Word32      _emptySeqNumHigh;
    // Store the sequence number that marks the last media packet
    WebRtc_Word32      _markerSeqNum;
};

} // namespace webrtc

#endif // WEBRTC_MODULES_VIDEO_CODING_SESSION_INFO_H_
