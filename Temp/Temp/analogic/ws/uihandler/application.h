/*!
* @file     application.h
* @author   Agiliad
* @brief    This file contains functions related to initializing the appcontroller.
* @date     Sept, 10 2016
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_APPLICATION_H
#define ANALOGIC_WS_UIHANDLER_APPLICATION_H
#include <iostream>
#include <QApplication>
#include <QDebug>
#include <analogic/ws/common.h>

/*!
 * \class   Application
 * \brief   This derived class of QApplication. This has been created to remove keyboard hang on login
 *          screen.
 */
class Application : public QApplication
{

    Q_OBJECT

public:
    /*!
    * @fn       Application
    * @param    argc-count of argument
    * @param    agv - arguments value
    * @return   None
    * @brief    Constructor responsible for initialization of class members, memory and resources.
    */
    Application(int& argc, char **argv):QApplication(argc,argv)
    {

    }

    /*!
    * @fn       notify
    * @param    QObject  - receiver object
    * @param    QEvent* - event
    * @return   None
    * @brief    Notify event
    */
    virtual bool 	notify(QObject *receiver, QEvent *e)
    {
        usleep(1);
        return QApplication::notify(receiver,e);
    }

protected:

    /*!
    * @fn       event
    * @param    QEvent - event
    * @return   None
    * @brief    event
    */
    virtual bool event(QEvent * evt)
    {
        usleep(1);
        return QApplication::event(evt);
    }
};
#endif // ANALOGIC_WS_UIHANDLER_APPLICATION_H
