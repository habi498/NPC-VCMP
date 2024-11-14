cmake_minimum_required (VERSION 3.15)

# Add source to this project's executable.
add_executable (npcclient 
"client/main.cpp"
"client/NPCClient.cpp"
"client/NPCFunctions.cpp"
"client/NPCFunctions2.cpp"
"client/Playback.cpp"
"client/Player.cpp"
"client/Playerpool.cpp"
"client/SquirrelVM.cpp"
"client/UpdateNPC.cpp"
"client/CTimer.cpp"
"client/NPCFunctions3.cpp"
"client/ClientFunctions.cpp" 
"client/ConsoleInput.cpp"
"client/EventHandler.cpp" 
"client/Plugins.cpp"
"client/SqVector.cpp"  
"client/VehiclePool.cpp"  
"client/SqQuaternion.cpp"
"client/Exports.cpp"
"client/NPCFunctions4.cpp" 
"client/NPCFunctions5.cpp"  
"client/NPCFunctions6.cpp" 
"client/FileReceiver.cpp")

if(REL047)
#Add compression source file
target_sources(npcclient PRIVATE "client/compress.cpp")
#Use SlikeNet Static Library
option( RAKNET_ENABLE_SAMPLES 	"Generate RakNet sample projects if true." 		FALSE  )
option( RAKNET_ENABLE_DLL		"Generate the DLL project if true." 			FALSE )
option( RAKNET_ENABLE_STATIC	"Generate the static library project if true." 	TRUE )
add_subdirectory("dependencies/slikenet_0.1.3_source/slikenet")
target_link_libraries(npcclient RakNetLibStatic )
message(STATUS "Using SlikeNet")

if(REL0470)
add_definitions(-D_REL0470)
elseif(REL0471)
add_definitions(-D_REL0471)
endif()

#Include zlib
add_subdirectory("dependencies/zlib")

add_dependencies(RakNetLibStatic zlibstatic)
target_link_libraries(RakNetLibStatic zlibstatic)
#RakNetSocket2.cpp needs zlib.h, zlibconf.h etc
target_include_directories(RakNetLibStatic PRIVATE "dependencies/zlib")

target_include_directories(npcclient PRIVATE "dependencies/slikenet_0.1.3_source/slikenet/Source")

if(REL0470)
set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient_047") 
elseif(REL0471)
set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient_0471") 
else()
set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient_047_unknown") 
endif()

