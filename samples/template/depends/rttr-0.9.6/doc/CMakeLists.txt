####################################################################################
#                                                                                  #
#  Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

find_package(Doxygen)
# These variables are defined in the doxygen file(@VAR@) as place holders
# an will be substituted by the following values
set(DOXYGEN_PROJECT_NUMBER    "${RTTR_VERSION_STR}")
set(DOXYGEN_PROJECT_NAME      "${PROJECT_NAME}")
set(DOXYGEN_INPUT_DIRECTORY   "${CMAKE_SOURCE_DIR}/src/rttr \\
                               ${CMAKE_CURRENT_SOURCE_DIR}" )
set(DOXYGEN_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/doc")
set(DOXYGEN_DOC_INSTALL_DIR   "doc")
set(DOXYGEN_CONFIG_FILE_IN    "DoxyFile.cfg.in")
set(DOXYGEN_LAYOUT_FILE_IN    "doxygen-customization/layout/DoxygenLayout.xml.in")
set(DOXYGEN_EXCLUDE           "")
set(DOXYGEN_STRIP_FROM_PATH   "${CMAKE_SOURCE_DIR}/src/rttr")
set(DOXYGEN_CUSTOM_HTML_DIR   "rttr-${RTTR_VERSION_MAJOR}-${RTTR_VERSION_MINOR}-${RTTR_VERSION_PATCH}")
set(DOXYGEN_PREDEFINED        "DOXYGEN \\
                               RTTR_INLINE= \\
                               RTTR_FORCE_INLINE= \\
                               RTTR_API= \\
                               RTTR_NOEXCEPT=noexcept \\
                               RTTR_CONSTEXPR=constexpr \\
                               RTTR_CXX14_CONSTEXPR=constexpr \\
                               RTTR_STATIC_CONSTEXPR=static constexpr \\
                               RTTR_NOEXCEPT_OR_NOTHROW=throw() \\
                               RTTR_DEPRECATED_WITH_MSG= \\
                               RTTR_LOCAL= \\
                               ")
set(DOXYGEN_EXCLUDE_SYMBOLS   "impl* \\
                               impl::* \\
                               detail* \\
                               detail:: \\
                               ")

set(DOXYGEN_EXCLUDE_PATTERNS  "*.cpp* \\
                              *_p.cpp* \\
                              *PCH.h* \\
                              *_impl.h* \\
                              *_p.h* \\
                              */src/rttr/detail/* \\
                              */src/test/* \\
                              */src/benchmark/*")

set(DOXYGEN_INDEX_FILE_IN   "index.html.in")

#### LAYOUT VARIABLES
set(DOXY_LAYOUT_MAIN_PAGE_TITLE "RTTR")
set(DOXY_LAYOUT_PAGES_TITLE     "Tutorial")
set(DOXY_LAYOUT_PAGES_TITLE2    "Installation")
set(DOXY_LAYOUT_PAGES_TITLE3    "License")


if (${CUSTOM_DOXYGEN_STYLE})
  set(DOXYGEN_CUSTOM_HTML_FOOTER     "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/html/footer.html")
  set(DOXYGEN_CUSTOM_HTML_HEADER     "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/html/header.html")
  set(DOXYGEN_CUSTOM_HTML_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/css/custom-doxygen.css")
endif()

if(NOT DOXYGEN_FOUND)
  message(WARNING "Doxygen not found - Documentation will not be generated!")
  return()
endif()

string ( REGEX REPLACE ".cfg.in$" ".cfg" DOXYGEN_CONFIG_FILE ${DOXYGEN_CONFIG_FILE_IN} )
string ( REGEX REPLACE ".html.in$" ".html" DOXYGEN_INDEX_FILE ${DOXYGEN_INDEX_FILE_IN} )
string ( REGEX REPLACE ".xml.in$" ".xml" DOXYGEN_LAYOUT_FILE ${DOXYGEN_LAYOUT_FILE_IN} )
string ( REGEX REPLACE "[^\\/]*/(.*)" "\\1" DOXYGEN_LAYOUT_FILE "${DOXYGEN_LAYOUT_FILE}")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_IN})
  message(WARNING "Doxygen configuration file not found - Documentation will not be generated!")
  return()
endif()

# modify the doxygen file, i.e. substitute the variables with values
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CONFIG_FILE})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_LAYOUT_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_LAYOUT_FILE})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_INDEX_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_INDEX_FILE})
# copy our shortcut html file (this 'index.html' will call itself the 'index.html' from doxygen,
# but this file is hard to locate in the big doxygen output directory)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/index.html DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY})

if (${CUSTOM_DOXYGEN_STYLE})
  #fonts
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/fonts/ptsans_regular_macroman DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR}/fonts)
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/fonts/source_code_pro_regular DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR}/fonts)


  #bootstrap
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/3rd-party/bootstrap-3.3.2/css/bootstrap.min.css DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR}/bootstrap/css)
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/3rd-party/bootstrap-3.3.2/js/bootstrap.min.js DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR}/bootstrap/js)
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/css/custom-bootstrap.css DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})
  #jquery
  file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/3rd-party/jquery-2.1.3/jquery.min.js DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})
  #the connection between the doxygen style and the bootstrap style
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/js/doxy-boot.js DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-customization/img/favicon.ico DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})
endif()

#########################################################################################

add_custom_target(doc ALL
                  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CONFIG_FILE}"
                  WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}"
                  COMMENT "Building documentation...")

set_target_properties(doc PROPERTIES FOLDER "Documentation")

install(DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR}"
        DESTINATION "${DOXYGEN_DOC_INSTALL_DIR}"
        PATTERN "*.*"
        PERMISSIONS OWNER_READ)

install(FILES "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_INDEX_FILE}"
        DESTINATION "${DOXYGEN_DOC_INSTALL_DIR}"
        PERMISSIONS OWNER_READ)

#########################################################################################

if (BUILD_WEBSITE_DOCU)
    set(DOXYGEN_OUTPUT_DIRECTORY        "${CMAKE_BINARY_DIR}/doc_website")
    set(DOXYGEN_CUSTOM_HTML_FOOTER      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen_website_customization/html/footer.html")
    set(DOXYGEN_CUSTOM_HTML_HEADER      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen_website_customization/html/header.html")
    set(DOXYGEN_CUSTOM_HTML_STYLESHEET  "${CMAKE_CURRENT_SOURCE_DIR}/doxygen_website_customization/css/custom-doxygen.css")
    set(DOXYGEN_CUSTOM_HTML_DIR         "master")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CONFIG_FILE})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_LAYOUT_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_LAYOUT_FILE})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_INDEX_FILE_IN} ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_INDEX_FILE})

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen_website_customization/css/custom-bootstrap.css DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen_website_customization/js/doxy-boot.js DESTINATION ${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CUSTOM_HTML_DIR})

    add_custom_target(doc_website ALL
                      COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_CONFIG_FILE}"
                      WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}"
                      COMMENT "Building documentation...")
    
    set_target_properties(doc_website PROPERTIES FOLDER "Documentation")
endif()

#########################################################################################
