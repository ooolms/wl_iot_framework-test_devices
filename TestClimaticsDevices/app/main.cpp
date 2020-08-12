#include <QCoreApplication>
#include <QTimer>
#include <math.h>
#include "wliot/client/ServerInstance.h"
#include "wliot/client/VirtualDeviceClient.h"
#include "wliot/client/VirtualDeviceCallback.h"
#include "wliot/storages/ISensorStorage.h"
#include "StdQFile.h"
#include "RoomCallback.h"
#include "SimpleRoomCallback.h"
#include "Outdoor.h"

using namespace WLIOT;
using namespace WLIOTClient;

void myMessageHandler(QtMsgType t,const QMessageLogContext &,const QString &s)
{
	QByteArray m=s.toUtf8();
	if(t==QtDebugMsg||t==QtInfoMsg)
	{
		StdQFile::stdoutFile()->write(m.constData(),m.size());
		StdQFile::stdoutFile()->write("\n",1);
		StdQFile::stdoutFile()->flush();
	}
	else
	{
		StdQFile::stderrFile()->write(m.constData(),m.size());
		StdQFile::stderrFile()->write("\n",1);
		StdQFile::stderrFile()->flush();
	}
}

struct ModelDevice
{
	QUuid uid;
	QByteArray name;
	VirtualDeviceClient *cli;
	Outdoor *outdoor;
};

QList<SensorDef> mkModelSensors()
{
	QList<SensorDef> retVal;
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"ext_temp"));
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"ext_hum"));
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"ext_light"));
	return retVal;
}

ControlsGroup mkModelControls()
{
	ControlsGroup grp;
	ControlsCommand cmd;

	cmd.buttonText="set";
	cmd.title="Set ext temp";
	cmd.forceBtn=true;
	cmd.commandToExec="set_ext_temp";
	cmd.params.append(ControlsCommandParam("temp",ControlsCommandParam::TEXT_EDIT));
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.commandToExec="set_ext_hum";
	cmd.title="Set ext hum";
	cmd.params[0].title="hum";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.commandToExec="set_ext_light";
	cmd.title="Set ext light";
	cmd.params[0].title="light";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.forceBtn=false;
	cmd.params.removeAt(0);

	cmd.commandToExec="room0people";
	cmd.title="room 1 people";
	cmd.params.append(ControlsCommandParam("count",ControlsCommandParam::SELECT));
	cmd.params[0].attributes["titles"]="0|1|2";
	cmd.params[0].attributes["values"]="0|1|2";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.commandToExec="room1people";
	cmd.title="room 2 people";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.commandToExec="room2people";
	cmd.title="room 3 people";
	grp.elements.append(ControlsGroup::Element(cmd));
	return grp;
}

Outdoor outdoor;
RoomDevice room0,room1,room2;
SimpleRoomDevice wcRoom;

QUuid modelDevId=QUuid("{ae0d946d-9a33-4c2b-95c4-59beacd2ed5f}");
QByteArray modelDevName=QByteArray("model");

