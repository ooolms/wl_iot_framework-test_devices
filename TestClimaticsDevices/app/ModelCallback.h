#ifndef MODELCALLBACK_H
#define MODELCALLBACK_H

#include <QObject>
#include "wliot/client/VirtualDeviceCallback.h"
#include "Outdoor.h"
#include "RoomCallback.h"
#include "SimpleRoomCallback.h"

class ModelCallback
	:public QObject
	,public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit ModelCallback(RoomDevice *r0,RoomDevice *r1,RoomDevice *r2,SimpleRoomDevice *wcR);
	void setOutdoor(Outdoor *o);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;

protected:
	virtual void prepareState(WLIOT::DeviceState &st)override;

private slots:
	void onMeas(const QByteArray &sens,const QByteArray &val);

private:
	Outdoor *outdoor;
	RoomDevice *room0,*room1,*room2;
	SimpleRoomDevice *wcRoom;
};

#endif // MODELCALLBACK_H
