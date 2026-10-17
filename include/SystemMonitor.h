#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class SystemMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)

public:
    explicit SystemMonitor(QObject *parent = nullptr);
    ~SystemMonitor() override;
    double batteryLevel() const { return m_batteryLevel; }
    double cpuUsage() const { return m_cpuUsage; }

signals:
    void batteryLevelChanged();
    void cpuUsageChanged();

private slots:
    void updateSystemStats();

private:
    void readBatteryStats(int exitCode, QProcess::ExitStatus exitStatus);
    void readCpuStats(int exitCode, QProcess::ExitStatus exitStatus);

    double m_batteryLevel = 0.0;
    double m_cpuUsage = 0.0;
    QTimer m_updateTimer;
    QProcess m_batteryProcess;
    QProcess m_cpuProcess;
};

#endif // SYSTEMMONITOR_H
