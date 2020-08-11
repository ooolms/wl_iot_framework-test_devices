#include "SimpleRoom.h"
#include <QDebug>

using namespace WLIOT;

SimpleRoom::SimpleRoom(const QByteArray roomSuff,QObject *parent)
	:QObject(parent)
{
	mRoomName=roomSuff;
	peopleCount=0;
	newPeopleCount=0;
	lightOn=false;
	peopleUpdateTimer.setInterval(2000);
	doorCloseTimer.setInterval(2000);
	peopleUpdateTimer.setSingleShot(true);
	doorCloseTimer.setSingleShot(true);
	connect(&peopleUpdateTimer,&QTimer::timeout,this,&SimpleRoom::onUpdatePeopleCountTimer);
	connect(&doorCloseTimer,&QTimer::timeout,this,&SimpleRoom::onDoorCloseTimer);
}

void SimpleRoom::setPeopleCount(int c)
{
	if(peopleCount==c)return;
	peopleUpdateTimer.stop();
	doorCloseTimer.stop();
	newPeopleCount=c;
	if(newPeopleCount>1)
		newPeopleCount=1;
	emit meas("door","1");
	peopleUpdateTimer.start();
}

void SimpleRoom::setLight(bool on)
{
//	if(lightOn==on)return;
	lightOn=on;
	emit meas("light_on",(lightOn?"1":"0"));
	emit stateChanged("light",0,(lightOn?"1":"0"));
}

QList<SensorDef> SimpleRoom::mkSensors()
{
	QList<SensorDef> r;
	r.append(SensorDef(SensorDef::Type(SensorDef::U32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"people"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"light"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"light_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"door"));
	return r;
}

void SimpleRoom::onTimer()
{
	int light=(lightOn?400:0);
	emit meas("light",QByteArray::number(light));
}

QByteArray SimpleRoom::roomName()
{
	return mRoomName;
}

void SimpleRoom::onStart()
{
	onTimer();
	emit meas("people","0");
	emit meas("light_on","0");
	emit meas("door","0");
}

void SimpleRoom::onUpdatePeopleCountTimer()
{
	peopleCount=newPeopleCount;
	emit meas("people",peopleCount==0?"0":"1");
	doorCloseTimer.start();
}

void SimpleRoom::onDoorCloseTimer()
{
	emit meas("door","0");
}
