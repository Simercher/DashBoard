#include "TimeProvider.h"
#include <QDebug> 
#include <QDateTime>
#include <mach/mach.h>
#include <sys/sysctl.h>

TimeProvider::TimeProvider(QObject *parent)
    : QObject(parent)
{
    // 設置更新計時器 (60 FPS)
    m_updateTimer.setTimerType(Qt::PreciseTimer);
    m_updateTimer.setInterval(33);
    connect(&m_updateTimer, &QTimer::timeout, this, &TimeProvider::updateElapsed);
    
    // 設置模式切換計時器
    m_modeChangeTimer = new QTimer(this);
    m_modeChangeTimer->setInterval(200);
    m_modeChangeTimer->setSingleShot(true);
    connect(m_modeChangeTimer, &QTimer::timeout, this, &TimeProvider::completeModeChange);
}

void TimeProvider::start()
{
    m_timer.start();
    m_updateTimer.start();
}

void TimeProvider::stop()
{
    m_updateTimer.stop();
}

void TimeProvider::updateElapsed()
{
    static qint64 lastFormatTime = 0;
    qint64 newElapsed = m_timer.elapsed();
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    if (newElapsed != m_elapsedMs) {
        m_elapsedMs = newElapsed;
        
        // 減少字串格式化頻率，只在實際需要時更新
        if (currentTime - lastFormatTime >= 50) {  // 增加更新間隔到50ms
            QString newFormattedTime = formatTime(m_elapsedMs);
            
            if (newFormattedTime != m_cachedFormattedTime) {
                m_cachedFormattedTime = newFormattedTime;
                emit elapsedMsChanged();
            }
            
            lastFormatTime = currentTime;
        }
    }
}

void TimeProvider::setCurrentMode(const QString &mode)
{
    if (m_currentMode != mode) {
        m_currentMode = mode;
        emit currentModeChanged();
    }
}

void TimeProvider::startModeChange(const QString &targetMode)
{
    if (m_currentMode != targetMode) {
        m_changingMode = true;
        m_targetMode = targetMode;
        emit changingModeChanged();
        m_modeChangeTimer->start();
    }
}

void TimeProvider::completeModeChange()
{
    setCurrentMode(m_targetMode);
    m_changingMode = false;
    emit changingModeChanged();
    emit modeChangeCompleted(m_currentMode);
    m_modeChangeTimer->stop();
}

QString TimeProvider::formatTime(qint64 ms) const
{
    int minutes = ms / 60000;
    int seconds = (ms % 60000) / 1000;
    int tenths = (ms % 1000) / 100;
    
    // 使用預分配空間的字串建構
    QString result(8, Qt::Uninitialized);
    result[0] = QChar('0' + minutes % 10);
    result[1] = ':';
    result[2] = QChar('0' + seconds / 10);
    result[3] = QChar('0' + seconds % 10);
    result[4] = '.';
    result[5] = QChar('0' + tenths);

    return result;
}

QString TimeProvider::formattedTime() const
{
    return m_cachedFormattedTime;
}

void TimeProvider::setChangingMode(bool changing)
{
    if (m_changingMode != changing) {
        m_changingMode = changing;
        emit changingModeChanged();
        
        if (changing && !m_targetMode.isEmpty()) {
            m_modeChangeTimer->start();
        }
    }
}