#ifndef SIMPLEROOM_H
#define SIMPLEROOM_H

#include <QObject>
#include <QTimer>
#include "wliot/devices/SensorDef.h"
#include "wliot/devices/ControlsDefinition.h"
#include "Outdoor.h"

class SimpleRoom
	:public QObject
{
	Q_OBJECT
public:
	explicit SimpleRoom(const QByteArray roomSuff,QObject *parent=nullptr);
	void setPeopleCount(int c);
	void setLight(bool on);
	QList<WLIOT::SensorDef> mkSensors();
	void onTimer();
	QByteArray roomName();
	void onStart();

signals:
	void meas(const QByteArray &sens,const QByteArray &val);
	void stateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val);

private slots:
	void onUpdatePeopleCountTimer();
	void onDoorCloseTimer();

private:
	Outdoor *mOutdoor;
	QTimer peopleUpdateTimer,doorCloseTimer;
	QByteArray mRoomName;
	int peopleCount,newPeopleCount;
	bool lightOn;
};

#endif // SIMPLEROOM_H
