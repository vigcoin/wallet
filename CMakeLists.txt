cmake_minimum_required(VERSION 2.8)

include(CryptoNoteWallet.cmake)

project(${CN_PROJECT_NAME})

execute_process(COMMAND git log -1 --pretty=format:%h
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
OUTPUT_VARIABLE GIT_REVISION)

set(CRYPTONOTE_LIB cryptonote)

include_directories(${CMAKE_BINARY_DIR}
    src
    cryptonote/external
    cryptonote/include
    cryptonote/contrib/epee/include
    cryptonote/src/common
    cryptonote/src/crypto
    cryptonote/src)

find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
if(WIN32)
  set(Boost_USE_STATIC_RUNTIME OFF)
else(WIN32)
  set(Boost_USE_STATIC_RUNTIME ON)
endif(WIN32)

find_package(Boost 1.58 REQUIRED COMPONENTS date_time filesystem program_options regex serialization system thread chrono)
if ((${Boost_MAJOR_VERSION} EQUAL 1) AND (${Boost_MINOR_VERSION} EQUAL 54))
	message(SEND_ERROR "Boost version 1.54 is unsupported, more details are available here http://goo.gl/RrCFmA")
endif ()

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(VERSION "1.0")
configure_file("cryptonote/src/version.h.in" "version.h")
configure_file("src/CryptoNoteWalletConfig.h.in" "CryptoNoteWalletConfig.h")

add_definitions(-DGIT_REVISION="${GIT_REVISION}")

set(CMAKE_AUTOMOC ON)

