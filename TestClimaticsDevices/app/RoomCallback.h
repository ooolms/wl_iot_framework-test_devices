#ifndef ROOMCALLBACK_H
#define ROOMCALLBACK_H

#include "wliot/client/VirtualDeviceCallback.h"
#include "Room.h"
#include "wliot/devices/ControlsDefinition.h"
#include <QUuid>

struct RoomDevice
{
	QUuid uid;
	QByteArray name;
	WLIOTClient::VirtualDeviceClient *cli;
	Room *room;
};

class RoomCallback
	:public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit RoomCallback(RoomDevice *d);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;
	WLIOT::ControlsGroup mkControls();

protected:
	virtual void prepareState(WLIOT::DeviceState &st)override;

private slots:
	void onMeas(const QByteArray &sens,const QByteArray &val);
	void onStateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val);

private:
	RoomDevice *dev;
};

#endif // ROOMCALLBACK_H
