#pragma once

#include <QtGlobal> // 為了使用 qint64

// IClock 是抽象介面，定義時間控制相關行為
struct IClock {
    virtual ~IClock() = default;  // 虛擬解構子，確保能安全透過指標刪除子類

    virtual void start() = 0;         // 啟動計時器
    virtual void stop() = 0;          // 停止計時器（可選）
    virtual qint64 elapsedMs() const = 0; // 取得目前經過的毫秒數
};