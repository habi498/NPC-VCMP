cmake_minimum_required (VERSION 3.8)
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
"client/NPCFunctions3.cpp"
"client/ClientFunctions.cpp" 
"client/ConsoleInput.cpp"
"client/EventHandler.cpp" 
"client/Plugins.cpp"
 "client/sqVector.cpp"  
"client/VehiclePool.cpp"  
"client/sqQuaternion.cpp" "client/Exports.cpp" "client/NPCFunctions4.cpp")
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
endif()
target_include_directories(npcclient PRIVATE "${PROJECT_SOURCE_DIR}/npcclient")
target_include_directories(npcclient PRIVATE "${PROJECT_SOURCE_DIR}/include")

if(WIN32)
    target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/squirrel_static.lib")
    target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/sqstdlib_static.lib")
    target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/npc_static.lib")
    target_link_libraries(npcclient wsock32 ws2_32)
else(WIN32)
	
    if(OUTPUT_BITNESS EQUAL 32)    
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libsquirrel_static.a")
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libsqstdlib_static.a")
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libnpc_static.a")
		target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libreadline.a")
		  target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libhistory.a")
target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x32/libtinfo.a") 
else()
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libsquirrel_static.a")
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libsqstdlib_static.a")
        target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libnpc_static.a")
	     target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libreadline.a")
		  target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libhistory.a")
target_link_libraries(npcclient "${PROJECT_SOURCE_DIR}/npcclient/libs/x64/libncurses.a")
    endif()
    set(CMAKE_CXX_FLAGS "-pthread")
    add_definitions(-DLINUX)
	target_link_libraries(npcclient ${CMAKE_DL_LIBS})
    if(FORCE_32)
        if(REL004)
            set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32_r004")
        else()
           set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient32")
        endif()
    else()
        if(REL004)
            set_target_properties(npcclient PROPERTIES OUTPUT_NAME "npcclient64_r004")
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

set(CMAKE_BUILD_TYPE Release)
