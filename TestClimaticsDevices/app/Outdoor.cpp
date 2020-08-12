#include "Outdoor.h"
#include "climatics_defines.h"

Outdoor::Outdoor(QObject *parent)
	:QObject(parent)
{
	mTemp=22;
	mHum=30;
	mLight=800;
}

void Outdoor::onTimer()
{
	emit meas("ext_temp",QByteArray::number(mTemp,'g',6));
	emit meas("ext_hum",QByteArray::number(mHum,'g',6));
	emit meas("ext_light",QByteArray::number(mLight));
}

void Outdoor::setTemp(double t)
{
	mTemp=t;
	if(mTemp<globalMinTemp)
		mTemp=globalMinTemp;
	else if(mTemp>globalMaxTemp)
		mTemp=globalMaxTemp;
}

void Outdoor::setHum(double h)
{
	mHum=h;
	if(mHum<globalMinHum)
		mHum=globalMinHum;
	else if(mHum>globalMaxHum)
		mTemp=globalMaxHum;
}

void Outdoor::setLight(quint32 l)
{
	mLight=l;
	if(mLight<globalMinLight)
		mLight=globalMinLight;
	else if(mLight>globalMaxLight)
		mLight=globalMaxLight;
}

double Outdoor::temp()
{
	return mTemp;
}

double Outdoor::hum()
{
	return mHum;
}

quint32 Outdoor::light()
{
	return mLight;
}
