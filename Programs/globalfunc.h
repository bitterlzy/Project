#ifndef GLOBALFUNC_H
#define GLOBALFUNC_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <memory>
#include <string.h>
#include <mutex>
#include <unistd.h>

#include "ctimer.h"
#include "design.h"


void fairness_com()
{
    std::cout << "hello, world!" << std::endl;
    int vec[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int a,b,c,d;
    int count = 1;
    while (true)
    {
        if (count == 20)
        {
            break;
        }
        float num = 0;
        float sumUP = 0;
        float sumDOWN = 0;
        std::cout << "input a :" << std::endl;
        std::cin  >> a;
        std::cout << "input b :" << std::endl;
        std::cin  >> b;
        std::cout << "input c :" << std::endl;
        std::cin  >> c;
        std::cout << "input d :" << std::endl;
        std::cin  >> d;
        vec[a] += 1; vec[b] += 1; vec[c] += 1; vec[d] += 1;
         for (int i = 0; i < sizeof(vec) / sizeof(vec[0]); ++i)
        {
            std::cout << vec[i] << ", ";
        }
        std::cout << std::endl;
        for (int i = 0; i < 20; i++)
        {
            sumUP += vec[i];
            sumDOWN += vec[i] * vec[i];
        }
        num = (sumUP * sumUP) / (20 * sumDOWN);
        std::cout << "fair : " << num << std::endl;
        ++count;
    }
}

void timer_test() // 定时器测试
{
    std::cout << "hello, world!!!" << std::endl;
    CTimer timer;
    timer.StartTimer(1, [](){
        std::cout << "This is My CTimer count !!!" << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::seconds(10));
    timer.Expire();
}

void chainOfResponsibility()
{
    //创建一个请求
	PurchaseRequest* purchaseRequest = new PurchaseRequest(1, 50000, 1);

	//创建相关的审批人
	DepartmentApprover* department = new DepartmentApprover("张主任");
	CollegeApprover* college = new CollegeApprover("李院长");
	ViceSchoolMasterApprover* viceSchoolMaster = new ViceSchoolMasterApprover("王副校长");
	SchoolMasterApprover* schoolMaster = new SchoolMasterApprover("佟校长");

	//需要将各个审批级别的下一个人设置好(处理人构成一个环装就可以从任何一个人开始处理了)
	//否则必须从第一个处理人开始
	department->setApprover(college);
	college->setApprover(viceSchoolMaster);
	viceSchoolMaster->setApprover(schoolMaster);
	schoolMaster->setApprover(department);
	//开始处理请求
	viceSchoolMaster->processRequest(purchaseRequest);
}


void memento_test() // 备忘录模式测试
{
    Originator originator;
    Caretaker caretaker;
    originator.SetState("状态1, 攻击力: 100");

    caretaker.add(originator.SaveStateMemento());
    originator.SetState("状态2, 攻击力: 200");
    caretaker.add(originator.SaveStateMemento());
    std::cout << "当前的状态: " << originator.GetState() << std::endl;
    std::cout << "增益BUFF结束, 回到之前的状态1" << std::endl;
    originator.GetStateFromMemento(caretaker.get(0));
    std::cout << "结束后的状态为: " << originator.GetState() << std::endl;
}

void charTest()
{
    char charStr[64] = {'\0'}; memset(charStr, 0, sizeof(charStr));
    strcpy(charStr, "This is my charStr!!!");
    std::string myStr;
    for (int i = 0; i < strlen(charStr); ++i)
    {
        if (charStr[i] != ' ')
            myStr += charStr[i];
        else
            myStr += ',';
        
        std::cout << i << std::endl;
    }
    std::cout << myStr << " str.size() = "<< myStr.length() <<  std::endl;
}


#endif