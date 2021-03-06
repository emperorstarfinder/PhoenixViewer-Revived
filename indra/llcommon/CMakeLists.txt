# -*- cmake -*-

project(llcommon)

include(00-Common)
include(LLCommon)
include(Utils)
include(Linking)
include(Boost)
include(GooglePerfTools)
include(ZLIB)

include_directories(
    ${EXPAT_INCLUDE_DIRS}
    ${LLCOMMON_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

set(llcommon_SOURCE_FILES
	imageids.cpp
	indra_constants.cpp
    llapp.cpp
    llapr.cpp
    llaprpool.cpp
    llassettype.cpp
    llavatarname.cpp
    llbase32.cpp
    llbase64.cpp
    llcommon.cpp
    llcommonutils.cpp
    llcrc.cpp
    llcriticaldamp.cpp
    llcursortypes.cpp
    lldate.cpp
	lldependencies.cpp
	lldictionary.cpp
    llerror.cpp
    llerrorthread.cpp
    llevent.cpp
    lleventapi.cpp
    lleventdispatcher.cpp
    llevents.cpp
    lleventtimer.cpp
    llfasttimer.cpp
    llfile.cpp
    llfindlocale.cpp
    llfixedbuffer.cpp
	llfoldertype.cpp
    llformat.cpp
    llframetimer.cpp
    llheartbeat.cpp
	llinstancetracker.cpp
    llliveappconfig.cpp
    lllivefile.cpp
    lllog.cpp
    llmd5.cpp
    llmemory.cpp
    llmemorystream.cpp
    llmetrics.cpp
    llmortician.cpp
    lloptioninterface.cpp
    llprocesslauncher.cpp
    llprocessor.cpp
    llqueuedthread.cpp
    llrand.cpp
    llrefcount.cpp
    llrun.cpp
    llscopedvolatileaprpool.h
    llsd.cpp
    llsdserialize.cpp
    llsdserialize_xml.cpp
    llsdutil.cpp
    llsecondlifeurls.cpp
    llsingleton.cpp
    llstat.cpp
    llstreamtools.cpp
    llstring.cpp
    llstringtable.cpp
    llsys.cpp
    llthread.cpp
    lltimer.cpp
    lluri.cpp
    lluuid.cpp
    llworkerthread.cpp
    metaclass.cpp
    metaproperty.cpp
    reflective.cpp
    u64.cpp
    )
    
set(llcommon_HEADER_FILES
    CMakeLists.txt

    bitpack.h
    ctype_workaround.h
    doublelinkedlist.h
    imageids.h
    indra_constants.h
    linden_common.h
    linked_lists.h
    llagentconstants.h
    llapp.h
    llapr.h
    llaprpool.h
    llassettype.h
    llassoclist.h
    llavatarconstants.h
    llavatarname.h
    llbase32.h
    llbase64.h
    llboost.h
    llchat.h
    llclickaction.h
    llcommon.h
    llcommonutils.h
    llcrc.h
    llcriticaldamp.h
    llcursortypes.h
    lldarray.h
    lldarrayptr.h
    lldate.h
    lldefs.h
	lldependencies.h
    lldepthstack.h
	lldictionary.h
    lldlinked.h
    lldqueueptr.h
    llendianswizzle.h
    llenum.h
    llerror.h
    llerrorcontrol.h
    llerrorlegacy.h
    llerrorthread.h
    llevent.h
    lleventapi.h
    lleventdispatcher.h
    lleventemitter.h
	llevents.h
    lldepthstack.h
    llextendedstatus.h
    llfasttimer.h
    llfile.h
    llfindlocale.h
    llfixedbuffer.h
	llfoldertype.h
    llformat.h
    llframetimer.h
    llhash.h
    llheartbeat.h
    llhttpstatuscodes.h
    llindexedqueue.h
    llinstancetracker.h
    llkeythrottle.h
    lllinkedqueue.h
    llliveappconfig.h
    lllivefile.h
    lllocalidhashmap.h
    lllog.h
    lllslconstants.h
    llmap.h
    llmd5.h
    llmemory.h
    llmemorystream.h
    llmemtype.h
    llmetrics.h
    llmortician.h
    llnametable.h
    lloptioninterface.h
    llpointer.h
    llpreprocessor.h
    llpriqueuemap.h
    llprocesslauncher.h
    llprocessor.h
    llptrskiplist.h
    llptrskipmap.h
    llqueuedthread.h
    llrand.h
    llrefcount.h
    llrun.h
    llsafehandle.h
    llsd.h
    llsdserialize.h
    llsdserialize_xml.h
    llsdutil.h
    llsecondlifeurls.h
    llsimplehash.h
	llsingleton.h
    llskiplist.h
    llskipmap.h
    llstack.h
    llstat.h
    llstatenums.h
    llstl.h
    llstreamtools.h
    llstrider.h
    llstring.h
    llstringtable.h
    llsys.h
    llthread.h
    lltimer.h
    lluri.h
    lluuid.h
    lluuidhashmap.h
#    llversionserver.h
#    llversionviewer.h
    llworkerthread.h
	ll_template_cast.h
    metaclass.h
    metaclasst.h
    metaproperty.h
    metapropertyt.h
    reflective.h
    reflectivet.h
    roles_constants.h
    stdenums.h
    stdtypes.h
    string_table.h
	stringize.h
    timer.h
    timing.h
    u64.h
    )

set_source_files_properties(${llcommon_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llcommon_SOURCE_FILES ${llcommon_HEADER_FILES})

configure_files(llcommon_GENERATED_HEADER_FILES
    llversionserver.h
    llversionviewer.h
    )
set_source_files_properties(${llcommon_GENERATED_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llcommon_SOURCE_FILES ${llcommon_GENERATED_HEADER_FILES})

add_library (llcommon SHARED ${llcommon_SOURCE_FILES})
add_dependencies(llcommon prepare)
target_link_libraries(
    llcommon
    ${APRUTIL_LIBRARIES}
    ${APR_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${ZLIB_LIBRARIES}
	${WINDOWS_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${GOOGLE_PERFTOOLS_LIBRARIES}
    )

if (DARWIN)
    include(CMakeFindFrameworks)
    find_library(CARBON_LIBRARY Carbon)
    target_link_libraries(llcommon ${CARBON_LIBRARY})
    # Don't embed a full path in the library's install name
    set_target_properties(
        llcommon
        PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        INSTALL_NAME_DIR "@executable_path/../Resources"
		)
endif (DARWIN)
