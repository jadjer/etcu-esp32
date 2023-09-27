//
// Created by jadjer on 29.08.22.
//

#include "Controller.hpp"

constexpr uint32_t speedThreshold_InKilometersPerHour = 30;
constexpr uint32_t minimalRPM = 2500;
constexpr uint32_t maximalRPM = 6000;

Controller::Controller() :
    m_clutchIsEnabled(true), m_accelerationValue(0), m_revolutionPerMinute(0),
    m_speed_InKilometersPerHour(0), m_cruiseSpeed_InKilometersPerHour(0)
{
}

void Controller::registerChangeValueCallback(ControllerChangeValueCallbackFunction const& changeValueCallbackFunction)
{
    m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void Controller::setRPM(uint32_t revolutionPerMinute)
{
    m_revolutionPerMinute = revolutionPerMinute;

    process();
}

void Controller::setSpeed(uint32_t speedInKilometersPerHour)
{
    m_speed_InKilometersPerHour = speedInKilometersPerHour;

    process();
}

void Controller::setAcceleration(uint32_t accelerationValue)
{
    m_accelerationValue = accelerationValue;

    process();
}

void Controller::setClutch(bool clutchIsEnabled)
{
    m_clutchIsEnabled = clutchIsEnabled;

    process();
}

void Controller::enable()
{
    m_cruiseSpeed_InKilometersPerHour = m_speed_InKilometersPerHour;
}

void Controller::disable()
{
    m_cruiseSpeed_InKilometersPerHour = 0;

    process();
}

void Controller::process()
{
    auto acceleratorValue = m_accelerationValue;

    if (m_cruiseSpeed_InKilometersPerHour > 0)
    {
        if (m_speed_InKilometersPerHour < m_cruiseSpeed_InKilometersPerHour)
        {
            acceleratorValue += 10;
        }
        if (m_speed_InKilometersPerHour > m_cruiseSpeed_InKilometersPerHour)
        {
            acceleratorValue -= 10;
        }

        if (m_revolutionPerMinute < minimalRPM or m_revolutionPerMinute > maximalRPM or not m_clutchIsEnabled)
        {
            m_cruiseSpeed_InKilometersPerHour = 0;
        }
    }

    if (m_revolutionPerMinute > maximalRPM)
    {
//        m_minimalAccelerationValue = 0;
    }

    if (m_changeValueCallbackFunction)
    {
        m_changeValueCallbackFunction(acceleratorValue);
    }
}
