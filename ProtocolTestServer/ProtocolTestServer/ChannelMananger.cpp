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

	if (IsLogin(channelNumber, id, clientaddr) == true)
		return true;

	SessionInfo info;
	info.m_ID = id;
	info.m_SocketAddress = clientaddr;

	m_SessionList.insert(SessionList::value_type(channelNumber, info));
	return true;
}

bool ChannelMananger::IsRightID(ChannelNumber channelNumber, ID id)
{
	if (m_IDCheckList.find(channelNumber) != m_IDCheckList.end())
		return m_IDCheckList[channelNumber] == id;
	return false;
}

bool ChannelMananger::LoadIDCheckList()
{
	// TODO

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
			return false;
	}
	return true;
}