if(BUILD_TESTING)
  add_subdirectory( tests )
endif()

set (libconverter_a_SOURCES
  mymoneystatementreader.cpp
  mymoneytemplate.cpp
  webpricequote.cpp
  transactionmatchfinder.cpp
  existingtransactionmatchfinder.cpp
  scheduledtransactionmatchfinder.cpp
  ../widgets/kmymoneymoneyvalidator.cpp
)

add_library(converter STATIC ${libconverter_a_SOURCES})

# TODO: clean dependencies
target_link_libraries(converter PUBLIC KF5::Service KF5::XmlGui KF5::Completion KF5::TextWidgets KF5::WidgetsAddons KF5::ConfigCore KF5::I18n Qt5::Widgets Qt5::Gui Qt5::Xml Alkimia::alkimia KF5::KIOWidgets kmm_csvimportercore
PRIVATE
newaccountwizard)

# we rely on some dialogs to be generated
add_dependencies(converter dialogs)

########### install files ###############

install(FILES mymoneytemplate.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/kmymoney COMPONENT Devel)