set(CRYPTONOTE_SOURCES
  cryptonote/external/miniupnpc/connecthostport.c
  cryptonote/external/miniupnpc/igd_desc_parse.c
  cryptonote/external/miniupnpc/minisoap.c
  cryptonote/external/miniupnpc/miniupnpc.c
  cryptonote/external/miniupnpc/miniwget.c
  cryptonote/external/miniupnpc/minixml.c
  cryptonote/external/miniupnpc/portlistingparse.c
  cryptonote/external/miniupnpc/receivedata.c
  cryptonote/external/miniupnpc/upnpcommands.c
  cryptonote/external/miniupnpc/upnpreplyparse.c
  cryptonote/src/config/common.cpp
  cryptonote/src/config/mainnet.cpp
  cryptonote/src/config/testnet.cpp
  cryptonote/src/common/base58.cpp
  cryptonote/src/common/StringTools.cpp
  cryptonote/src/common/StringView.cpp
  cryptonote/src/common/JsonValue.cpp
  cryptonote/src/common/ConsoleTools.cpp
  cryptonote/src/common/binary_array.cpp
  cryptonote/src/common/stream.cpp
  cryptonote/src/common/os.cpp
  cryptonote/src/common/hex.cpp
  cryptonote/src/common/file.cpp
  cryptonote/src/stream/MemoryInputStream.cpp
  cryptonote/src/stream/StdInputStream.cpp
  cryptonote/src/stream/StdOutputStream.cpp
  cryptonote/src/stream/StreamTools.cpp
  cryptonote/src/stream/StringOutputStream.cpp
  cryptonote/src/stream/VectorOutputStream.cpp
  cryptonote/src/crypto/blake256.c
  cryptonote/src/crypto/chacha.c
  cryptonote/src/crypto/crypto-ops-data.c
  cryptonote/src/crypto/crypto-ops.c
  cryptonote/src/crypto/crypto.c
  cryptonote/src/crypto/groestl.c
  cryptonote/src/crypto/hash-extra-blake.c
  cryptonote/src/crypto/hash-extra-groestl.c
  cryptonote/src/crypto/hash-extra-jh.c
  cryptonote/src/crypto/hash-extra-skein.c
  cryptonote/src/crypto/hash.c
  cryptonote/src/crypto/jh.c
  cryptonote/src/crypto/keccak.c
  cryptonote/src/crypto/oaes_lib.c
  cryptonote/src/crypto/random.c
  cryptonote/src/crypto/skein.c
  cryptonote/src/crypto/slow-hash.c
  cryptonote/src/crypto/tree-hash.c
  cryptonote/src/crypto/aesb.c
  cryptonote/src/cryptonote/core/TransactionExtra.cpp
  cryptonote/src/cryptonote/core/key.cpp
  cryptonote/src/cryptonote/core/account.cpp
  cryptonote/src/cryptonote/core/blockchain.cpp
  cryptonote/src/cryptonote/core/currency.cpp
  cryptonote/src/cryptonote/core/core.cpp
  cryptonote/src/cryptonote/core/Miner.cpp
  cryptonote/src/cryptonote/core/IBlock.cpp
  cryptonote/src/cryptonote/core/hardfork.cpp
  cryptonote/src/cryptonote/core/difficulty.cpp
  cryptonote/src/cryptonote/core/Transaction.cpp
  cryptonote/src/cryptonote/core/TransactionExtra.cpp
  cryptonote/src/cryptonote/core/TransactionUtils.cpp
  cryptonote/src/cryptonote/core/CryptoNoteTools.cpp
  cryptonote/src/cryptonote/core/tx_memory_pool.cpp
  cryptonote/src/cryptonote/core/checkpoints.cpp
  cryptonote/src/cryptonote/core/BlockchainMessages.cpp
  cryptonote/src/cryptonote/core/CryptoNoteFormatUtils.cpp
  cryptonote/src/cryptonote/core/blockchain/indexing/payment.cpp
  cryptonote/src/cryptonote/core/blockchain/indexing/orphan_block.cpp
  cryptonote/src/cryptonote/core/blockchain/indexing/generated_transaction.cpp
  cryptonote/src/cryptonote/core/blockchain/indexing/timestamp_transaction.cpp
  cryptonote/src/cryptonote/core/blockchain/indexing/timestamp_block.cpp
  cryptonote/src/cryptonote/core/blockchain/serializer/basics.cpp
  cryptonote/src/cryptonote/core/blockchain/serializer/block_index.cpp
  cryptonote/src/cryptonote/core/blockchain/serializer/crypto.cpp
  cryptonote/src/cryptonote/core/blockchain/serializer/transaction_index.cpp
  cryptonote/src/cryptonote/core/TransactionPrefixImpl.cpp
  cryptonote/src/cryptonote/crypto/crypto.cpp
  cryptonote/src/cryptonote/structures/block_entry.cpp
  cryptonote/src/cryptonote/structures/array.cpp
  cryptonote/src/NodeRpcProxy/NodeRpcProxy.cpp
  cryptonote/src/NodeRpcProxy/NodeErrors.cpp
  cryptonote/src/InProcessNode/InProcessNode.cpp
  cryptonote/src/InProcessNode/InProcessNodeErrors.cpp
  cryptonote/src/serialization/BinaryInputStreamSerializer.cpp
  cryptonote/src/serialization/BinaryOutputStreamSerializer.cpp
  cryptonote/src/serialization/JsonInputValueSerializer.cpp
  cryptonote/src/serialization/JsonOutputStreamSerializer.cpp
  cryptonote/src/serialization/KVBinaryInputStreamSerializer.cpp
  cryptonote/src/serialization/KVBinaryOutputStreamSerializer.cpp
  cryptonote/src/serialization/SerializationOverloads.cpp
  cryptonote/src/serialization/BlockchainExplorerDataSerialization.cpp
  cryptonote/src/transfers/BlockchainSynchronizer.cpp
  cryptonote/src/transfers/SynchronizationState.cpp
  cryptonote/src/transfers/TransfersConsumer.cpp
  cryptonote/src/transfers/TransfersContainer.cpp
  cryptonote/src/transfers/TransfersSubscription.cpp
  cryptonote/src/transfers/TransfersSynchronizer.cpp
  cryptonote/src/wallet/LegacyKeysImporter.cpp
  cryptonote/src/wallet/WalletAsyncContextCounter.cpp
  cryptonote/src/wallet/WalletErrors.cpp
  cryptonote/src/wallet_legacy/KeysStorage.cpp
  cryptonote/src/wallet_legacy/WalletLegacy.cpp
  cryptonote/src/wallet_legacy/WalletHelper.cpp
  cryptonote/src/wallet_legacy/WalletLegacySerializer.cpp
  cryptonote/src/wallet_legacy/WalletLegacySerialization.cpp
  cryptonote/src/wallet_legacy/WalletTransactionSender.cpp
  cryptonote/src/wallet_legacy/WalletUnconfirmedTransactions.cpp
  cryptonote/src/wallet_legacy/WalletUserTransactionsCache.cpp
  cryptonote/src/system/ContextGroup.cpp
  cryptonote/src/system/ContextGroup.cpp
  cryptonote/src/system/Event.cpp
  cryptonote/src/system/EventLock.cpp
  cryptonote/src/system/InterruptedException.cpp
  cryptonote/src/system/Ipv4Address.cpp
  cryptonote/src/system/TcpStream.cpp
  cryptonote/src/http/HttpRequest.cpp
  cryptonote/src/http/HttpParser.cpp
  cryptonote/src/http/HttpParserErrorCodes.cpp
  cryptonote/src/http/HttpResponse.cpp
  cryptonote/src/rpc/HttpClient.cpp
  cryptonote/src/rpc/JsonRpc.cpp
  cryptonote/src/p2p/NetNode.cpp
  cryptonote/src/p2p/LevinProtocol.cpp
  cryptonote/src/command_line/NetNodeConfig.cpp
  cryptonote/src/command_line/CoreConfig.cpp
  cryptonote/src/command_line/MinerConfig.cpp
  cryptonote/src/command_line/options.cpp
  cryptonote/src/p2p/PeerListManager.cpp
  cryptonote/src/cryptonote/protocol/handler.cpp
  cryptonote/src/logging/ILogger.cpp
  cryptonote/src/logging/LoggerGroup.cpp
  cryptonote/src/logging/CommonLogger.cpp
  cryptonote/src/logging/LoggerManager.cpp
  cryptonote/src/logging/FileLogger.cpp
  cryptonote/src/logging/StreamLogger.cpp
  cryptonote/src/logging/ConsoleLogger.cpp
  cryptonote/src/logging/LoggerMessage.cpp
  cryptonote/src/logging/LoggerRef.cpp
  cryptonote/src/blockchain_explorer/BlockchainExplorer.cpp
  cryptonote/src/blockchain_explorer/BlockchainExplorerDataBuilder.cpp
  cryptonote/src/blockchain_explorer/BlockchainExplorerErrors.cpp
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/gui/ui/*.ui")

set(QRC src/resources.qrc)

qt5_wrap_ui(UIS ${FORMS})
qt5_add_resources(RCC ${QRC})


if (WIN32)
  if (NOT MSVC)
	message(FATAL_ERROR "Only MSVC is supported on this platform")
  endif ()
  add_definitions(/D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x0600 /DSTATICLIB)
  include_directories(cryptonote/src/platform/msc)

  set(PLATFORM_DIR Windows)
  set(BUILD_PLATFORM WIN32)
  set(BUILD_RESOURCES src/cryptonotewallet.rc)
  set(QTMAIN Qt5::WinMain)

elseif (UNIX)
  set(CRYPTONOTE_SOURCES ${CRYPTONOTE_SOURCES} cryptonote/external/miniupnpc/minissdpc.c)
  if (APPLE)
    enable_language(ASM)
    file(GLOB_RECURSE OBJC_SOURCES "src/*.mm")
    set(SOURCES ${SOURCES} ${OBJC_SOURCES})
    set(PLATFORM_DIR OSX)
    set(MACOSX_BUNDLE_INFO_STRING "Cryptonote GUI wallet")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_VERSION}.${VERSION_MINOR}.${VERSION_PATCH}")
    set(MACOSX_BUNDLE_BUNDLE_NAME CryptonoteWallet)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_VERSION}.${VERSION_MINOR}.${VERSION_PATCH}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "$${VERSION_VERSION}.${VERSION_MINOR}.${VERSION_PATCH}")

    find_package(Qt5PrintSupport REQUIRED)

    include_directories(/usr/include/malloc)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maes -std=c++11 -stdlib=libc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -maes -D_DARWIN_C_SOURCE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework CoreFoundation -framework Carbon -framework IOKit -L/usr/lib")

    set(MACOSX_BUNDLE_ICON_FILE cryptonote.icns)
    set(APPLICATION_ICON src/images/cryptonote.icns)
    set_source_files_properties(${APPLICATION_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(BUILD_PLATFORM MACOSX_BUNDLE)
    set(BUILD_RESOURCES ${APPLICATION_ICON})
	
    GET_TARGET_PROPERTY(QT_LIB_DIR "${Qt5Widgets_LIBRARIES}" LOCATION)
    GET_FILENAME_COMPONENT(QT_LIB_DIR "${QT_LIB_DIR}" PATH)
  else(APPLE)
    set(PLATFORM_DIR Linux)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maes -std=c++11")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maes -std=c11")
  endif (APPLE)
endif ()

include_directories(cryptonote/src/platform/${PLATFORM_DIR})
file(GLOB PLATFORM_SOURCES "cryptonote/src/platform/${PLATFORM_DIR}/system/*")
set(CRYPTONOTE_SOURCES ${CRYPTONOTE_SOURCES} ${PLATFORM_SOURCES})

add_library(${CRYPTONOTE_LIB} STATIC ${CRYPTONOTE_SOURCES})
set_target_properties(${CRYPTONOTE_LIB} PROPERTIES COMPILE_DEFINITIONS _GNU_SOURCE)
target_link_libraries(${CRYPTONOTE_LIB} ${Boost_LIBRARIES})

add_executable(${PROJECT_NAME} ${BUILD_PLATFORM} ${BUILD_RESOURCES} ${SOURCES} ${HEADERS} ${UIS} ${RCC} ${QM_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS _GNU_SOURCE)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${QTMAIN} ${CRYPTONOTE_LIB})

if (APPLE)
  qt5_use_modules(${PROJECT_NAME} PrintSupport)
elseif (UNIX)
  target_link_libraries(${PROJECT_NAME} -lpthread)
elseif (WIN32)
  target_link_libraries(${PROJECT_NAME} Imm32 Iphlpapi Winmm)
endif (APPLE)

qt5_use_modules(${PROJECT_NAME} Widgets Gui)
