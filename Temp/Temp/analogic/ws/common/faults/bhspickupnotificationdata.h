#ifndef BHSPICKUPNOTIFICATIONDATA_H
#define BHSPICKUPNOTIFICATIONDATA_H

#include <QObject>
#include <QString>
#include <QMap>

namespace analogic
{
namespace ws
{

struct BhsTrayPickupInfo
{
    QString description;
    QString useraction;
};

class BHSPickupNotificationData: public QObject
{
 Q_OBJECT
public:
	BHSPickupNotificationData();

	 static QMap <QString, BhsTrayPickupInfo>     m_bhsPickupInfo;

	 static void insertBhsPickupData();

	 static QMap<QString, BhsTrayPickupInfo>* getBhsPickupData();
};
}
}
#endif