target_sources(npcclient PRIVATE "client/app.rc")
else()
#include Raknet files (4.082?)
message(STATUS "Using Raknet")
target_sources(npcclient PRIVATE 
"Raknet/Base64Encoder.cpp"
"Raknet/BitStream.cpp"
"Raknet/CCRakNetSlidingWindow.cpp"
"Raknet/CCRakNetUDT.cpp"
"Raknet/CheckSum.cpp"
"Raknet/CloudClient.cpp"
"Raknet/CloudCommon.cpp"
"Raknet/CloudServer.cpp"
"Raknet/CommandParserInterface.cpp"
"Raknet/ConnectionGraph2.cpp"
"Raknet/ConsoleServer.cpp"
"Raknet/DataCompressor.cpp"
"Raknet/DirectoryDeltaTransfer.cpp"
"Raknet/DR_SHA1.cpp"
"Raknet/DS_BytePool.cpp"
"Raknet/DS_ByteQueue.cpp"
"Raknet/DS_HuffmanEncodingTree.cpp"
"Raknet/DS_Table.cpp"
"Raknet/DynDNS.cpp"
"Raknet/EmailSender.cpp"
"Raknet/EpochTimeToString.cpp"
"Raknet/FileList.cpp"
"Raknet/FileListTransfer.cpp"
"Raknet/FileOperations.cpp"
"Raknet/FormatString.cpp"
"Raknet/FullyConnectedMesh2.cpp"
"Raknet/Getche.cpp"
"Raknet/Gets.cpp"
"Raknet/GetTime.cpp"
"Raknet/gettimeofday.cpp"
"Raknet/GridSectorizer.cpp"
"Raknet/HTTPConnection.cpp"
"Raknet/HTTPConnection2.cpp"
"Raknet/IncrementalReadInterface.cpp"
"Raknet/Itoa.cpp"
"Raknet/LinuxStrings.cpp"
"Raknet/LocklessTypes.cpp"
"Raknet/LogCommandParser.cpp"
"Raknet/MessageFilter.cpp"
"Raknet/NatPunchthroughClient.cpp"
"Raknet/NatPunchthroughServer.cpp"
"Raknet/NatTypeDetectionClient.cpp"
"Raknet/NatTypeDetectionCommon.cpp"
"Raknet/NatTypeDetectionServer.cpp"
"Raknet/NetworkIDManager.cpp"
"Raknet/NetworkIDObject.cpp"
"Raknet/PacketConsoleLogger.cpp"
"Raknet/PacketFileLogger.cpp"
"Raknet/PacketizedTCP.cpp"
"Raknet/PacketLogger.cpp"
"Raknet/PacketOutputWindowLogger.cpp"
"Raknet/PluginInterface2.cpp"
"Raknet/PS4Includes.cpp"
"Raknet/Rackspace.cpp"
"Raknet/RakMemoryOverride.cpp"
"Raknet/RakNetCommandParser.cpp"
"Raknet/RakNetSocket.cpp"
"Raknet/RakNetSocket2.cpp"
"Raknet/RakNetSocket2_360_720.cpp"
"Raknet/RakNetSocket2_Berkley.cpp"
"Raknet/RakNetSocket2_Berkley_NativeClient.cpp"
"Raknet/RakNetSocket2_NativeClient.cpp"
"Raknet/RakNetSocket2_PS3_PS4.cpp"
"Raknet/RakNetSocket2_PS4.cpp"
"Raknet/RakNetSocket2_Vita.cpp"
"Raknet/RakNetSocket2_WindowsStore8.cpp"
"Raknet/RakNetSocket2_Windows_Linux.cpp"
"Raknet/RakNetSocket2_Windows_Linux_360.cpp"
"Raknet/RakNetStatistics.cpp"
"Raknet/RakNetTransport2.cpp"
"Raknet/RakNetTypes.cpp"
"Raknet/RakPeer.cpp"
"Raknet/RakSleep.cpp"
"Raknet/RakString.cpp"
"Raknet/RakThread.cpp"
"Raknet/RakWString.cpp"
"Raknet/Rand.cpp"
"Raknet/RandSync.cpp"
"Raknet/ReadyEvent.cpp"
"Raknet/RelayPlugin.cpp"
"Raknet/ReliabilityLayer.cpp"
"Raknet/ReplicaManager3.cpp"
"Raknet/Router2.cpp"
"Raknet/RPC4Plugin.cpp"
"Raknet/SecureHandshake.cpp"
"Raknet/SendToThread.cpp"
"Raknet/SignaledEvent.cpp"
"Raknet/SimpleMutex.cpp"
"Raknet/SocketLayer.cpp"
"Raknet/StatisticsHistory.cpp"
"Raknet/StringCompressor.cpp"
"Raknet/StringTable.cpp"
"Raknet/SuperFastHash.cpp"
"Raknet/TableSerializer.cpp"
"Raknet/TCPInterface.cpp"
"Raknet/TeamBalancer.cpp"
"Raknet/TeamManager.cpp"
"Raknet/TelnetTransport.cpp"
"Raknet/ThreadsafePacketLogger.cpp"
"Raknet/TwoWayAuthentication.cpp"
"Raknet/UDPForwarder.cpp"
"Raknet/UDPProxyClient.cpp"
"Raknet/UDPProxyCoordinator.cpp"
"Raknet/UDPProxyServer.cpp"
"Raknet/VariableDeltaSerializer.cpp"
"Raknet/VariableListDeltaTracker.cpp"
"Raknet/VariadicSQLParser.cpp"
"Raknet/VitaIncludes.cpp"
"Raknet/WSAStartupSingleton.cpp"
"Raknet/_FindFirst.cpp"
"client/app.rc" )
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(OUTPUT_BITNESS 64)
else()
	set(OUTPUT_BITNESS 32)
endif()

