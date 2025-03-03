// Copyright (c) 2024 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Sensor/AsyncDataStream.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/streaming/Stream.h>
#include <optional>
#include <compiler/enable-ue4-macros.h>

// =============================================================================
// -- FDataStreamTmpl ----------------------------------------------------------
// =============================================================================

/// A streaming channel for sending sensor data to clients. Each sensor has its
/// own FDataStream. Note however that this class does not provide a send
/// function. In order to send data, a FAsyncDataStream needs to be created
/// using "MakeAsyncDataStream" function. FAsyncDataStream allows sending data
/// from any thread.
template <typename T>
class FDataStreamTmpl
{
public:

  using StreamType = T;

  FDataStreamTmpl() = default;

  FDataStreamTmpl(StreamType InStream) : Stream(std::move(InStream)) {}

  /// Create a FAsyncDataStream object.
  ///
  /// @pre This functions needs to be called in the game-thread.
  template <typename SensorT>
  FAsyncDataStreamTmpl<T> MakeAsyncDataStream(const SensorT &Sensor, double Timestamp)
  {
    check(Stream.has_value());
    return FAsyncDataStreamTmpl<T>{Sensor, Timestamp, *Stream};
  }

  bool IsStreamReady()
  {
    return Stream.has_value();
  }

  /// Return the token that allows subscribing to this stream.
  auto GetToken() const
  {
    check(Stream.has_value());
    return Stream->token();
  }

  uint64_t GetSensorType()
  {
    check(Stream.has_value());
    return Stream->get_stream_id();
  }

  bool AreClientsListening()
  {
    check(Stream.has_value());
    return Stream->AreClientsListening();
  }

private:

  std::optional<StreamType> Stream;
};

// =============================================================================
// -- FDataStream and FDataMultiStream -----------------------------------------
// =============================================================================

using FDataStream = FDataStreamTmpl<carla::streaming::Stream>;

using FDataMultiStream = FDataStreamTmpl<carla::streaming::Stream>;
