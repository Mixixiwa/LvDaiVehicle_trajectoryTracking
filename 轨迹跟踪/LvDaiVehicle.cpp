// LvDaiVehicle.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "serialPort.h"
#include <conio.h>
#include "VehicleSimulator.h"
#include <fstream>

//SERIALPORT serialPort;
//SERIALPORT* pserial = &serialPort;
//
////按下q程序退出
//bool quitFlag = false;
////W S A D按键
//bool fourKey[4] = {'W','S','A','D'};
//
//DWORD WINAPI PortSend(LPVOID lpParameter)
//{
//	pserial->StartThread();
//	while (!quitFlag)
//	{
//		//发送数据 
//		//w按键按下：前进
//		if (fourKey[0])
//		{
//			pserial->m_aSpeedCommand = 300;
//			pserial->m_bSpeedCommand = 300;
//			pserial->SendFrontCommand();
//			Sleep(50);
//		}
//		//s键按下
//		if (fourKey[1])
//		{
//			pserial->m_aSpeedCommand = -300;
//			pserial->m_bSpeedCommand = -300;
//			pserial->SendRetreatCommand();
//			Sleep(50);
//		}
//		//a键按下
//		if (fourKey[2])
//		{
//			pserial->m_aSpeedCommand = -300;
//			pserial->m_bSpeedCommand = 300;
//			pserial->SendLeftTurnCommand();
//			Sleep(50);
//		}
//		//d键按下
//		if (fourKey[3])
//		{
//			pserial->m_aSpeedCommand = 300;
//			pserial->m_bSpeedCommand = -300;
//			pserial->SendRightTurnCommand();
//			Sleep(50);
//		}
//		//循环发送当前转速问询指令
//		pserial->SendAskSpeedNow();
//		Sleep(50);
//	}
//	//程序退出前，发送停车指令
//	pserial->SendStopCommand();
//
//	pserial->StopThread();
//	Sleep(10);
//	cout << "发送线程已关闭" << endl;
//	return 0;
//}

//int main()
//{
//	//串口号
//	pserial->m_comNum = 2;
//	memset(pserial->m_comBuf, 0, sizeof(pserial->m_comBuf));
//	if (pserial->m_comNum < 10)
//	{
//		sprintf_s(pserial->m_comBuf, "COM%d", pserial->m_comNum);
//	}
//	else
//	{
//		sprintf_s(pserial->m_comBuf, "\\\\.\\COM%d", pserial->m_comNum);
//	}
//
//	if (!pserial->InitPort())
//	{
//		printf("Link Failed, Please Check the COM Number!=%s=\n", pserial->m_comBuf);
//		return false;
//	}
//	else
//	{
//		printf("%s Opened!\n", pserial->m_comBuf);
//	}
//	HANDLE hSend = CreateThread(NULL, 0, &PortSend, NULL, 0, NULL);//创建发送线程
//	int lifeSignal = 0; //心跳计时
//	while (!quitFlag)
//	{
//		if (_kbhit())
//		{
//			switch (_getch())
//			{
//			case 'q':
//				quitFlag = true;
//				break;
//			case 'w':
//				fourKey[0] = true;
//				break;
//			case 's':
//				fourKey[1] = true;
//				break;
//			case 'a':
//				fourKey[2] = true;
//				break;
//			case 'd':
//				fourKey[3] = true;
//				break;
//			default:
//				break;
//			}
//		}
//		else
//		{
//			memset(fourKey, 0, sizeof(fourKey));
//		}
//
//		Sleep(50);
//	}
//	if (hSend != NULL)
//	{
//		WaitForSingleObject(hSend, INFINITE);
//		CloseHandle(hSend);
//		hSend = NULL;
//	}
//	cout << "程序退出！" << endl;
//	Sleep(3000);
//	return 0;
//}


