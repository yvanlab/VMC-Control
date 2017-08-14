

#ifndef BaseManager_h
#define BaseManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

class BaseManager
{
  public:
    BaseManager(unsigned char pinLed);
    void switchOn();
    void switchOff();
    void setStatus(unsigned long timeStampStatus, unsigned char codeStatus, String description) {
      m_timeStampStatus = timeStampStatus; m_changed = m_codeStatus != codeStatus;  m_codeStatus = codeStatus; m_description = description; }
    boolean  newLog() {return m_changed;}
    virtual String toString(){return "";}
    String log();
    virtual String getClassName(){ return "BaseManager";}
    unsigned long getLastUpdate(){ return m_timeStampStatus;}
  private:
    unsigned char m_pinLed;
    unsigned long m_timeStampStatus;
    unsigned char m_codeStatus;
    String m_description;
    boolean m_changed;
  };

#endif
