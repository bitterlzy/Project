#ifndef DESIGN_H
#define DESIGN_H

#include <iostream>
#include <string>
#include <vector>

//////////////////////// 责任链模式 /////////////////////////////
class PurchaseRequest
{
public:
    PurchaseRequest(int type, float price, int ID)
     : m_type(type), m_price(price), m_ID(ID) {}

    inline const int getType() const { return m_type; }
    inline const float getPrice() const { return m_price; }
    inline const int getID() const {return m_ID;}
private:
    float m_price; 
    int m_type;
    int m_ID;
};

class Approver // baseclass
{
public:
    explicit Approver(const std::string& name) : m_name(name) {}

    inline void setApprover(Approver* const approver) {m_approver = approver;}

    // 处理审批请求的方法， 得到一个请求， 处理是子类完成
    virtual void processRequest(PurchaseRequest* PurchaseRequest) = 0;

protected:
    Approver* m_approver; // 下一个处理者
    std::string m_name; // 名字
};

class DepartmentApprover : public Approver
{
public:
    explicit DepartmentApprover(const std::string& name) : Approver(name){}

    void processRequest(PurchaseRequest *purchaseRequest) override 
    {
        if (purchaseRequest->getPrice() <= 5000)
            std::cout << "请求编号ID= " << purchaseRequest->getID() << "被" << m_name << "处理" << std::endl;
        else 
            m_approver->processRequest(purchaseRequest);
    }
};

class CollegeApprover :public Approver
{
public:
	explicit CollegeApprover(const std::string& name)
		: Approver(name)
	{
	}

	void processRequest(PurchaseRequest* purchaseRequest) override
	{
		if (purchaseRequest->getPrice() > 5000 && purchaseRequest->getPrice() <= 10000)
		{
			std::cout << "请求编号id=" << purchaseRequest->getID() << "被" << m_name << "处理" << std::endl;
		}
		else
		{
			m_approver->processRequest(purchaseRequest);
		}
	}
};
//副校长
class ViceSchoolMasterApprover :public Approver
{
public:
	explicit ViceSchoolMasterApprover(const std::string& name)
		: Approver(name)
	{
	}

	void processRequest(PurchaseRequest* purchaseRequest) override
	{
		if (purchaseRequest->getPrice() > 10000 && purchaseRequest->getPrice() <= 30000)
		{
			std::cout << "请求编号id=" << purchaseRequest->getID() << "被" << m_name << "处理" << std::endl;
		}
		else
		{
			m_approver->processRequest(purchaseRequest);
		}
	}
};
//校长
class SchoolMasterApprover :public Approver
{
public:
	explicit SchoolMasterApprover(const std::string& name)
		: Approver(name)
	{
	}

	void processRequest(PurchaseRequest* purchaseRequest) override
	{
		if (purchaseRequest->getPrice() > 30000)
		{
			std::cout << "请求编号id=" << purchaseRequest->getID() << "被" << m_name << "处理" << std::endl;
		}
		else
		{
			m_approver->processRequest(purchaseRequest);
		}
	}
};






































/////////////////////////// 备忘录模式 /////////////////////////
class Memento    // 备忘录（用于记录状态）
{
public:
    explicit Memento(const std::string& state) : m_state(state) {}
    ~Memento() {}

    inline std::string GetState() const { return m_state; }

private:
    std::string m_state;
};

class Caretaker // 备忘录管理类
{
public:
    Caretaker() {}
    ~Caretaker() {}

    void add(Memento memento)
    {
        mementoList.emplace_back(memento);
    }

    inline Memento get(int index) { return mementoList[index]; }
private:
    std::vector<Memento> mementoList;
};

class Originator // 行为者（）
{
public:
    Originator() {}
    ~Originator() {}

    inline std::string GetState() const { return m_state; }
    void SetState(const std::string& state) { m_state = state; }

    Memento SaveStateMemento() { return Memento(m_state); }
    inline void GetStateFromMemento(Memento memento) { m_state = memento.GetState(); }

private:
    std::string m_state;
};






#endif