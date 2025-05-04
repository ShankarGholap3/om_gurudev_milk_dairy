/*!
* @file     reruntaskconfig.h
* @author   Agiliad
* @brief    This file contains interface having configuration for rerun task
* @date     Mar, 31 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/
#ifndef RERUNTASKCONFIG_H
#define RERUNTASKCONFIG_H

#include <QString>
#include <QObject>

//------------------------------------------------------------------------------
// begin namespace analogic_ws
//------------------------------------------------------------------------------

namespace analogic
{
namespace ws
{
/*!
 * \class   RerunTaskConfig
 * \brief   This class contains configuration for rerun task
 */

class RerunTaskConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bagInputFolder READ bagInputFolder WRITE setBagInputFolder)
    Q_PROPERTY(QString resultOutputFolder READ resultOutputFolder WRITE setResultOutputFolder)
    Q_PROPERTY(QString selectedBagListFile READ selectedBagListFile WRITE setSelectedBagListFile)
    Q_PROPERTY(QString editBagListFile READ editBagListFile WRITE setEditBagListFile)
    Q_PROPERTY(int algorithm READ getAlgorithm WRITE setAlgorithm)
    Q_PROPERTY(bool isrunc READ getIsRun WRITE setIsRun)
    Q_PROPERTY(RerunBagListOption bagListOption READ getBagListOption WRITE setBagListOption)
public:

    /**
     * @brief The rerun detection algorithm enum
     */

    enum RerunBagListOption
    {
        SELECT_BAG_LIST_TYPE=0,
        Existing_Bag_List,
        From_Input_Folder


    };


    Q_ENUM(RerunBagListOption);

    /*!
    * @fn       RerunTaskConfig
    * @param    None
    * @return   None
    * @brief    Constructor for class RerunTaskConfig.
    */
   explicit RerunTaskConfig(QObject *parent = 0);

    /*!
    * @fn       ~RerunTaskConfig
    * @param    None
    * @return   None
    * @brief    Destructor for class RerunTaskConfig.
    */
    ~RerunTaskConfig();

    /*!
    * @fn       RerunTaskConfig
    * @param    const RerunTaskConfig &
    * @return   None
    * @brief    Copy Constructor for class RerunTaskConfig.
    */
    RerunTaskConfig(const RerunTaskConfig &conf):QObject(nullptr)
    {
        m_bagInputFolder = conf.m_bagInputFolder;
        m_editBagListFile = conf.m_editBagListFile;
        m_selectedBagListFile = conf.m_selectedBagListFile;
        m_resultOutputFolder = conf.m_resultOutputFolder;
        m_algorithm = conf.m_algorithm;
        m_bagListOption = conf.m_bagListOption;
        isrun = conf.isrun;
    }

    RerunTaskConfig &operator= (const RerunTaskConfig &conf) {
      m_bagInputFolder = conf.m_bagInputFolder;
      m_editBagListFile = conf.m_editBagListFile;
      m_selectedBagListFile = conf.m_selectedBagListFile;
      m_resultOutputFolder = conf.m_resultOutputFolder;
      m_algorithm = conf.m_algorithm;
      m_bagListOption = conf.m_bagListOption;
      isrun = conf.isrun;
      return *this;
    }

    QString bagInputFolder() const
    {
        return m_bagInputFolder;
    }
    QString resultOutputFolder() const
    {
        return m_resultOutputFolder;
    }
    QString selectedBagListFile() const
    {
        return m_selectedBagListFile;
    }
    QString editBagListFile() const
    {
        return m_editBagListFile;
    }

    void setBagInputFolder(QString inputFolfer)
    {
        m_bagInputFolder = inputFolfer;
    }

    void setResultOutputFolder(QString inputFolfer)
    {
        m_resultOutputFolder = inputFolfer;
    }
    void setSelectedBagListFile(QString inputFolfer)
    {
        m_selectedBagListFile = inputFolfer;
    }
    void setEditBagListFile(QString inputFolfer)
    {
        m_editBagListFile = inputFolfer;
    }

    int getAlgorithm() const
    {
        return m_algorithm;
    }

    void setAlgorithm(int alg)
    {
        m_algorithm = alg;
    }
    RerunBagListOption getBagListOption() const
    {
        return m_bagListOption;
    }

    void setBagListOption(RerunBagListOption opt)
    {
        m_bagListOption = opt;
    }

    void setIsRun(bool fl)
    {
        isrun = fl;
    }
    bool getIsRun()
    {
        return isrun;
    }

private:
    QString m_bagInputFolder;
    QString m_resultOutputFolder;
    QString m_selectedBagListFile;
    QString m_editBagListFile;
    int m_algorithm;
    RerunBagListOption  m_bagListOption;
    bool isrun;

};

}  // end of namespace ws
}  // end of namespace analogic
//Q_DECLARE_METATYPE(analogic::ws::RerunTaskConfig)
#endif // RERUNTASKCONFIG_H
