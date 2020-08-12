#ifndef MODELCALLBACK_H
#define MODELCALLBACK_H

#include <QObject>
#include "wliot/client/VirtualDeviceCallback.h"
#include "Outdoor.h"
#include "RoomCallback.h"

class ModelCallback
	:public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit ModelCallback(RoomDevice *r0,RoomDevice *r1,RoomDevice *r2);
	void setOutdoor(Outdoor *o);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;

private slots:
	void onMeas(const QByteArray &sens,const QByteArray &val);

private:
	Outdoor *outdoor;
	RoomDevice *room0,*room1,*room2;
};

#endif // MODELCALLBACK_H
