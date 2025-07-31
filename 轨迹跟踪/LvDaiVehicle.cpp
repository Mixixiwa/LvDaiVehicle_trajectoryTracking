// LvDaiVehicle.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "serialPort.h"
#include <conio.h>
#include "VehicleSimulator.h"
#include <fstream>
//使用PID进行轨迹跟踪
#include "PIDController.h"
#include <vector>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include"path_heading_kappa.h"
#include "ProjectionMatcher.h"
#include "ErrorCalculator.h"

SERIALPORT serialPort;
SERIALPORT* pserial = &serialPort;
struct TrackerOutput {
    double v_left;
    double v_right;  // 可以加其它路径跟踪器的控制信号，如目标点坐标、角度等
};

//按下q程序退出
bool quitFlag = false;

DWORD WINAPI PortSend(LPVOID lpParameter)
{
    TrackerOutput* output = static_cast<TrackerOutput*>(lpParameter);
    pserial->StartThread();

    while (!quitFlag)
    {
        pserial->m_aSpeedCommand = output->v_left;
        pserial->m_bSpeedCommand = output->v_right;

        pserial->SendControlCommand();

        Sleep(1000);//调整串口发送的时间间隔（1s）
    }

    pserial->SendStopCommand();
    pserial->StopThread();
    cout << "发送线程已关闭" << endl;
    return 0;
}

