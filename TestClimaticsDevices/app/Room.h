#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <QTimer>
#include "wliot/devices/SensorDef.h"
#include "Outdoor.h"

class Room
	:public QObject
{
	Q_OBJECT
public:
	explicit Room(Outdoor *out,const QByteArray roomN,QObject *parent=nullptr);
	void onTimer();
	QList<WLIOT::SensorDef> mkSensors();
	QByteArray roomName();
	void onStart();

	//controls
	void setPeopleCount(quint32 c);
	void setHeater(bool on);
	void setHumid(bool on);
	void setCond(bool on);
	void setLight(bool on);
	void setBreezer(bool on);
	void setUFLamp(bool on);

signals:
	void meas(const QByteArray &sens,const QByteArray &val);
	void stateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val);

private slots:
	void onUpdatePeopleCountTimer();
	void onDoorCloseTimer();
	void updatePwrSens();

private:
	Outdoor *mOutdoor;
	QTimer peopleUpdateTimer,doorCloseTimer;
	QByteArray mRoomName;
	quint32 mPeopleCount,newPeopleCount;
	//sensors
	double tempSens,humSens;
	quint32 co2Sens;
	//controls
	bool heaterOn,condOn,humidOn,ufLampOn,lightOn,breezerOn;
};

#endif // ROOM_H
