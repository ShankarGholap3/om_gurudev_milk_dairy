/*!
* @file     VREnums.h
* @author   Agiliad
* @brief    Common include for ENUMS used across Workatation and Volumrenderer interface
* @date     Sep, 26 2016
*
(c) Copyright 2018 Analogic Corporation. All Rights Reserved
*/
//-------------------------------------------------------------------------------
// File: VREnums.h
// Purpose: Common include for ENUMS used across Workatation and Volumrenderer interface
// Copyright 2018 Analogic Corp.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <QtGlobal>
#include <qqml.h>

/*!
 * \class   VREnums
 * \brief   This class contains ENUMS used across Workatation and Volumrenderer interface
 *
 */
class SSLErrorEnums: public QObject
{
  Q_OBJECT

public:
  /*!
  * @fn       VREnums();
  * @param    QObject* - parent
  * @return   None
  * @brief    Constructor responsible for initialization of class members, memory and resources.
  */
  explicit SSLErrorEnums(QObject *parent = NULL):QObject(parent) {}


  /**
 * Enumerat the ways the Volume viewer can view the bag data
 *
 */
  enum SSLErrorEnum
  {
    NoError,
         UnableToGetIssuerCertificate,
         UnableToDecryptCertificateSignature,
         UnableToDecodeIssuerPublicKey,
         CertificateSignatureFailed,
         CertificateNotYetValid,
         CertificateExpired,
         InvalidNotBeforeField,
         InvalidNotAfterField,
         SelfSignedCertificate,
         SelfSignedCertificateInChain,
         UnableToGetLocalIssuerCertificate,
         UnableToVerifyFirstCertificate,
         CertificateRevoked,
         InvalidCaCertificate,
         PathLengthExceeded,
         InvalidPurpose,
         CertificateUntrusted,
         CertificateRejected,
         SubjectIssuerMismatch, // hostname mismatch?
         AuthorityIssuerSerialNumberMismatch,
         NoPeerCertificate,
         HostNameMismatch,
         NoSslSupport,
         CertificateBlacklisted,
         UnspecifiedError = -1
  };


  Q_ENUM(SSLErrorEnum);

  static void declareQML()
  {
    qmlRegisterType<SSLErrorEnums>("SSLErrorEnums", 1, 0, "SSLErrorEnums");
  }
};

Q_DECLARE_METATYPE(SSLErrorEnums::SSLErrorEnum)


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
