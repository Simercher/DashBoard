#pragma once

// 提供系統資訊的抽象介面，例如電池、CPU 使用率等
struct ISystemInfoProvider {
    virtual ~ISystemInfoProvider() = default; // 虛擬解構子

    virtual double batteryLevel() const = 0;  // 取得電池電量（0.0~1.0）
    virtual double cpuUsage() const = 0;      // 取得 CPU 使用率（0.0~1.0）

    virtual void refresh() = 0;               // 手動刷新系統資訊資料
};