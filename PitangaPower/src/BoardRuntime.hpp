#pragma once

class BoardRuntime final
{
public:
    explicit BoardRuntime();
    ~BoardRuntime() = default;
    BoardRuntime(const BoardRuntime&) = delete;
    BoardRuntime& operator=(const BoardRuntime&) = delete;
    BoardRuntime(BoardRuntime&&) = delete;
    BoardRuntime& operator=(BoardRuntime&&) = delete;
};