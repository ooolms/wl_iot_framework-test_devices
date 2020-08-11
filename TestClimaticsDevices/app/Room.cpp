#include "Room.h"
#include "climatics_defines.h"
#include <QDebug>

using namespace WLIOT;

Room::Room(Outdoor *out,const QByteArray roomN,QObject *parent)
	:QObject(parent)
{
	mRoomName=roomN;
	mOutdoor=out;
	tempSens=22;
	humSens=30;
	co2Sens=400;
	peopleCount=0;
	newPeopleCount=0;
	heaterOn=condOn=humidOn=ufLampOn=lightOn=breezerOn=false;
	peopleUpdateTimer.setInterval(2000);
	doorCloseTimer.setInterval(2000);
	peopleUpdateTimer.setSingleShot(true);
	doorCloseTimer.setSingleShot(true);
	connect(&peopleUpdateTimer,&QTimer::timeout,this,&Room::onUpdatePeopleCountTimer);
	connect(&doorCloseTimer,&QTimer::timeout,this,&Room::onDoorCloseTimer);
}

void Room::onTimer()
{
	//temp,hum
	double tempDelta=0,humDelta=0;
	if(heaterOn)
	{
		tempDelta+=0.3;
		humDelta-=0.6;
	}
	if(humidOn)
		humDelta+=2;
	if(condOn)
	{
		tempDelta-=0.2;
		humDelta-=1;
	}
	if(!heaterOn&&!humidOn&&!condOn)
	{
		if(tempSens<mOutdoor->temp())
			tempDelta+=0.1;
		else if(tempSens>mOutdoor->temp())
			tempDelta-=0.1;
		if(humSens<mOutdoor->hum())
			humDelta+=0.2;
		else if(humSens>mOutdoor->hum())
			humDelta-=0.2;
	}
	tempSens+=tempDelta;
	humSens+=humDelta;
	if(tempSens<globalMinTemp)
		tempSens=globalMinTemp;
	else if(tempSens>globalMaxTemp)
		tempSens=globalMaxTemp;
	if(humSens<globalMinHum)
		humSens=globalMinHum;
	else if(humSens>globalMaxHum)
		humSens=globalMaxHum;
	//light
	quint32 light=mOutdoor->light()+(lightOn?400:0);
	if(light>globalMaxLight)
		light=globalMaxLight;
	//co2
	quint32 co2Delta=50*peopleCount;
	if(breezerOn)
		co2Delta-=80;
	co2Sens+=co2Delta;
	if(co2Sens<globalMinCO2)
		co2Sens=globalMinCO2;
	else if(co2Sens>globalMaxCO2)
		co2Sens=globalMaxCO2;
	//output
	emit meas("temp",QByteArray::number(tempSens,'g',6));
	emit meas("hum",QByteArray::number(humSens,'g',6));
	emit meas("light",QByteArray::number(light));
	emit meas("co2",QByteArray::number(co2Sens));
}

QList<SensorDef> Room::mkSensors()
{
	QList<SensorDef> r;
	r.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"temp"));
	r.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"hum"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"co2"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"people"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"light"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"light_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"heater_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"humid_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"cond_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"breezer_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"uflamp_on"));
	r.append(SensorDef(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1),"door"));
	return r;
}

QByteArray Room::roomName()
{
	return mRoomName;
}

void Room::onStart()
{
	onTimer();
	emit meas("heater_on","0");
	emit meas("light_on","0");
	emit meas("humid_on","0");
	emit meas("cond_on","0");
	emit meas("breezer_on","0");
	emit meas("uflamp_on","0");
	emit meas("people","0");
	emit meas("door","0");
}

void Room::setPeopleCount(quint32 c)
{
	if(peopleCount==c)return;
	peopleUpdateTimer.stop();
	doorCloseTimer.stop();
	newPeopleCount=c;
	if(newPeopleCount>2)
		newPeopleCount=2;
	emit meas("door","1");
	peopleUpdateTimer.start();
}

void Room::setHeater(bool on)
{
//	if(heaterOn==on)return;
	heaterOn=on;
	emit meas("heater_on",(heaterOn?"1":"0"));
	emit stateChanged("heater",0,(heaterOn?"1":"0"));
	updatePwrSens();
}

void Room::setHumid(bool on)
{
//	if(humidOn==on)return;
	humidOn=on;
	emit meas("humid_on",(humidOn?"1":"0"));
	emit stateChanged("humid",0,(humidOn?"1":"0"));
	updatePwrSens();
}

void Room::setCond(bool on)
{
//	if(condOn==on)return;
	condOn=on;
	qDebug()<<"room "<<mRoomName<<" cond: "<<(condOn?"on":"off");
	emit meas("cond_on",(condOn?"1":"0"));
	emit stateChanged("cond",0,(condOn?"1":"0"));
	updatePwrSens();
}

void Room::setLight(bool on)
{
//	if(lightOn==on)return;
	lightOn=on;
	emit meas("light_on",(lightOn?"1":"0"));
	emit stateChanged("light",0,(lightOn?"1":"0"));
	updatePwrSens();
}

void Room::setBreezer(bool on)
{
//	if(breezerOn==on)return;
	breezerOn=on;
	qDebug()<<"room "<<mRoomName<<" breezer: "<<(breezerOn?"on":"off");
	emit meas("breezer_on",(breezerOn?"1":"0"));
	emit stateChanged("breezer",0,(breezerOn?"1":"0"));
	updatePwrSens();
}

void Room::setUFLamp(bool on)
{
//	if(ufLampOn==on)return;
	ufLampOn=on;
	emit meas("uflamp_on",(ufLampOn?"1":"0"));
	emit stateChanged("uflamp",0,(ufLampOn?"1":"0"));
	updatePwrSens();
}

void Room::onUpdatePeopleCountTimer()
{
	peopleCount=newPeopleCount;
	emit meas("people",QByteArray::number(peopleCount));
	doorCloseTimer.start();
}

void Room::onDoorCloseTimer()
{
	emit meas("door","0");
}

void Room::updatePwrSens()
{
	double pwr=0;
	if(heaterOn)
		pwr+=1500;
	if(condOn)
		pwr+=2100;
	if(humidOn)
		pwr+=200;
	if(ufLampOn)
		pwr+=50;
	if(lightOn)
		pwr+=100;
	emit meas("pwr",QByteArray::number(pwr,'g',6));
}
