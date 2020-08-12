#include "ModelCallback.h"
#include "wliot/client/VirtualDeviceClient.h"
#include <QDebug>

using namespace WLIOTClient;

ModelCallback::ModelCallback(RoomDevice *r0,RoomDevice *r1,RoomDevice *r2,SimpleRoomDevice *wcR)
{
	outdoor=0;
	room0=r0;
	room1=r1;
	room2=r2;
	wcRoom=wcR;
}

void ModelCallback::setOutdoor(Outdoor *o)
{
	outdoor=o;
	connect(outdoor,&Outdoor::meas,this,&ModelCallback::onMeas);
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
	else if(cmd=="wcroompeople"&&args.count()>0)
	{
		bool ok=false;
		quint32 cnt=args[0].toUInt(&ok);
		if(!ok)return false;
		wcRoom->room->setPeopleCount(cnt);
		return true;
	}
	return false;
}

void ModelCallback::onMeas(const QByteArray &sens,const QByteArray &val)
{
	qDebug()<<"model "<<sens<<":"<<val;
	if(mDev)
		mDev->sendVDevMeasurement(sens,QByteArrayList()<<val);
}
