//*****************************************************************************
/*!
  \file divelog.h
  \brief This file contains the definition of the DiveLog class.

  This file is part of ScubaLog, a dive logging application for KDE.
  ScubaLog is free software licensed under the GPL.

  \par Copyright:
  André Johansen
*/
//*****************************************************************************

#ifndef DIVELOG_H
#define DIVELOG_H

#include <qdatetime.h>
#include <qstring.h>

//*****************************************************************************
/*!
  \class DiveLog
  \brief The DiveLog class is used to hold log data for a dive.

  \author André Johansen
*/
//*****************************************************************************

class DiveLog
{
public:
  //! The plan types.
  enum PlanType_e {
    //! Single-level dive (with e.g. PADI recreational dive planner).
    e_SingleLevel,
    //! Multi-level dive (with e.g. ``the wheel'').
    e_MultiLevel
  };

  DiveLog();
  ~DiveLog();

  //! Get the log number for this dive.
  int logNumber() const { return m_nLogNumber; }
  //! Set the log number for this dive to \a nNumber.
  void setLogNumber(int nNumber) { m_nLogNumber = nNumber; }
  //! Get the date for this dive.
  QDate diveDate() const { return m_cDiveDate; }
  //! Set the date for this dive to \a cDate.
  void setDiveDate(QDate cDate) { m_cDiveDate = cDate; }
  //! Get the start-time for this dive.
  QTime diveStart() const { return m_cDiveStart; }
  //! Set the start time for this dive to \a cTime.
  void setDiveStart(QTime cTime) { m_cDiveStart = cTime; }
  //! Get the dive location.
  QString diveLocation() const { return m_cLocation; }
  //! Set the dive location to \a cLocation.
  void setDiveLocation(const QString& cLocation) { m_cLocation = cLocation; }
  //! Get the buddy name.
  QString buddyName() const { return m_cBuddyName; }
  //! Set the buddy name to \a cName.
  void setBuddyName(const QString& cName) { m_cBuddyName = cName; }
  //! Get the maximum depth on this dive.
  float maxDepth() const { return m_vMaxDepth; }
  //! Set the maximum depth for this dive to \a vDepth.
  void setMaxDepth(float vDepth) { m_vMaxDepth = vDepth; }
  //! Get the dive time (duration).
  QTime diveTime() const { return m_cDiveTime; }
  //! Set the dive time to \a cTime.
  void setDiveTime(QTime cTime) { m_cDiveTime = cTime; }
  //! Get the bottom-time for this dive (only applies to single-level).
  QTime bottomTime() const { return m_cBottomTime; }
  //! Set the bottom-time for this dive to \a cTime.
  void setBottomTime(QTime cTime) { m_cBottomTime = cTime; }
  //! Get the gas-type(s) used in this dive.
  QString gasType() const { return m_cGasType; }
  //! Set the gas-type(s) used in this dive to \a eType.
  void setGasType(const QString& cType) { m_cGasType = cType; }
  //! Get the air temperature on this dive.
  float airTemperature() const { return m_vAirTemperature; }
  //! Set the air temperature on this dive to \a vTemp.
  void setAirTemperature(float vTemp) { m_vAirTemperature = vTemp; }
  //! Get the water surface temperature on this dive.
  float waterSurfaceTemperature() const { return m_vSurfaceTemperature; }
  //! Set the water surface temperature on this dive to \a vTemp.
  void setWaterSurfaceTemperature(float vTemp) {m_vSurfaceTemperature = vTemp;}
  //! Get the water temperature on this dive.
  float waterTemperature() const { return m_vWaterTemperature; }
  //! Set the water temperature on this dive to \a vTemp.
  void setWaterTemperature(float vTemp) { m_vWaterTemperature = vTemp; }
  //! Get the plan-type for this dive.
  PlanType_e planType() const { return m_ePlanType; }
  //! Set the plan-type for this dive to \e eType.
  void setPlanType(PlanType_e eType) { m_ePlanType = eType; }
  //! Get the dive type for this dive.
  QString diveType() const { return m_cDiveType; }
  //! Set the dive type for this dive to \a cDiveType.
  void setDiveType(const QString& cDiveType) { m_cDiveType = cDiveType; }
  //! Get the dive description.
  QString diveDescription() const { return m_cDiveDescription; }
  //! Set the dive description to \a cDescription.
  void setDiveDescription(const QString& cDescription) {
    m_cDiveDescription = cDescription;
  }
  //! Get the surface air consuption.
  unsigned int surfaceAirConsuption() const { return m_nNumLitresUsed; }
  //! Set the surface air consuption to \a nLitres.
  void setSurfaceAirConsumption(int nLitres) { m_nNumLitresUsed = nLitres; }

private:
  //! Disabled copy constructor.
  DiveLog(const DiveLog&);
  //! Disabled assignment operator.
  DiveLog& operator =(const DiveLog&);

  //! The log number.
  int        m_nLogNumber;
  //! The date of the dive.
  QDate      m_cDiveDate;
  //! The time of the start of the dive.
  QTime      m_cDiveStart;
  //! The location of the dive.
  QString    m_cLocation;
  //! The name of the buddy.
  QString    m_cBuddyName;
  //! The maximum dive depth, expressed in meters below the surface.
  float      m_vMaxDepth;
  //! The dive time (time from entering the water until reaching the surface).
  QTime      m_cDiveTime;
  //! The bottom time (time from entering the water until leaving the bottom).
  QTime      m_cBottomTime;
  //! The gas type.
  QString    m_cGasType;
  //! Number of litres og gas used.
  int        m_nNumLitresUsed;
  //! Air temerature, degrees Celcius.
  float      m_vAirTemperature;
  //! Water temperature at the surface.
  float      m_vSurfaceTemperature;
  //! Minimum water temperature.
  float      m_vWaterTemperature;
  //! The plan type used for this dive.
  PlanType_e m_ePlanType;
  //! The dive type -- main description of this dive.
  QString    m_cDiveType;
  //! A long description about the dive.
  QString    m_cDiveDescription;
};

#endif // DIVELOG_H

// Local Variables:
// mode: c++
// tab-width: 8
// c-basic-offset: 2
// indent-tabs-mode: nil
// coding: utf-8
// End:
