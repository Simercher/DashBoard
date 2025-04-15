#ifndef TIMEPROVIDER_H
#define TIMEPROVIDER_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

/**
 * 後台計時類別：使用 QElapsedTimer 計算經過毫秒數，
 * 並透過 QTimer 定期(約16ms)更新 elapsedMs。
 */
class TimeProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 elapsedMs READ elapsedMs NOTIFY elapsedMsChanged)
    Q_PROPERTY(QString currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(bool changingMode READ isChangingMode WRITE setChangingMode NOTIFY changingModeChanged)
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY elapsedMsChanged)

public:
    explicit TimeProvider(QObject *parent = nullptr);

    qint64 elapsedMs() const { return m_elapsedMs; }
    QString currentMode() const { return m_currentMode; }
    bool isChangingMode() const { return m_changingMode; }
    void setChangingMode(bool changing);
    QString formattedTime() const;

public slots:
    void start();
    void stop();
    void setCurrentMode(const QString &mode);
    void startModeChange(const QString &targetMode);

signals:
    void elapsedMsChanged();
    void currentModeChanged();
    void changingModeChanged();
    void modeChangeCompleted(const QString &newMode);

private slots:
    void updateElapsed();
    void completeModeChange();

private:
    QString formatTime(qint64 ms) const;
    
    QTimer m_updateTimer;
    QElapsedTimer m_timer;
    QTimer* m_modeChangeTimer;
    qint64 m_elapsedMs = 0;
    QString m_currentMode = "TeleOperated";
    bool m_changingMode = false;
    QString m_targetMode;
    QString m_cachedFormattedTime;
};

#endif // TIMEPROVIDER_H