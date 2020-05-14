#include <QCoreApplication>
#include <QTimer>
#include <math.h>
#include "IotServer.h"
#include "IotServerDeviceBackend.h"
#include "IotServerVirtualDeviceClient.h"
#include "IotServerVirtualDeviceCallback.h"
#include "IotServerStorage.h"
#include "StdQFile.h"

void myMessageHandler(QtMsgType t,const QMessageLogContext &,const QString &s)
{
	QByteArray m=s.toUtf8();
	if(t==QtDebugMsg||t==QtInfoMsg)
	{
		StdQFile::stdout()->write(m.constData(),m.size());
		StdQFile::stdout()->write("\n",1);
		StdQFile::stdout()->flush();
	}
	else
	{
		StdQFile::stderr()->write(m.constData(),m.size());
		StdQFile::stderr()->write("\n",1);
		StdQFile::stderr()->flush();
	}
}

const QUuid meteoMeasId=QUuid("{65bfaa06-07a3-4adc-8266-8f1773625c19}");
const QByteArray meteoMeasName="meteo_meas";
const QUuid meteoCtlId=QUuid("{7fe697a1-3e81-46df-85c1-4d8cd2f1e346}");
const QByteArray meteoCtlName="meteo_ctl";

IotServerVirtualDeviceClient *meteoMeas=0;
IotServerVirtualDeviceClient *meteoCtl=0;

QList<SensorDef> mkSensors()
{
	QList<SensorDef> retVal;
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"temp"));
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),"hum"));
	return retVal;
}

ControlsGroup mkControls()
{
	ControlsGroup grp;
	CommandControl cmd;
	cmd.buttonText="heater";
	cmd.command="heat";
	cmd.title="heater";
	cmd.params.append(ControlParam("on",ControlParam::CHECKBOX));
	grp.elements.append(ControlsGroup::Element(cmd));
	cmd.buttonText="conditioner";
	cmd.command="cond";
	cmd.title="conditioner";
	grp.elements.append(ControlsGroup::Element(cmd));
	cmd.buttonText="humidifier";
	cmd.command="hum";
	cmd.title="humidifier";
	grp.elements.append(ControlsGroup::Element(cmd));
	return grp;
}

bool heaterOn=false;
bool conditionerOn=false;
bool humidifierOn=false;
double temp=20;
double hum=25;

class CtlCallback
	:public IotServerVirtualDeviceCallback
{
public:
	explicit CtlCallback(IotServerVirtualDeviceClient *dev)
		:IotServerVirtualDeviceCallback(dev){}
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override
	{
		Q_UNUSED(retVal)
		if(cmd=="heat"&&!args.isEmpty())
		{
			bool on=args[0]=="1";
			if(heaterOn!=on)
			{
				heaterOn=on;
				qDebug()<<"heat: "<<(heaterOn?"on":"off");
				commandParamStateChanged("heat",0,(heaterOn?"1":"0"));
			}
			return true;
		}
		else if(cmd=="cond"&&!args.isEmpty())
		{
			bool on=args[0]=="1";
			if(conditionerOn!=on)
			{
				conditionerOn=on;
				qDebug()<<"conditioner: "<<(conditionerOn?"on":"off");
				commandParamStateChanged("cond",0,(conditionerOn?"1":"0"));
			}
			return true;
		}
		else if(cmd=="hum"&&!args.isEmpty())
		{
			bool on=args[0]=="1";
			if(humidifierOn!=on)
			{
				humidifierOn=on;
				qDebug()<<"humidifier: "<<(humidifierOn?"on":"off");
				commandParamStateChanged("hum",0,(humidifierOn?"1":"0"));
			}
			return true;
		}
		return false;
	}
};

void onTimer()
{
	double tempDelta=0,humDelta=0;
	if(heaterOn)
	{
		tempDelta+=0.3;
		humDelta-=0.6;
	}
	if(humidifierOn)
		humDelta+=2;
	if(conditionerOn)
	{
		tempDelta-=0.2;
		humDelta-=1;
	}
	if(!heaterOn&&!humidifierOn&&!conditionerOn)
	{
		if(temp<19)
			tempDelta+=0.1;
		else if(temp>21)
			tempDelta-=0.1;
		if(hum<24)
			hum+=0.2;
		else if(hum>26)
			hum-=0.2;
	}
	temp+=tempDelta;
	hum+=humDelta;
	if(temp<10)temp=10;
	if(temp>40)temp=40;
	if(hum<5)hum=5;
	if(hum>95)hum=95;
	qDebug()<<"temp: "<<temp;
	qDebug()<<"hum: "<<hum;
	meteoMeas->sendVDevMeasurement("temp",QByteArrayList()<<QByteArray::number(temp,'g',6));
	meteoMeas->sendVDevMeasurement("hum",QByteArrayList()<<QByteArray::number(hum,'g',6));
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	qInstallMessageHandler(&myMessageHandler);
	IotServer srv;
	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.devices()->registerVirtualDevice(meteoMeasId,meteoMeasName,mkSensors(),ControlsGroup()))
		return __LINE__;
	if(!srv.devices()->registerVirtualDevice(meteoCtlId,meteoCtlName,QList<SensorDef>(),mkControls()))
		return __LINE__;

	meteoMeas=srv.devices()->registeredVDev(meteoMeasId);
	if(!meteoMeas)return __LINE__;
	meteoCtl=srv.devices()->registeredVDev(meteoCtlId);
	if(!meteoCtl)return __LINE__;
	meteoCtl->setDevEventsCallback(new CtlCallback(meteoCtl));

	QObject::connect(srv.connection(),&IotServerConnection::disconnected,&app,&QCoreApplication::quit);

	QTimer tmr;
	tmr.setInterval(5000);
	QObject::connect(&tmr,&QTimer::timeout,&onTimer);
	tmr.start();

	return app.exec();
}

