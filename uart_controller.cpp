/*
 * uart_controller.cpp
 *
 *  Created on: Feb 27, 2025
 *      Author: knap-linux
 */

#include <uart_controller.h>

#include <algorithm>


UartController::UartController(UART_HandleTypeDef *huart)
	: m_huart{huart}
	, m_txReady{true}
	, m_rxChar{}
	, m_rxCtr{0}
	, m_rxBuffer{}
	, m_rxReady{false}
	, m_terminators{}
{

}

UartController::~UartController()
{

}

UartController::State UartController::init()
{
	if (HAL_UART_Receive_IT(m_huart, &m_rxChar, 1) != HAL_OK)
	{
		return State::ErrorInit;
	}

	return State::Ok;
}

UartController::State UartController::send(const std::string &data, const uint32_t timeout)
{
	uint32_t tmout = HAL_GetTick() + timeout;
	while (!m_txReady)
	{
		if (timeout)
		{
			if (HAL_GetTick() > tmout) return State::ErrorTxTimeout;
		}
	}

	if (HAL_UART_Transmit_IT(m_huart, (uint8_t*)data.c_str(), data.length()) != HAL_OK)
	{
		return State::ErrorTx;
	}

	m_txReady = false;

	return State::Ok;
}

std::string UartController::recieve(const uint32_t timeout)
{
	m_rxReady = false;
	uint32_t tmout = HAL_GetTick() + timeout;

	while (!m_rxReady)
	{
		if (timeout)
		{
			if (HAL_GetTick() > tmout) return "";
		}
	}

	return std::string{reinterpret_cast<char*>(m_rxBuffer)};
}

UartController::State UartController::updateInterruptTx(
		UART_HandleTypeDef *huart)
{
	if (huart->Instance == m_huart->Instance)
	{
		m_txReady = true;
	}

	return State::Ok;
}

UartController::State UartController::updateInterruptRx(
		UART_HandleTypeDef *huart)
{
    if (huart->Instance == m_huart->Instance)
    {
        m_rxBuffer[m_rxCtr] = m_rxChar;
        ++m_rxCtr;

        if (std::find(m_terminators.begin(), m_terminators.end(), m_rxChar) != m_terminators.end() || m_rxCtr >= UART_CTL_BUFFLEN - 1)
		{
			m_rxBuffer[m_rxCtr] = '\0';
			m_rxReady = true;
			m_rxCtr = 0;
		}

        if (HAL_UART_Receive_IT(m_huart, &m_rxChar, 1) != HAL_OK)
        {
            return State::ErrorRxIsr;
        }
    }

    return State::Ok;
}

UartController::State UartController::registerTerminator(const char terminator)
{
    for (char &t : m_terminators)
    {
        if (t == '\0')
        {
            t = terminator;
            return State::Ok;
        }
    }
    return State::Error;
}
