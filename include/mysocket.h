#ifndef  __MYSOCKET_H__
#define  __MYSOCKET_H__


class CMySocket
{
public:
	CMySocket();
	~CMySocket();
	int		CreateSocket(unsigned short usPort=0);
	int		GetSocket();
	void	DestroySocket();
	int		Listen();
	int		Send(char *pSendPkt, size_t length);
	int		Receive(char *pRcvdPkt, size_t length);
	int		ConnectToServer(char *pIpAddr, unsigned short usPort);

private:
	int		m_nSock;
};


#endif	// end of __MYSOCKET_H__
