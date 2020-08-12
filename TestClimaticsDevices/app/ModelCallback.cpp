#include "ModelCallback.h"
#include "wliot/client/VirtualDeviceClient.h"
#include <QDebug>

using namespace WLIOTClient;

ModelCallback::ModelCallback(Outdoor *out,RoomDevice *r0,RoomDevice *r1,RoomDevice *r2)
{
	outdoor=out;
	room0=r1;
	room1=r2;
	room2=r0;
	connect(out,&Outdoor::meas,this,&ModelCallback::onMeas);
}

bool ModelCallback::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	qDebug()<<"model command: "<<cmd<<args;
	if(cmd=="set_ext_temp"&&args.count()>0)
	{
		bool ok=false;
		double temp=args[0].toDouble(&ok);
		if(!ok)return false;
		outdoor->setTemp(temp);
		return true;
	}
	else if(cmd=="set_ext_hum"&&args.count()>0)
	{
		bool ok=false;
		double hum=args[0].toDouble(&ok);
		if(!ok)return false;
		outdoor->setHum(hum);
		return true;
	}
	else if(cmd=="set_ext_light"&&args.count()>0)
	{
		bool ok=false;
		quint32 light=args[0].toUInt(&ok);
		if(!ok)return false;
		outdoor->setLight(light);
		return true;
	}
	else if(cmd=="room0people"&&args.count()>0)
	{
		bool ok=false;
		quint32 cnt=args[0].toUInt(&ok);
		if(!ok)return false;
		room0->room->setPeopleCount(cnt);
		return true;
	}
	else if(cmd=="room1people"&&args.count()>0)
	{
		bool ok=false;
		quint32 cnt=args[0].toUInt(&ok);
		if(!ok)return false;
		room1->room->setPeopleCount(cnt);
		return true;
	}
	else if(cmd=="room2people"&&args.count()>0)
	{
		bool ok=false;
		quint32 cnt=args[0].toUInt(&ok);
		if(!ok)return false;
		room2->room->setPeopleCount(cnt);
		return true;
	}
	return false;
}

void ModelCallback::onMeas(const QByteArray &sens, const QByteArray &val)
{
	if(mDev)
		mDev->sendVDevMeasurement(sens,QByteArrayList()<<val);
}