//int main()     //测试履带车模型
//{
//    VehicleSimulator sim(0.6); // 履带间距 b = 0.6m
//    std::ofstream fout("VehicleSimulator.csv");
//
//    double dt = 0.01;
//
//    for (int i = 0; i < 1000; ++i) {
//        double vL = 0.5; // m/s
//        double vR = 1.0; // m/s
//        sim.step(vL, vR, dt);
//
//        const auto& s = sim.getState();
//        fout << s.x << "," << s.y << "," << s.psi << "\n";
//    }
//
//    fout.close();
//    std::cout << "Simulation complete.\n";
//    return 0;
//}

//使用PID进行轨迹跟踪
#include "PIDController.h"
#include <vector>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include"path_heading_kappa.h"
#include "ProjectionMatcher.h"


#define M_PI 3.14159265358979323846


double normalizeAngle(double angle) {
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

// 计算最近路径点索引
int findClosestPointIndex(const VehicleState& state, const std::vector<std::pair<double, double>>& path) {
    int closest_idx = 0;
    double min_dist = 1e9;
    for (int i = 0; i < path.size(); ++i) {
        double dx = path[i].first - state.x;
        double dy = path[i].second - state.y;
        double dist = dx * dx + dy * dy;
        
        if (dist < min_dist) {
            min_dist = dist;
            closest_idx = i;
        }
    }
    return closest_idx;
}
int main()     //测试履带车模型
{
    //获取目标路径
    std::ifstream file("path.csv");
    if (!file.is_open()) {
        std::cerr << "无法打开文件 path.csv" << std::endl;
        return 1;
    }

    std::vector<std::pair<double, double>> path;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string x_str, y_str;

        if (std::getline(ss, x_str, ',') && std::getline(ss, y_str)) {
            double x = std::stod(x_str);
            double y = std::stod(y_str);
            path.emplace_back(x, y);
        }
    }
    file.close();

    std::vector<double> path_x;
    std::vector<double> path_y;
    size_t N = path.size();
    path_x.resize(N);
    path_y.resize(N);

    for (size_t i = 0; i < N; ++i) {
        path_x[i] = path[i].first;
        path_y[i] = path[i].second;
    }

    //计算目标路径的航向角和曲率
    std::vector<double> heading, kappa;
    computePathHeadingAndKappa(path, heading, kappa);

    //计算投影点的匹配点的编号，投影点点的坐标，航向角，曲率
    ProjectionMatcher matcher;

    ProjectionResult res = matcher.matchProjection(x_set, y_set, path_x, path_y, heading, kappa);

    VehicleSimulator sim(0.5); // 履带间距 b = 0.6m
    double v_desired = 0.5;  // 前进线速度
    PIDController heading_pid(1.2, 0, 0.1); // 角度 PID 控制器
    std::ofstream fout("../visualize_traj.py/PIDtrajectory_output.csv");

    double dt = 0.1;

    for (int i = 0; i < 9000; ++i) {
        const auto& state = sim.getState();
        int target_idx = findClosestPointIndex(state, path);
        

        if (target_idx >= path.size()) break;

        // 目标点坐标
        double tx = path[target_idx].first;
        double ty = path[target_idx].second;

        // 计算目标方向角
        double dx = tx - state.x;
        double dy = ty - state.y;
        double psi_des = std::atan2(dy, dx);
        double psi_err = normalizeAngle(psi_des - state.psi);

        // 计算角速度命令
        double omega_cmd = heading_pid.compute(psi_err, dt);

        // 计算履带速度
        double vL = v_desired - 0.5 * sim.getTrackWidth() * omega_cmd;
        double vR = v_desired + 0.5 * sim.getTrackWidth() * omega_cmd;

        sim.step(vL, vR, dt);
        fout << state.x << "," << state.y << "," << state.psi << "\n";
    }
    int ret = std::system("..\\visualize_traj.py\\visualize_traj.py.py");
    ; // 要确保 python 命令有效
    if (ret != 0) {
        std::cerr << "Failed to run Python script.\n";
    }

    std::cout << "Simulation complete.\n";
    return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
