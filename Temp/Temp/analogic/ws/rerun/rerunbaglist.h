/*!
* @file     rerunbaglist.h
* @author   Agiliad
* @brief    This file contains structure for rerun bag list
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef RERUNBAGLIST_H
#define RERUNBAGLIST_H

#include <analogic/ws/common/bagdata/baglisthandler.h>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------
namespace analogic
{
namespace ws
{

/*!
 * \class   RerunBagList
 * \brief   This class contains structure for rerun bag list
 */

class RerunBagList
{
public:
    /*!
    * @fn       RerunBagList
    * @param    None
    * @return   None
    * @brief    Constructor for class RerunBagList.
    */
    RerunBagList();
    /*!
    * @fn       ~RerunBagList
    * @param    None
    * @return   None
    * @brief    Destructor for class RerunBagList.
    */
    ~RerunBagList();

    /*!
    * @fn       getBagData
    * @param    boost::shared_ptr<BagData> bag
    * @param    const QString bagDataFilePath
    * @param    const QString outputFolderPath
    * @return   None
    * @brief    Fetch the bag data.
    */
    bool getBagData(boost::shared_ptr<BagData> bag, const QString bagDataFilePath, const QString outputFolderPath);
};
}  // end of namespace ws
}  // end of namespace analogic


#endif // RERUNBAGLIST_H
