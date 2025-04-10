#include "SystemMonitor.h"
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent)
{
    connect(&m_updateTimer, &QTimer::timeout, this, &SystemMonitor::updateSystemStats);
    m_updateTimer.start(1000); // 每秒更新一次
}

void SystemMonitor::updateSystemStats()
{
    // 獲取電池信息
    QProcess batteryInfo;
    batteryInfo.start("pmset", QStringList() << "-g" << "batt");
    batteryInfo.waitForFinished();
    QString output = batteryInfo.readAllStandardOutput();
    
    // 解析電池百分比
    QRegularExpression rx("(\\d+)%");
    QRegularExpressionMatch match = rx.match(output);
    if (match.hasMatch()) {
        m_batteryLevel = match.captured(1).toDouble() / 100.0;
        emit batteryLevelChanged();
    }

    // 獲取 CPU 使用率
    QProcess cpuInfo;
    cpuInfo.start("top", QStringList() << "-l" << "1" << "-n" << "0");
    cpuInfo.waitForFinished();
    QString cpuOutput = cpuInfo.readAllStandardOutput();
    
    // 解析 CPU 使用率
    QRegularExpression cpuRx("CPU usage: (\\d+\\.\\d+)% user");
    QRegularExpressionMatch cpuMatch = cpuRx.match(cpuOutput);
    if (cpuMatch.hasMatch()) {
        m_cpuUsage = cpuMatch.captured(1).toDouble() / 100.0;
        emit cpuUsageChanged();
    }
}