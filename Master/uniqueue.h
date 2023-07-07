#pragma once

#include <shared_mutex>
#include <queue>
#include <list>
#include <map>
#include <string>

template<typename OBJ>

class CUniQueue
{
public:
	//CUniQueue();
	//~CUniQueue();

	using Container = std::queue<OBJ>;
	CUniQueue() :is_run_(true) {};
	virtual ~CUniQueue() {};

	// 컨테이너가 비어 있는지 확인합니다.
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	bool Empty()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.empty();
	}

	// OBJ를 컨테이너에서 추출합니다. 							
	// 데이터가 없다면 condition_variable을 통해서 대기 합니다.							
	bool Pop(OBJ& obj) 
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		cv_.wait(ul, [this] {return (!con_.empty() || !is_run_); });

		if (!is_run_) { return false; }

		if (!con_.empty()) {
			obj = con_.front();
			con_.pop();
			return true;
		}
		return false;
	}

	// 데이터를 컨테이너에 넣습니다. 							
	// condition_variable 를 통해서 통지합니다.							
	void Push(const OBJ& obj) 
	{
		std::unique_lock<std::shared_mutex> sl(sm_);
		con_.push(obj);
		cv_.notify_one();
	}

	// 모든 대기 스레드에 통지를 전달합니다.							
	void NotifyAll()
	{
		cv_.notify_all();
	}

	// 컨테이너의 크기를 확인합니다. 							
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	size_t Size()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.size();
	}

	bool IsRun() 
	{ 
		return is_run_; 
	}
	void SetStop() 
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		is_run_ = false;
		NotifyAll();
	}

	// 컨테이너 OBJ 비우기
	size_t Clear()
	{
		std::unique_lock<std::shared_mutex> sl(sm_);

		int nPopSize = con_.size();
		while (!con_.empty()) {
			con_.pop();
		}
		return nPopSize;
	}

private:
	Container						con_;
	std::condition_variable_any		cv_;
	std::shared_mutex				sm_;
	bool							is_run_;
};

template<typename OBJ>

class CUniVector
{
public:

	using Container = std::vector<OBJ>;
	CUniVector() :is_run_(true) {};
	virtual ~CUniVector() {};

	// 컨테이너가 비어 있는지 확인합니다.
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	bool Empty()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.empty();
	}

	// OBJ를 컨테이너에서 추출합니다. 							
	// 데이터가 없다면 condition_variable을 통해서 대기 합니다.							
	

	bool Pop(OBJ& obj)
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		cv_.wait(ul, [this] {return (!con_.empty() || !is_run_); });

		if (!is_run_) { return false; }

		if (!con_.empty()) {
			obj = con_.front();
			con_.erase(con_.begin());
			//con_.clear();
			return true;
		}
		return false;
	}

	// 데이터를 컨테이너에 넣습니다. 							
	// condition_variable 를 통해서 통지합니다.							
	void Push(const OBJ& obj)
	{
		std::unique_lock<std::shared_mutex> sl(sm_);
		//con_.push(obj);
		con_.emplace_back(obj);
		cv_.notify_one();
	}

	void GetData(int nidx, OBJ& obj)
	{
		std::unique_lock<std::shared_mutex> sl(sm_);
		
		obj = con_.at(nidx);

		cv_.notify_one();
	}

	// 모든 대기 스레드에 통지를 전달합니다.							
	void NotifyAll()
	{
		cv_.notify_all();
	}

	// 컨테이너의 크기를 확인합니다. 							
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	size_t Size()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.size();
	}

	bool IsRun()
	{
		return is_run_;
	}
	void SetStop()
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		is_run_ = false;
		NotifyAll();
	}

	// 컨테이너 OBJ 비우기
	size_t Clear()
	{
		std::unique_lock<std::shared_mutex> sl(sm_);

		int nPopSize = con_.size();
		while (!con_.empty()) {
			//con_.pop();
			con_.clear();
		}
		return nPopSize;
	}

private:
	Container						con_;
	std::condition_variable_any		cv_;
	std::shared_mutex				sm_;
	bool							is_run_;
};

template<typename OBJ1, typename OBJ2>

class CUniMap
{
public:

	using Container = std::map<OBJ1, OBJ2>;
	CUniMap() :is_run_(true) {};
	virtual ~CUniMap() {};

	// 컨테이너가 비어 있는지 확인합니다.
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	bool Empty()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.empty();
	}

	// OBJ를 컨테이너에서 추출합니다. 							
	// 데이터가 없다면 condition_variable을 통해서 대기 합니다.							


	bool Pop(OBJ1& obj)
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		cv_.wait(ul, [this] {return (!con_.empty() || !is_run_); });

		if (!is_run_) { return false; }

		if (!con_.empty()) {
			obj = con_.front();
			con_.erase(con_.begin());
			return true;
		}
		return false;
	}

	// 데이터를 컨테이너에 넣습니다. 							
	// condition_variable 를 통해서 통지합니다.							
	void Push(const OBJ1& obj1, const OBJ2& obj2)
	{
		std::unique_lock<std::shared_mutex> sl(sm_);
		con_.insert({ obj1, obj2 });// emplace_back(obj);
		cv_.notify_one();
	}

	bool GetData(std::string* strRet, OBJ1& obj1)
	{
		std::unique_lock<std::shared_mutex> sl(sm_);

		bool bRet = false;

		if (con_.find(obj1) != con_.end()) {
			(*strRet) = con_[obj1];
			bRet = true;
		}
		else {
			bRet = false;
		}

		cv_.notify_one();

		return bRet;
	}

	// 모든 대기 스레드에 통지를 전달합니다.							
	void NotifyAll()
	{
		cv_.notify_all();
	}

	// 컨테이너의 크기를 확인합니다. 							
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	size_t Size()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.size();
	}

	bool IsRun()
	{
		return is_run_;
	}
	void SetStop()
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		is_run_ = false;
		NotifyAll();
	}

	// 컨테이너 OBJ 비우기
	size_t Clear()
	{
		std::unique_lock<std::shared_mutex> sl(sm_);

		int nPopSize = con_.size();
	
		con_.erase(con_.begin(), con_end());

		return nPopSize;
	}

private:
	Container						con_;
	std::condition_variable_any		cv_;
	std::shared_mutex				sm_;
	bool							is_run_;
};

