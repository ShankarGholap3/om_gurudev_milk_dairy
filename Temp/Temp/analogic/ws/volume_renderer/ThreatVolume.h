//------------------------------------------------------------------------------
// Name: ThreatVolume.h
// Purpose: Clas to maintain threat volume information
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_THREATVOLUME_H_
#define ANALOGIC_WS_VOLUME_RENDERER_THREATVOLUME_H_
//------------------------------------------------------------------------------

#include <QVector3D>

class ThreatVolume
{
public:
  ThreatVolume();

  void operator=(const ThreatVolume &V )
  {
    m_p0 = V.m_p0;
    m_p1 = V.m_p1;
  }

  explicit ThreatVolume(QVector3D pMin,
                        QVector3D pMax);
  virtual ~ThreatVolume();

  QVector3D p0();
  QVector3D p1();
  void setP0(double x, double y, double z)
  {
    m_p0.setX(x);
    m_p0.setY(y);
    m_p0.setZ(z);
  }

  void setP1(double x, double y, double z)
  {
    m_p1.setX(x);
    m_p1.setY(y);
    m_p1.setZ(z);
  }

   int getSize()
   {
     int w = m_p1.x() - m_p0.x();
     int h = m_p1.y() - m_p0.y();
     int l = m_p1.z() - m_p0.z();
     return (w*h*l);
   }

private:
  QVector3D m_p0;
  QVector3D m_p1;
};

//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_THREATVOLUME_H_
//------------------------------------------------------------------------------

