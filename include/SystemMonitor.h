#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>

class SystemMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)

public:
    explicit SystemMonitor(QObject *parent = nullptr);
    double batteryLevel() const { return m_batteryLevel; }
    double cpuUsage() const { return m_cpuUsage; }

signals:
    void batteryLevelChanged();
    void cpuUsageChanged();

private slots:
    void updateSystemStats();

private:
    double m_batteryLevel = 0.0;
    double m_cpuUsage = 0.0;
    QTimer m_updateTimer;
};

#endif // SYSTEMMONITOR_H