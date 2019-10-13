# Install script for directory: D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/pcapCent")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Debug/pcapCent.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Release/pcapCent.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/MinSizeRel/pcapCent.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/RelWithDebInfo/pcapCent.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE SHARED_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/Debug/pcapCent.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE SHARED_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/Release/pcapCent.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE SHARED_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/MinSizeRel/pcapCent.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE SHARED_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/RelWithDebInfo/pcapCent.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Debug/pcapCent_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Release/pcapCent_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/MinSizeRel/pcapCent_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/amd64" TYPE STATIC_LIBRARY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/RelWithDebInfo/pcapCent_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Debug/pcapCent_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/Release/pcapCent_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/MinSizeRel/pcapCent_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/RelWithDebInfo/pcapCent_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/Debug/pcapCent.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/Release/pcapCent.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/MinSizeRel/pcapCent.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/amd64" TYPE FILE OPTIONAL FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/run/RelWithDebInfo/pcapCent.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pcap" TYPE DIRECTORY FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/pcap/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/pcap.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/pcap-bpf.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/pcap-namedb.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/testprogs/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/A/SourceCode/Aurora/SelenaSoft/Nmap-7.70/libpcap/Build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
