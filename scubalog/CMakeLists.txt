set(SCUBALOG_SRC
  dateitem.cpp
  divelog.cpp
  equipmentlog.cpp
  equipmentview.cpp
  htmlexporter.cpp
  integerdialog.cpp
  kdateedit.cpp
  kdatevalidator.cpp
  kintegeredit.cpp
  kintegervalidator.cpp
  ktimeedit.cpp
  ktimevalidator.cpp
  listbox.cpp
  locationlog.cpp
  locationview.cpp
  logbook.cpp
  loglistview.cpp
  logview.cpp
  main.cpp
  personalinfoview.cpp
  scubalog.cpp
  scubalogproject.cpp
  udcfexporter.cpp
  udcfimporter.cpp
)

add_executable(scubalog ${SCUBALOG_SRC})

configure_file(config.h.in config.h)

target_include_directories(scubalog
                           PUBLIC
                           "${PROJECT_BINARY_DIR}/scubalog")

target_link_libraries(
  scubalog
  KF5::XmlGui
  KF5::I18n
  Qt5::Widgets
  Qt5::PrintSupport
)
