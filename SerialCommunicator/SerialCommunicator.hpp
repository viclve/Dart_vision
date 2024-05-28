/*
 * @Description: serial encapsulation
 * @Version: 1.0
 * @Autor: dangwi
 * @Date: 2022-02-27
 * @LastEditors: Julian Lin
 * @LastEditTime: 2023-06-23 21:41:48
 */

#ifndef __SERIAL_COMMUNICATOR_HPP__
#define __SERIAL_COMMUNICATOR_HPP__

#include <stdint.h>
#include <glog/logging.h>
#include "serial/serial.h"
#include "params.hpp"

struct SerialPkg
{
	uint8_t frame[12];
	uint8_t size;

	// 自定义发送包
	// 发送自瞄角度
	SerialPkg(float predict_pitch_angle, float predict_yaw_angle, int16_t shoot_flag, uint8_t target_num_list, int16_t target_num)
	{
		frame[0] = 0xaa, frame[1] = 0xbb, frame[2] = 0xcc;
		int16_t pitch_angle = predict_pitch_angle;
		int16_t yaw_angle = predict_yaw_angle;
		frame[3] = (uint8_t)(pitch_angle >> 8);
		frame[4] = (uint8_t)(pitch_angle);
		frame[5] = (uint8_t)(yaw_angle >> 8);
		frame[6] = (uint8_t)(yaw_angle);
		frame[7] = (uint8_t)(target_num_list);
		frame[8] = (uint8_t)(shoot_flag);
		frame[9] = 0xFF;
		size = 10;
		// frame[7] = (uint8_t)(shoot_flag | target_color << 2  | target_num << 4);
		// frame[8] = 0xFF;
		// size = 9;
	}
};

class SerialCommunicator : public serial::Serial
{
private:
	bool enable;
	bool connected;
	std::vector<std::string> port_names;
	std::vector<int> header;

public:
	// 不上车调试时，设enable为false
	SerialCommunicator(bool _enable,
				 std::vector<std::string> _port_names = {"/dev/ttyUSB0", "/dev/ttytousb", "/dev/ttyACM0"},
				 std::vector<int> _header = {0x3f, 0x4f});
	~SerialCommunicator();

	void connect();

	// usage: send({...})
	// 隐式调用SerialPkg构造
	void send(const SerialPkg &pkg);

	int receive(uint8_t *buffer, size_t n);
};

#endif
