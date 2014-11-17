#pragma once
class ChannelMananger
{
public:
	static ChannelMananger* GetInstance(){
		if (instance == nullptr)
			instance = new ChannelMananger();
		return instance;
	}
	static void FreeInstance(){
		if (instance != nullptr)
			delete instance;
		instance = nullptr;
	}

	bool Login(ChannelNumber channelNumber, ID id, struct sockaddr_in& clientaddr);
	void Init();

	bool IsRightID(ChannelNumber channelNumber, ID id);
	bool IsLogin(ChannelNumber channelNumber, ID id, struct sockaddr_in& clientaddr);

	std::pair<SessionList::iterator, SessionList::iterator> GetChannelUsers(ChannelNumber channelNumber){
		return m_SessionList.equal_range(channelNumber);
	}

private:
	ChannelMananger();
	~ChannelMananger();
	bool LoadIDCheckList();

private:
	static ChannelMananger* instance;

	SessionList m_SessionList;

	IDCheckList m_IDCheckList;
};
