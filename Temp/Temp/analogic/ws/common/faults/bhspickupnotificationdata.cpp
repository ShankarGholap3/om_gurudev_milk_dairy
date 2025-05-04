#include "bhspickupnotificationdata.h"

namespace analogic
{
namespace ws
{

QMap <QString, BhsTrayPickupInfo>     BHSPickupNotificationData::m_bhsPickupInfo;

BHSPickupNotificationData::BHSPickupNotificationData(){}

void BHSPickupNotificationData::insertBhsPickupData()
{
	m_bhsPickupInfo.insert("UNEXPECTED_BAG_AT_DECISION_POINT",{tr("A bin or loose object at the decision point will be diverted to search."),tr("1) Loose object may require manual removal. \n2) Press the OK button below to acknowledge.")});
	m_bhsPickupInfo.insert("UNEXPECTED_BAG_AT_DIVERT_POINT",{tr("A bin or loose object at the decision point will be diverted to search."),tr("1) Loose object may require manual removal. \n2) Press the OK button below to acknowledge.")});
	m_bhsPickupInfo.insert("UNEXPECTED_BAG_AT_ACCEPT_LANE",{tr("A bin or loose object has been detected going to the clear lane and must be removed."),tr("1) Manually remove the bin or loose object on the diverter \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("LONG_BAG_AT_DECISION_POINT",{tr("The bin at the decision point is too long and must be removed."),tr("1) Remove bin or loose object at the decision point. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAD_BIN_AT_DECISION_POINT",{tr("An unprogrammed or duplicate bin at the decision point must be removed."),tr("1) Remove bin or loose object at the decision point and remove from service. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("REJECT_BAG_AT_DECISION_POINT",{tr("A suspect bin at the decision point needs to be removed."),tr("1) Remove bin or loose object at the decision point. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("UNLINKED_BAG_AT_DECISION_POINT",{tr("There is an unlinked bin being sent to search."),tr("1) If there is a loose object remove it from the diverter. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("LOOSE_ITEM_AT_DECISION_POINT",{tr("There is a loose item or unlinked bin being sent to search."),tr("1) If there is a loose object, remove it from the diverter. \n2) Press the OK button below to resume.")});
	m_bhsPickupInfo.insert("DUP_RFID_AT_DECISION_POINT",{tr("The bin at the decision point has the same RFID as another bin in the system and must be removed."),tr("1) Remove bin or loose object at the decision point. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("ZERO_RFID_AT_DECISION_POINT",{tr("The bin at the decision point has an unprogrammed RFID and must be removed."),tr("1) Remove bin or loose object at the decision point. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("HIGH_THREAT_AT_DECISION_POINT",{tr("The bin at the decision point has been marked as HIGH THREAT."),tr("1) Follow approved protocols for a High Threat bin. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_2",{tr("There is a jam on the conveyor before the scanner entrance."),tr("1) Clear the jam on the conveyor at the entrance to the scanner. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_SCANNER",{tr("There is a jam in the scanner."),tr("1) Clear the jam in the scanner. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_3",{tr("There is a jam on the conveyor past the scanner exit."),tr("1) Clear the jam on the conveyors at the exit outside of the scanner. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_3B",{tr("There is a jam on the conveyor past the scanner exit."),tr("1) Clear the jam on the conveyors at the exit outside of the scanner. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_4",{tr("There is a jam at the decision point."),tr("1) Clear the jam at the decision point. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_5",{tr("There is a jam at the diverter."),tr("1) Clear the jam at the diverter. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_6",{tr("There is a jam at the diverter."),tr("1) Clear the jam at the diverter. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_7",{tr("The clear lane is full."),tr("1) Remove a bin from the clear lane. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAG_JAM_AT_ZONE_8",{tr("The suspect lane is full."),tr("1) Remove a bin from the suspect lane. \n2) Press the OK button to resume.")});
	m_bhsPickupInfo.insert("BAD_RFID_AT_DECISION_POINT",{tr("No RFID Read. Bin or item will be sent for search."),tr("1) Pick up any loose items from the diverter. \n2) Press the OK button to resume.")});
}
QMap<QString, BhsTrayPickupInfo>* BHSPickupNotificationData::getBhsPickupData()
{
	return &m_bhsPickupInfo;
}
}
}