class ModelCallback
	:public VirtualDeviceCallback
{
public:
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override
	{
		qDebug()<<"model command: "<<cmd<<args;
		if(cmd=="set_ext_temp"&&args.count()>0)
		{
			bool ok=false;
			double temp=args[0].toDouble(&ok);
			if(!ok)return false;
			outdoor.setTemp(temp);
			if(mDev)
				mDev->sendVDevMeasurement("ext_temp",QByteArrayList()<<args[0]);
			return true;
		}
		else if(cmd=="set_ext_hum"&&args.count()>0)
		{
			bool ok=false;
			double hum=args[0].toDouble(&ok);
			if(!ok)return false;
			outdoor.setHum(hum);
			if(mDev)
				mDev->sendVDevMeasurement("ext_hum",QByteArrayList()<<args[0]);
			return true;
		}
		else if(cmd=="set_ext_light"&&args.count()>0)
		{
			bool ok=false;
			quint32 light=args[0].toUInt(&ok);
			if(!ok)return false;
			outdoor.setLight(light);
			if(mDev)
				mDev->sendVDevMeasurement("ext_light",QByteArrayList()<<args[0]);
			return true;
		}
		else if(cmd=="room0people"&&args.count()>0)
		{
			bool ok=false;
			quint32 cnt=args[0].toUInt(&ok);
			if(!ok)return false;
			room0.room->setPeopleCount(cnt);
			return true;
		}
		else if(cmd=="room1people"&&args.count()>0)
		{
			bool ok=false;
			quint32 cnt=args[0].toUInt(&ok);
			if(!ok)return false;
			room1.room->setPeopleCount(cnt);
			return true;
		}
		else if(cmd=="room2people"&&args.count()>0)
		{
			bool ok=false;
			quint32 cnt=args[0].toUInt(&ok);
			if(!ok)return false;
			room2.room->setPeopleCount(cnt);
			return true;
		}
		return false;
	}
}modelCb;

void onTimer()
{
	room0.room->onTimer();
	room1.room->onTimer();
	room2.room->onTimer();
	wcRoom.room->onTimer();
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	qInstallMessageHandler(&myMessageHandler);

	wcRoom={QUuid("{00954b46-b561-44ce-b090-af636f063fea}"),QByteArray("room_wc"),0,new SimpleRoom("wc")};
	room0={QUuid("{671b0d33-f46a-4d6a-8945-777c51219156}"),QByteArray("room_1"),0,new Room(&outdoor,"1")};
	room1={QUuid("{970c5056-f467-4800-bef4-a81dcdb8215e}"),QByteArray("room_2"),0,new Room(&outdoor,"2")};
	room2={QUuid("{8208b075-956a-4699-bc13-8a10f099e813}"),QByteArray("room_3"),0,new Room(&outdoor,"3")};
	SimpleRoomCallback cbWcRoom(&wcRoom);
	RoomCallback cbRoom0(&room0);
	RoomCallback cbRoom1(&room1);
	RoomCallback cbRoom2(&room2);
	VirtualDeviceClient *modelCli;

	ServerInstance srv;
	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.connection()->authenticateLocalFromRoot("climatics"))
		return __LINE__;
	wcRoom.cli=srv.devices()->registerVirtualDevice(wcRoom.uid,wcRoom.name,
		wcRoom.room->mkSensors(),cbWcRoom.mkControls(),"",&cbWcRoom);
	if(!wcRoom.cli)return __LINE__;
	room0.cli=srv.devices()->registerVirtualDevice(room0.uid,room0.name,
		room0.room->mkSensors(),cbRoom0.mkControls(),"",&cbRoom0);
	if(!room0.cli)return __LINE__;
	room1.cli=srv.devices()->registerVirtualDevice(room1.uid,room1.name,
		room1.room->mkSensors(),cbRoom1.mkControls(),"",&cbRoom1);
	if(!room1.cli)return __LINE__;
	room2.cli=srv.devices()->registerVirtualDevice(room2.uid,room2.name,
		room2.room->mkSensors(),cbRoom2.mkControls(),"",&cbRoom2);
	if(!room2.cli)return __LINE__;
	modelCli=srv.devices()->registerVirtualDevice(modelDevId,modelDevName,
		mkModelSensors(),mkModelControls(),"",&modelCb);
	if(!modelCli)return __LINE__;

	QObject::connect(srv.connection(),&ServerConnection::disconnected,&app,&QCoreApplication::quit);

	wcRoom.room->onStart();
	room0.room->onStart();
	room1.room->onStart();
	room2.room->onStart();
	QTimer tmr;
	tmr.setInterval(10000);
	QObject::connect(&tmr,&QTimer::timeout,&onTimer);
	tmr.start();

	int r=app.exec();
	delete wcRoom.room;
	delete room0.room;
	delete room1.room;
	delete room2.room;
	return r;
}

