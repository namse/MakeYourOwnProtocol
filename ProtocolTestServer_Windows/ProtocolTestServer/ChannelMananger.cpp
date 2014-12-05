#include "stdafx.h"
#include "ChannelMananger.h"

ChannelMananger* ChannelMananger::instance = nullptr;
ChannelMananger::ChannelMananger()
{
}


ChannelMananger::~ChannelMananger()
{
}

bool ChannelMananger::Login(ChannelNumber channelNumber, ID id, struct sockaddr_in& clientaddr)
{
	if (IsRightID(channelNumber, id) == false)
		return false;

	auto ids = m_SessionList.equal_range(channelNumber);
	for (auto it = ids.first; it != ids.second; it++)
	{
		auto clientData = it->second;
		if (clientData.m_ID == id)
		{
			it->second.m_SocketAddress = clientaddr;
			return true;
		}
	}

	SessionInfo info;
	info.m_ID = id;
	info.m_SocketAddress = clientaddr;
	m_SessionList.insert(SessionList::value_type(channelNumber, info));
	return true;
}

bool ChannelMananger::IsRightID(ChannelNumber channelNumber, ID id)
{
	if (m_IDCheckList.find(id) != m_IDCheckList.end())
		return m_IDCheckList[id] == channelNumber;
	return false;
}

bool ChannelMananger::LoadIDCheckList()
{
	// TODO
	std::ifstream file("IDList.txt");
	if (file.is_open())
	{
		int n;
		file >> n;
		for (int i = 0; i < n; i++)
		{
			ChannelNumber cn;
			ID id;
			file >> cn >> id;
			m_IDCheckList.insert(IDCheckList::value_type(id, cn));
		}
	}

	return true;
}

void ChannelMananger::Init()
{
	LoadIDCheckList();
}

bool ChannelMananger::IsLogin(ChannelNumber channelNumber, ID id, struct sockaddr_in& clientaddr)
{
	auto ids = m_SessionList.equal_range(channelNumber);
	for (auto it = ids.first; it != ids.second; it++)
	{
		auto clientData = it->second;
		if (memcmp(&clientData.m_SocketAddress, &clientaddr, sizeof(struct sockaddr_in)) == 0
			&& clientData.m_ID == id)
			return true;
	}
	return false;
}