#ifndef SIMPLEROOMCALLBACK_H
#define SIMPLEROOMCALLBACK_H

#include <wliot/client/VirtualDeviceCallback.h>
#include "SimpleRoom.h"
#include <QUuid>

struct SimpleRoomDevice
{
	QUuid uid;
	QByteArray name;
	WLIOTClient::VirtualDeviceClient *cli;
	SimpleRoom *room;
};

class SimpleRoomCallback
	:public QObject
	,public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit SimpleRoomCallback(SimpleRoomDevice *d);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;
	WLIOT::ControlsGroup mkControls();

protected:
	virtual void prepareState(WLIOT::DeviceState &st)override;

private slots:
	void onMeas(const QByteArray &sens,const QByteArray &val);
	void onStateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val);

private:
	SimpleRoomDevice *dev;
};

#endif // SIMPLEROOMCALLBACK_H
