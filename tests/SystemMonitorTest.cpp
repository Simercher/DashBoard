#include "SystemMonitor.h"

#include <QElapsedTimer>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class SystemMonitorTest : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        QVERIFY(m_tools.isValid());
        m_originalPath = qgetenv("PATH");
        qputenv("PATH", m_tools.path().toLocal8Bit() + ':' + m_originalPath);
        QVERIFY(writeTool("pmset", "sleep 0.3\nprintf 'InternalBattery 75%%; charging\\n'\n"));
        QVERIFY(writeTool("top", "sleep 0.3\nprintf 'CPU usage: 12.5%% user, 17.5%% sys, 70.0%% idle\\n'\n"));
    }

    void cleanup()
    {
        qputenv("PATH", m_originalPath);
    }

    void slowCommandsDoNotBlockOrOverlap()
    {
        QFile::remove(m_tools.filePath("pmset.calls"));
        QFile::remove(m_tools.filePath("top.calls"));
        SystemMonitor monitor;
        QSignalSpy batteryChanged(&monitor, &SystemMonitor::batteryLevelChanged);
        QSignalSpy cpuChanged(&monitor, &SystemMonitor::cpuUsageChanged);
        QElapsedTimer elapsed;
        elapsed.start();

        QVERIFY(QMetaObject::invokeMethod(&monitor, "updateSystemStats", Qt::DirectConnection));
        QVERIFY2(elapsed.elapsed() < 150, "System monitoring blocked the GUI event loop");
        QVERIFY(QMetaObject::invokeMethod(&monitor, "updateSystemStats", Qt::DirectConnection));

        bool heartbeatDelivered = false;
        QTimer::singleShot(50, [&] { heartbeatDelivered = true; });
        QTRY_VERIFY_WITH_TIMEOUT(heartbeatDelivered, 150);
        QCOMPARE(batteryChanged.count(), 0);
        QCOMPARE(cpuChanged.count(), 0);

        QTRY_COMPARE_WITH_TIMEOUT(batteryChanged.count(), 1, 1500);
        QTRY_COMPARE_WITH_TIMEOUT(cpuChanged.count(), 1, 1500);
        QCOMPARE(monitor.batteryLevel(), 0.75);
        QVERIFY(qAbs(monitor.cpuUsage() - 0.30) < 0.0001);
        QCOMPARE(callCount("pmset"), 1);
        QCOMPARE(callCount("top"), 1);
    }

    void failedCommandsDoNotPublishValues()
    {
        QVERIFY(writeTool("pmset", "printf 'InternalBattery 75%%; charging\\n'\nexit 1\n"));
        QVERIFY(writeTool("top", "printf 'CPU usage: 10%% user, 20%% sys, 70%% idle\\n'\nexit 1\n"));
        SystemMonitor monitor;
        QSignalSpy batteryChanged(&monitor, &SystemMonitor::batteryLevelChanged);
        QSignalSpy cpuChanged(&monitor, &SystemMonitor::cpuUsageChanged);
        QVERIFY(QMetaObject::invokeMethod(&monitor, "updateSystemStats", Qt::DirectConnection));
        QTest::qWait(150);
        QCOMPARE(batteryChanged.count(), 0);
        QCOMPARE(cpuChanged.count(), 0);
        QCOMPARE(monitor.batteryLevel(), 0.0);
        QCOMPARE(monitor.cpuUsage(), 0.0);
    }

private:
    bool writeTool(const QString &name, const QByteArray &body)
    {
        QFile tool(m_tools.filePath(name));
        if (!tool.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;
        const QByteArray script = "#!/bin/sh\nprintf 'call\\n' >> \"$0.calls\"\n" + body;
        if (tool.write(script) != script.size())
            return false;
        tool.close();
        return tool.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    }

    int callCount(const QString &name)
    {
        QFile calls(m_tools.filePath(name + ".calls"));
        if (!calls.open(QIODevice::ReadOnly))
            return 0;
        return calls.readAll().count('\n');
    }

    QTemporaryDir m_tools;
    QByteArray m_originalPath;
};

QTEST_GUILESS_MAIN(SystemMonitorTest)
#include "SystemMonitorTest.moc"
