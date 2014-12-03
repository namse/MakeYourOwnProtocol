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
	
	void SetPacketMissingRate(ChannelNumber channelNumber, int percentage){
		m_PacketMissingRateList[channelNumber] = (percentage >= 0 && percentage <= 100) ? percentage : 0;
	}

	int GetPacketMissingRate(ChannelNumber channelNumber)
	{
		if (m_PacketMissingRateList.find(channelNumber) == m_PacketMissingRateList.end())
			return 100;
		return m_PacketMissingRateList[channelNumber];
	}

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
	PacketMissingRateList m_PacketMissingRateList;

	IDCheckList m_IDCheckList;
};
