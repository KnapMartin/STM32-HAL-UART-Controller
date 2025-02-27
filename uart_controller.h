/*
 * uart_controller.h
 *
 *  Created on: Feb 27, 2025
 *      Author: knap-linux
 */

#ifndef INC_UART_CONTROLLER_H_
#define INC_UART_CONTROLLER_H_

#include <array>
#include <string>
#include <cstdint>
#include "main.h"

#define UART_CTL_VER "0.1.0"
#define UART_CTL_BUFFLEN 64
#define UART_CTL_MAX_TERMINATORS 16

class UartController
{
public:
	UartController(UART_HandleTypeDef *huart);
	virtual ~UartController();

	enum class State
	{
		None,
		Ok,
		Error,
		ErrorInit,
		ErrorTx,
		ErrorRx,
		ErrorTxIsr,
		ErrorRxIsr,
		ErrorTxTimeout,
		ErrorRxTimeout,
	};

	State init();
	State send(const std::string &data, const uint32_t timeout=0);
	std::string recieve(const uint32_t timeout=0);
	State updateInterruptTx(UART_HandleTypeDef *huart);
	State updateInterruptRx(UART_HandleTypeDef *huart);
	State registerTerminator(const char terminator);

private:
	UART_HandleTypeDef *m_huart;
	bool m_txReady;
	uint8_t m_rxChar;
	uint8_t m_rxCtr;
	uint8_t m_rxBuffer[UART_CTL_BUFFLEN];
	bool m_rxReady;
	std::array<char, UART_CTL_MAX_TERMINATORS> m_terminators;
};

#endif /* INC_UART_CONTROLLER_H_ */
