#include "TimeProvider.h"
#include <QDebug> 
#include <QDateTime>
#include <mach/mach.h>
#include <sys/sysctl.h>


TimeProvider::TimeProvider(QObject *parent)
    : QObject(parent)
{
    m_updateTimer.setTimerType(Qt::PreciseTimer);    // 使用精確計時器
    m_updateTimer.setInterval(16);                    // 約 60 FPS
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
    
    // 更新時間顯示（限制更新頻率）
    if (newElapsed != m_elapsedMs) {
        qint64 formatInterval = currentTime - lastFormatTime;
        
        // 每100ms才更新一次格式化字符串
        if (formatInterval >= 16) {
            m_elapsedMs = newElapsed;
            QString newFormattedTime = formatTime(m_elapsedMs);
            
            // 只在時間字符串實際變化時才發送信號
            if (newFormattedTime != m_cachedFormattedTime) {
                m_cachedFormattedTime = newFormattedTime;
                emit elapsedMsChanged();
            }
            
            lastFormatTime = currentTime;
        } else {
            // 僅更新原始毫秒數
            m_elapsedMs = newElapsed;
        }
    }
}

void TimeProvider::setCurrentMode(const QString &mode)
{
    if (m_currentMode != mode) {  // Fix: use mode parameter instead of targetMode
        m_currentMode = mode;
        emit currentModeChanged();
    }
}

void TimeProvider::startModeChange(const QString &targetMode)
{
    if (m_currentMode != targetMode) {  // Fix: use targetMode parameter instead of mode
        m_changingMode = true;
        m_targetMode = targetMode;
        emit changingModeChanged();
        m_modeChangeTimer->start();
    }
}

void TimeProvider::completeModeChange()
{
    // Set the current mode to target mode
    setCurrentMode(m_targetMode);
    m_changingMode = false;
    emit changingModeChanged();
    emit modeChangeCompleted(m_currentMode);
    
    // Reset timer for next mode change
    m_modeChangeTimer->stop();
}

QString TimeProvider::formatTime(qint64 ms) const
{
    int minutes = ms / 60000;
    int seconds = (ms % 60000) / 1000;
    int tenths = (ms % 1000) / 100;
    
    return QString("%1:%2.%3")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'))
        .arg(tenths);
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
        
        // Only start timer when changing to true
        if (changing && !m_targetMode.isEmpty()) {
            m_modeChangeTimer->start();
        }
    }
}