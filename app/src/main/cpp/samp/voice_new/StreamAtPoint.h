#pragma once

#include "../main.h"
#include "../game/common.h"

#include "LocalStream.h"
#include "Channel.h"

class StreamAtPoint : public LocalStream {
    StreamAtPoint() = delete;
    StreamAtPoint(const StreamAtPoint&) = delete;
    StreamAtPoint(StreamAtPoint&&) = delete;
    StreamAtPoint& operator=(const StreamAtPoint&) = delete;
    StreamAtPoint& operator=(StreamAtPoint&&) = delete;

public:
    explicit StreamAtPoint(uint32_t color, std::string name,
                           float distance, const CVector& position) noexcept;

    ~StreamAtPoint() noexcept = default;

public:
    void SetPosition(const CVector& position) noexcept;

private:
    void OnChannelCreate(const Channel& channel) noexcept override;

private:
    CVector position;
};

using StreamAtPointPtr = std::unique_ptr<StreamAtPoint>;
#define MakeStreamAtPoint std::make_unique<StreamAtPoint>
