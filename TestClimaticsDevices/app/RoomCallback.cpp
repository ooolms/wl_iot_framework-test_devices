#include "RoomCallback.h"
#include "wliot/client/VirtualDeviceClient.h"
#include <QDebug>

using namespace WLIOT;
using namespace WLIOTClient;

RoomCallback::RoomCallback(RoomDevice *d)
{
	dev=d;
	connect(dev->room,&Room::meas,this,&RoomCallback::onMeas);
	connect(dev->room,&Room::stateChanged,this,&RoomCallback::onStateChanged);
}

bool RoomCallback::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	Q_UNUSED(retVal)
	qDebug()<<"room command: "<<cmd<<args;
	if(cmd=="heater"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setHeater(on);
		return true;
	}
	else if(cmd=="cond"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setCond(on);
		return true;
	}
	else if(cmd=="humid"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setHumid(on);
		return true;
	}
	else if(cmd=="light"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setLight(on);
		return true;
	}
	else if(cmd=="breezer"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setBreezer(on);
		return true;
	}
	else if(cmd=="uflamp"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setUFLamp(on);
		return true;
	}
	return false;
}

WLIOT::ControlsGroup RoomCallback::mkControls()
{
	ControlsGroup grp;
	ControlsCommand cmd;

	cmd.buttonText="heater";
	cmd.commandToExec="heater";
	cmd.params.append(ControlsCommandParam("heater",ControlsCommandParam::CHECKBOX));
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.buttonText="conditioner";
	cmd.commandToExec="cond";
	cmd.params[0].title="conditioner";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.buttonText="humidifier";
	cmd.commandToExec="humid";
	cmd.params[0].title="humidifier";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.buttonText="light";
	cmd.commandToExec="light";
	cmd.params[0].title="light";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.buttonText="breezer";
	cmd.commandToExec="breezer";
	cmd.params[0].title="breezer";
	grp.elements.append(ControlsGroup::Element(cmd));

	cmd.buttonText="UV lamp";
	cmd.commandToExec="uflamp";
	cmd.params[0].title="UV lamp";
	grp.elements.append(ControlsGroup::Element(cmd));
	return grp;
}

void RoomCallback::prepareState(DeviceState &st)
{
	st.commandParams.clear();
	st.additionalAttributes.clear();
	st.commandParams["heater"][0]="0";
	st.commandParams["cond"][0]="0";
	st.commandParams["humid"][0]="0";
	st.commandParams["light"][0]="0";
	st.commandParams["breezer"][0]="0";
	st.commandParams["uflamp"][0]="0";
}

void RoomCallback::onMeas(const QByteArray &sens,const QByteArray &val)
{
	qDebug()<<"room "<<dev->room->roomName()<<" "<<sens<<":"<<val;
	dev->cli->sendVDevMeasurement(sens,QByteArrayList()<<val);
}

void RoomCallback::onStateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val)
{
	qDebug()<<"room state "<<cmd<<":"<<index<<":"<<val;
	commandParamStateChanged(cmd,index,val);
}
