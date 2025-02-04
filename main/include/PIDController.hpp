//
// Created by jadjer on 03.02.25.
//

#pragma once

#include <algorithm>
#include <iostream>

template<class T>
class PIDController {
public:
  PIDController(T kP, T kI, T kD) : m_kP(kP),
                                    m_kI(kI),
                                    m_kD(kD),
                                    m_integral(0),
                                    m_setPoint(0),
                                    m_previousError(0) {}

public:
  void setPoint(T const point) {
    m_setPoint = point;
  }

public:
  T compute(T const measuredValue) {
    // Ошибка регулирования
    T error = m_setPoint - measuredValue;

    // Пропорциональная составляющая
    T proportional = m_kP * error;

    // Интегральная составляющая
    m_integral += error;
    T integral_term = m_kI * m_integral;

    // Дифференциальная составляющая
    T derivative = m_kD * (error - m_previousError);
    m_previousError = error;

    // Выходное значение ПИД-регулятора
    T output = proportional + integral_term + derivative;

    return output;
  }

  T compute(T const point, T const measuredValue) {
    setPoint(point);

    return compute(measuredValue);
  }

private:
  T m_kP;           // Пропорциональный коэффициент
  T m_kI;           // Интегральный коэффициент
  T m_kD;           // Дифференциальный коэффициент
  T m_integral;     // Накопленная ошибка
  T m_setPoint;     // Заданная скорость
  T m_previousError;// Предыдущая ошибка
};