//测试履带车模型
int main()     
{
    //串口号
    pserial->m_comNum = 1;
    memset(pserial->m_comBuf, 0, sizeof(pserial->m_comBuf));
    if (pserial->m_comNum < 10)
    {
        sprintf_s(pserial->m_comBuf, "COM%d", pserial->m_comNum);
    }
    else
    {
        sprintf_s(pserial->m_comBuf, "\\\\.\\COM%d", pserial->m_comNum);
    }

    if (!pserial->InitPort())
    {
        printf("Link Failed, Please Check the COM Number!=%s=\n", pserial->m_comBuf);
        return false;
    }
    else
    {
        printf("%s Opened!\n", pserial->m_comBuf);
    }

    //创建传递参数的指针变量
    TrackerOutput* trackerOutput = new TrackerOutput;
    trackerOutput->v_left = 0;
    trackerOutput->v_right = 0;
    HANDLE hSend = CreateThread(NULL, 0, &PortSend, trackerOutput, 0, NULL);//创建发送线程
    
    //设置车辆参数和PID参数
    
     VehicleSimulator sim(0.6); // 履带间距 b = 0.6m
     double v_desired = 0.5;  // 前进线速度
     PIDController heading_pid(2, 0.05, 0); // 角度 PID 控制器
     double dt = 0.1;
     const auto& state = sim.getState();//获取车辆状态
      
     std::ifstream file("../visualize_traj.py/Double_lane.csv");   //提取目标轨迹的文件
     std::ofstream file1("../visualize_traj.py/PIDtrajectory_output.csv");
     std::ofstream file2("../visualize_traj.py/path_output.csv");
     std::ofstream file3("../visualize_traj.py/err_output.csv");
     std::ofstream file4("../visualize_traj.py/path_heading_kappa_output.csv");
     std::ofstream file5("../visualize_traj.py/MatchPoint_index_heading_kappa_output.csv");
     std::ofstream file6("../visualize_traj.py/vl_vr_nl_nr_output.csv");
     //获取目标路径
     if (!file.is_open()) {
            std::cerr << "无法打开文件 Double_lane.csv" << std::endl;
            return 1;
        }

     std::vector<std::pair<double, double>> path;
     std::string line;

     while (std::getline(file, line)) 
     {
        std::stringstream ss(line);
        std::string x_str, y_str;

        if (std::getline(ss, x_str, ',') && std::getline(ss, y_str)) 
        {
            double x = std::stod(x_str);
            double y = std::stod(y_str);
            path.emplace_back(x, y);
            file2 << x << "," << y << "\n";
        }
     }

     //目标路径
    std::vector<double> path_x;
    std::vector<double> path_y;
    size_t N0 = path.size();
    path_x.resize(N0);
    path_y.resize(N0);

    for (size_t i = 0; i < N0; ++i) 
    {
        path_x[i] = path[i].first;
        path_y[i] = path[i].second;
    }

    //计算目标路径的航向角和曲率
    std::vector<double> heading, kappa;
    computePathHeadingAndKappa(path, heading, kappa);
    
    for (size_t i = 0; i < N0; ++i) {
        file4 << path_x[i] << "," << path_y[i] << "," << heading[i] << "," << kappa[i] << "\n";
    }

    //开始轨迹跟踪仿真，仿真时间5000*0.1=500（s）
   for (int i = 0; i < 5000; ++i) 
    {
        
        //车辆位置
        std::vector<double> x_set;
        std::vector<double> y_set;
        size_t N1 = 1;
        x_set.resize(N1);
        y_set.resize(N1);

        for (size_t i = 0; i < N1; ++i) {
            x_set[i] = state.x;
            y_set[i] = state.y;
        }

        //计算投影点的匹配点的编号，投影点点的坐标，航向角，曲率
        ProjectionMatcher matcher;
        ProjectionResult res = matcher.matchProjection(x_set, y_set, path_x, path_y, heading, kappa);

        int target_idx = res.match_point_index_set[0];
        
        if (target_idx >= path.size()) break;

        // 匹配点坐标,航向角，曲率
        double x_desire = path[target_idx].first;
        double y_desire = path[target_idx].second;
        double thetar_desire = heading[target_idx];
        double kappar_desire = kappa[target_idx];

        file5 << i * 0.1 << "," << target_idx << "," << x_desire << "," << thetar_desire << "," << kappar_desire << "\n";

        //预测车辆状态
        double ts = 0.1;
        VehicleState PredictedState = predictNextState(state, ts);

        //计算误差
        std::array<double, 4> err;  //err[0] = ed; err[1] = ed_dot； err[2] = ephi;  err[3] = ephi_dot;
        double es = 0.0;
        double s_dot = 0.0;
        computeErrors(
            PredictedState.x, PredictedState.y, PredictedState.phi,    // x, y, phi
            PredictedState.v_x, PredictedState.v_y, PredictedState.phi_dot,    // vx, vy, phi_dot
            x_desire, y_desire, thetar_desire,    // xr, yr, thetar
            kappar_desire,             // kappar
            err, es, s_dot
        );
        file3 << i * 0.1 << "," << err[0]<<","<<err[1]<<","<<err[2]<<","<<err[3] << "\n";


        // 用PID，通过横向误差计算角速度命令
        double omega_cmd = heading_pid.compute(err[0], dt);

        // 计算履带速度
        double vL = 0;
            vL = v_desired + 0.5 * sim.getTrackWidth() * omega_cmd;
            double vR = 0;
            vR = v_desired - 0.5 * sim.getTrackWidth() * omega_cmd;

        sim.step(vL, vR, dt);
        file1 << state.x << "," << state.y << "," << state.phi << "\n";

        trackerOutput->v_left = 60*vL/(2*3.14*0.1);
        trackerOutput->v_right = 60 * vR / (2 * 3.14 * 0.1);

        file6 << i * 0.1 << "," << vL << "," << vR << "," << trackerOutput->v_left << "," << trackerOutput->v_right << "\n";

	    int lifeSignal = 0; //心跳计时
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 'q') {
                quitFlag = true;
                break;
            }
        }

        Sleep(10);//控制仿真的时间

    }
    // 主循环结束，通知串口线程退出
    quitFlag = true;

    if (hSend != NULL)
    {
        WaitForSingleObject(hSend, INFINITE);
        CloseHandle(hSend);
        hSend = NULL;
    }
    cout << "程序退出！" << endl;
    Sleep(300);
    delete trackerOutput;
    file.close();
    file1.close();
    file2.close();
    file3.close();
    file4.close();
    file5.close();
    file6.close();
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


//W S A D按键
//bool fourKey[4] = {};

////DWORD WINAPI PortSend(LPVOID lpParameter)
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
//		/*pserial->SendAskSpeedNow();*/
//		Sleep(50);
//	}
//	//程序退出前，发送停车指令
//	pserial->SendStopCommand();
//	pserial->StopThread();
//	Sleep(10);
//	cout << "发送线程已关闭" << endl;
//	return 0;
//}
//
//int main()
//{
//	//串口号
//	pserial->m_comNum = 3;
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
