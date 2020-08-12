#ifndef OUTDOOR_H
#define OUTDOOR_H

#include <QObject>

class Outdoor
	:public QObject
{
	Q_OBJECT
public:
	explicit Outdoor(QObject *parent=0);
	void onTimer();
	void setTemp(double t);
	void setHum(double h);
	void setLight(quint32 l);
	double temp();
	double hum();
	quint32 light();

signals:
	void meas(const QByteArray &sens,const QByteArray &val);

private:
	double mTemp,mHum;
	quint32 mLight;
};

#endif // OUTDOOR_H