if(FORCE_32)
	set_target_properties(npcclient PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	set(OUTPUT_BITNESS 32)
endif()

if(REL004)
	message(STATUS "You are building for rel004")
    add_definitions(-D_REL004)
	set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient_r004") 
elseif(REL006) #after 1.8 beta3
	message(STATUS "You are building for rel006")
       add_definitions(-D_REL006)
endif()

target_include_directories(npcclient PRIVATE "${PROJECT_SOURCE_DIR}/npcclient")
target_include_directories(npcclient PRIVATE "${PROJECT_SOURCE_DIR}/include")

if(with-squirrel AND WIN32)
	message(STATUS "with-squirrel is disabled for windows. It will be compiled from source, which is included")
endif()

if(with-squirrel AND NOT WIN32)
	#only on linux, use squirrel, sqstdlib. In windows /MT problems, etc
	target_link_libraries(npcclient  ${with-squirrel}/lib/libsquirrel_static.a)
	target_link_libraries(npcclient  ${with-squirrel}/lib/libsqstdlib_static.a)
	target_include_directories(npcclient PRIVATE ${with-squirrel}/include)
else()
#First build squirrel libs
option(DISABLE_DYNAMIC "Disable building squirrel-dynamic libraries" ON)
#No need to include squirrel include directory, because in building squirrel, those will be included
add_subdirectory("dependencies/squirrel-master" "dependencies/squirrel-master")
if(FORCE_32)  
set_target_properties(squirrel_static PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
set_target_properties(sqstdlib_static PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()
target_link_libraries(npcclient squirrel_static)
target_link_libraries(npcclient sqstdlib_static)
endif()

   
if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
   if(SLIKENET)
   	target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/npcConnect.lib")
   else()
   	target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/npc_static.lib")
   endif()
   target_link_libraries(npcclient ws2_32)
else(WIN32)
	
    if(OUTPUT_BITNESS EQUAL 32)    
        if(SLIKENET)
		target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/npcConnect.a")
	  else()
		target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libnpc_static.a")
	  endif()	
else()
         if(SLIKENET)
		target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/npcConnect.a")
	  else()
		target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libnpc_static.a")
	  endif()
 endif()
    set(CMAKE_CXX_FLAGS "-pthread")
    add_definitions(-DLINUX)
	target_link_libraries(npcclient ${CMAKE_DL_LIBS})
    if(FORCE_32 OR OUTPUT_BITNESS EQUAL 32)
        if(REL004)
            set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32_r004")
        elseif(REL0471)
			set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32_0471")
		elseif(REL0470)
			set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32_0470") 
		else()
           set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32")
        endif()
    else()
        if(REL004)
            set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient64_r004")
        elseif(REL0471)
		set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient64_0471")
	  elseif(REL0470)
		set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient64_0470")
	  else()
            set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient64")
        endif()
    endif()
	
	
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

set_target_properties(npcclient PROPERTIES LINK_FLAGS_RELEASE -s)

#set(CMAKE_BUILD_TYPE Release)
#build readline
if(NOT WIN32)
include(ExternalProject)
if(with-libreadline)
	target_link_libraries(npcclient ${with-libreadline}/lib/libhistory.a)
	target_link_libraries(npcclient ${with-libreadline}/lib/libreadline.a)
	target_include_directories(npcclient PRIVATE ${with-libreadline}/include)
else()
message(STATUS "libreadline libraries not specified(-Dwith-libreadline=path). So it will be compiled")
if(OUTPUT_BITNESS EQUAL 32)  
message(STATUS "setting -m32 c flags and cxx flags to libreadline")
#message(STATUS "Cxx flags is ${CMAKE_CXX_FLAGS} and c flags is ${CMAKE_C_FLAGS}")
ExternalProject_Add(libreadline
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/readline-8.2
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/readline-8.2/configure --prefix=<INSTALL_DIR> --enable-shared=false --enable-install-examples=false --with-curses
    INSTALL_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline
    BINARY_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline-out
	CMAKE_ARGS "-DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
    BUILD_COMMAND ${MAKE})
else()
ExternalProject_Add(libreadline
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/readline-8.2
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/readline-8.2/configure --prefix=<INSTALL_DIR> --enable-shared=false --enable-install-examples=false --with-curses
    INSTALL_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline
    BINARY_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline-out
    BUILD_COMMAND ${MAKE})
endif()
target_include_directories(npcclient PRIVATE ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline/include)
target_link_libraries(npcclient ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline/lib/libhistory.a)
target_link_libraries(npcclient ${CMAKE_BINARY_DIR}/npcclient/dependencies/libreadline/lib/libreadline.a)
add_dependencies(npcclient libreadline)
endif()

if(with-ncurses)
target_link_libraries(npcclient ${with-ncurses}/lib/libncurses.a)
target_include_directories(npcclient PRIVATE ${with-libreadline}/include)
else()
message(STATUS "ncurses libraries not specified.(-Dwith-ncurses=path). So it will be compiled")
if(OUTPUT_BITNESS EQUAL 32)  
message(STATUS "setting -m32 c flags and cxx flags to libncurses")
ExternalProject_Add(libncurses
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ncurses-6.4
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ncurses-6.4/configure --prefix=<INSTALL_DIR> 
    INSTALL_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libncurses
    BINARY_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libncurses-out
	CMAKE_ARGS "-DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
    BUILD_COMMAND ${MAKE})
else()
ExternalProject_Add(libncurses
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ncurses-6.4
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ncurses-6.4/configure --prefix=<INSTALL_DIR> 
    INSTALL_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libncurses
    BINARY_DIR ${CMAKE_BINARY_DIR}/npcclient/dependencies/libncurses-out
    BUILD_COMMAND ${MAKE})
endif()
target_link_libraries(npcclient ${CMAKE_BINARY_DIR}/npcclient/dependencies/libncurses/lib/libncurses.a)
add_dependencies(npcclient libncurses)
endif()

endif()
