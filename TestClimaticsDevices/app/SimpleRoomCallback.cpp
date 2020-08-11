#include "SimpleRoomCallback.h"
#include "wliot/client/VirtualDeviceClient.h"

using namespace WLIOT;

SimpleRoomCallback::SimpleRoomCallback(SimpleRoomDevice *d)
{
	dev=d;
	connect(dev->room,&SimpleRoom::meas,this,&SimpleRoomCallback::onMeas);
	connect(dev->room,&SimpleRoom::stateChanged,this,&SimpleRoomCallback::onStateChanged);
}

bool SimpleRoomCallback::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	Q_UNUSED(retVal)
	if(cmd=="light"&&!args.isEmpty())
	{
		bool on=args[0]=="1";
		dev->room->setLight(on);
		return true;
	}
	return false;
}

WLIOT::ControlsGroup SimpleRoomCallback::mkControls()
{
	ControlsGroup grp;
	ControlsCommand cmd;
	cmd.buttonText="light";
	cmd.commandToExec="light";
	cmd.params.append(ControlsCommandParam("light",ControlsCommandParam::CHECKBOX));
	grp.elements.append(ControlsGroup::Element(cmd));
	return grp;
}

void SimpleRoomCallback::onMeas(const QByteArray &sens,const QByteArray &val)
{
	qDebug()<<"room "<<dev->room->roomName()<<" "<<sens<<":"<<val;
	dev->cli->sendVDevMeasurement(sens,QByteArrayList()<<val);
}

void SimpleRoomCallback::onStateChanged(const QByteArray &cmd,quint32 index,const QByteArray &val)
{
	qDebug()<<"room state "<<cmd<<":"<<index<<":"<<val;
	commandParamStateChanged(cmd,index,val);
}
