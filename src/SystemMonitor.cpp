#include "SystemMonitor.h"
#include <QProcessEnvironment>
#include <QRegularExpression>

namespace {
void configureProcess(QProcess &process)
{
    auto *timeout = new QTimer(&process);
    timeout->setSingleShot(true);
    timeout->setInterval(5000);
    QObject::connect(&process, &QProcess::started, timeout, [timeout] { timeout->start(); });
    QObject::connect(&process, &QProcess::finished, timeout, &QTimer::stop);
    QObject::connect(&process, &QProcess::errorOccurred, timeout, &QTimer::stop);
    QObject::connect(timeout, &QTimer::timeout, &process, &QProcess::kill);

    auto environment = QProcessEnvironment::systemEnvironment();
    environment.insert("LC_ALL", "C");
    process.setProcessEnvironment(environment);
}
}

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent)
{
    configureProcess(m_batteryProcess);
    configureProcess(m_cpuProcess);
    connect(&m_batteryProcess, &QProcess::finished, this, &SystemMonitor::readBatteryStats);
    connect(&m_cpuProcess, &QProcess::finished, this, &SystemMonitor::readCpuStats);
    connect(&m_updateTimer, &QTimer::timeout, this, &SystemMonitor::updateSystemStats);
    m_updateTimer.start(1000); // 每秒更新一次
}

SystemMonitor::~SystemMonitor()
{
    // Reap child processes before destroying QProcess instances during shutdown.
    for (auto *process : {&m_batteryProcess, &m_cpuProcess}) {
        if (process->state() != QProcess::NotRunning) {
            process->kill();
            process->waitForFinished(1000);
        }
    }
}

void SystemMonitor::updateSystemStats()
{
    if (m_batteryProcess.state() == QProcess::NotRunning)
        m_batteryProcess.start("pmset", {"-g", "batt"});
    if (m_cpuProcess.state() == QProcess::NotRunning)
        m_cpuProcess.start("top", {"-l", "1", "-n", "0"});
}

void SystemMonitor::readBatteryStats(int exitCode, QProcess::ExitStatus exitStatus)
{
    const QString output = QString::fromLocal8Bit(m_batteryProcess.readAllStandardOutput());
    if (exitStatus != QProcess::NormalExit || exitCode != 0)
        return;

    static const QRegularExpression rx("(\\d+)%");
    const auto match = rx.match(output);
    if (!match.hasMatch())
        return;

    const double level = qBound(0.0, match.captured(1).toDouble() / 100.0, 1.0);
    if (m_batteryLevel != level) {
        m_batteryLevel = level;
        emit batteryLevelChanged();
    }
}

void SystemMonitor::readCpuStats(int exitCode, QProcess::ExitStatus exitStatus)
{
    const QString output = QString::fromLocal8Bit(m_cpuProcess.readAllStandardOutput());
    if (exitStatus != QProcess::NormalExit || exitCode != 0)
        return;

    static const QRegularExpression rx(R"((\d+(?:\.\d+)?)% idle)");
    const auto match = rx.match(output);
    if (!match.hasMatch())
        return;

    const double usage = qBound(0.0, 1.0 - match.captured(1).toDouble() / 100.0, 1.0);
    if (m_cpuUsage != usage) {
        m_cpuUsage = usage;
        emit cpuUsageChanged();
    }
}
