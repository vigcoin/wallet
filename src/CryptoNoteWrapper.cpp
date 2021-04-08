// Copyright (c) 2011-2015 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "CryptoNoteWrapper.h"
#include "common/binary.h"
#include "config/common.h"
#include "cryptonote/core/CryptoNoteFormatUtils.h"
#include "cryptonote/core/currency.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include "command_line/CoreConfig.h"
#include "command_line/NetNodeConfig.h"
#include "cryptonote/core/core.h"
#include "cryptonote/protocol/handler.h"
#include "cryptonote/core/blockchain.h"
#include "cryptonote/structures/block_entry.h"
#include "InProcessNode/InProcessNode.h"
#include "p2p/NetNode.h"
#include "wallet_legacy/WalletLegacy.h"
#include "logging/LoggerManager.h"
#include "system/Dispatcher.h"

namespace WalletGui {

namespace {

bool parsePaymentId(const std::string& payment_id_str, hash_t& payment_id) {
  return cryptonote::parsePaymentId(payment_id_str, payment_id);
}

std::string convertPaymentId(const std::string& paymentIdString) {
  if (paymentIdString.empty()) {
    return "";
  }

  hash_t paymentId;
  if (!parsePaymentId(paymentIdString, paymentId)) {
    std::stringstream errorStr;
    errorStr << "Payment id has invalid format: \"" + paymentIdString + "\", expected 64-character string";
    throw std::runtime_error(errorStr.str());
  }

  std::vector<uint8_t> extra;
  binary_array_t extraNonce;
  cryptonote::setPaymentIdToTransactionExtraNonce(extraNonce, paymentId);
  if (!cryptonote::addExtraNonceToTransactionExtra(extra, extraNonce)) {
    std::stringstream errorStr;
    errorStr << "Something went wrong with payment_id. Please check its format: \"" + paymentIdString + "\", expected 64-character string";
    throw std::runtime_error(errorStr.str());
  }

  return std::string(extra.begin(), extra.end());
}

std::string extractPaymentId(const std::string& extra) {
  std::vector<transaction_extra_t> extraFields;
  std::vector<uint8_t> extraVector;
  std::copy(extra.begin(), extra.end(), std::back_inserter(extraVector));

  if (!cryptonote::parseTransactionExtra(extraVector, extraFields)) {
    throw std::runtime_error("Can't parse extra");
  }

  std::string result;
  cryptonote::transaction_extra_nonce_t extraNonce;
  if (cryptonote::findTransactionExtraFieldByType(extraFields, extraNonce)) {
    hash_t paymentIdHash;
    if (cryptonote::getPaymentIdFromTransactionExtraNonce(extraNonce.nonce, paymentIdHash)) {
      unsigned char* buff = reinterpret_cast<unsigned char *>(&paymentIdHash);
      for (size_t i = 0; i < sizeof(paymentIdHash); ++i) {
        result.push_back("0123456789ABCDEF"[buff[i] >> 4]);
        result.push_back("0123456789ABCDEF"[buff[i] & 15]);
      }
    }
  }

  return result;
}


}

Node::~Node() {
}

class RpcNode : cryptonote::INodeObserver, public Node {
public:
  RpcNode(const cryptonote::Currency& currency, INodeCallback& callback, const std::string& nodeHost, unsigned short nodePort) :
    m_callback(callback),
    m_currency(currency),
    m_node(nodeHost, nodePort) {
    m_node.addObserver(this);
  }

  ~RpcNode() override {
  }

  void init(const std::function<void(std::error_code)>& callback) override {
    m_node.init(callback);
  }

  void deinit() override {
  }

  std::string convertPaymentId(const std::string& paymentIdString) override {
    return WalletGui::convertPaymentId(paymentIdString);
  }

  std::string extractPaymentId(const std::string& extra) override {
    return WalletGui::extractPaymentId(extra);
  }

  uint64_t getLastKnownBlockHeight() const override {
    return m_node.getLastKnownBlockHeight();
  }

  uint64_t getLastLocalBlockHeight() const override {
    return m_node.getLastLocalBlockHeight();
  }

  uint64_t getLastLocalBlockTimestamp() const override {
    return m_node.getLastLocalBlockTimestamp();
  }

  uint64_t getPeerCount() const override {
    return m_node.getPeerCount();
  }

  cryptonote::IWalletLegacy* createWallet() override {
    return new cryptonote::WalletLegacy(m_currency, m_node);
  }

private:
  INodeCallback& m_callback;
  const cryptonote::Currency& m_currency;
  cryptonote::NodeRpcProxy m_node;

  void peerCountUpdated(size_t count) {
    m_callback.peerCountUpdated(*this, count);
  }

  void localBlockchainUpdated(uint64_t height) {
    m_callback.localBlockchainUpdated(*this, height);
  }

  void lastKnownBlockHeightUpdated(uint64_t height) {
    m_callback.lastKnownBlockHeightUpdated(*this, height);
  }
};

class InprocessNode : cryptonote::INodeObserver, public Node {
public:
  InprocessNode(const cryptonote::Currency& currency, Logging::LoggerManager& logManager, const cryptonote::CoreConfig& coreConfig,
    const cryptonote::NetNodeConfig& netNodeConfig, INodeCallback& callback) :
    m_currency(currency), m_dispatcher(),
    m_callback(callback),
    m_netNodeConfig(netNodeConfig),
    m_protocolHandler(currency, m_dispatcher, m_core, nullptr, logManager),
    m_core(currency, &m_protocolHandler, logManager),
    m_nodeServer(m_dispatcher, m_protocolHandler, logManager),
    m_node(m_core, m_protocolHandler) {

    m_core.set_cryptonote_protocol(&m_protocolHandler);
    m_protocolHandler.set_p2p_endpoint(&m_nodeServer);
    cryptonote::Checkpoints checkpoints(logManager);
    for (const config::checkpoint_data_t & checkpoint : config::get().checkpoints) {
      checkpoints.add(checkpoint.height, checkpoint.blockId);
    }
  }

  ~InprocessNode() override {

  }

  void init(const std::function<void(std::error_code)>& callback) override {
    try {
      if (!m_core.init(cryptonote::MinerConfig(), true)) {
        callback(make_error_code(cryptonote::error::NOT_INITIALIZED));
        return;
      }

      if (!m_nodeServer.init(m_netNodeConfig)) {
        callback(make_error_code(cryptonote::error::NOT_INITIALIZED));
        return;
      }
    } catch (std::runtime_error& _err) {
      callback(make_error_code(cryptonote::error::NOT_INITIALIZED));
      return;
    }

    m_node.init([this, callback](std::error_code ec) {
      m_node.addObserver(this);
      callback(ec);
    });

    m_nodeServer.run();
    m_nodeServer.deinit();
    m_node.shutdown();
  }

  void deinit() override {
    m_nodeServer.sendStopSignal();
  }

  std::string convertPaymentId(const std::string& paymentIdString) override {
    return WalletGui::convertPaymentId(paymentIdString);
  }

  std::string extractPaymentId(const std::string& extra) override {
    return WalletGui::extractPaymentId(extra);
  }

  uint64_t getLastKnownBlockHeight() const override {
    return m_node.getLastKnownBlockHeight();
  }

  uint64_t getLastLocalBlockHeight() const override {
    return m_node.getLastLocalBlockHeight();
  }

  uint64_t getLastLocalBlockTimestamp() const override {
    return m_node.getLastLocalBlockTimestamp();
  }

  uint64_t getPeerCount() const override {
    return m_node.getPeerCount();
  }

  cryptonote::IWalletLegacy* createWallet() override {
    return new cryptonote::WalletLegacy(m_currency, m_node);
  }

private:
  INodeCallback& m_callback;
  const cryptonote::Currency& m_currency;
  System::Dispatcher m_dispatcher;
  cryptonote::NetNodeConfig m_netNodeConfig;
  cryptonote::core m_core;
  cryptonote::CryptoNoteProtocolHandler m_protocolHandler;
  cryptonote::NodeServer m_nodeServer;
  cryptonote::InProcessNode m_node;
  std::future<bool> m_nodeServerFuture;

  void peerCountUpdated(size_t count) {
    m_callback.peerCountUpdated(*this, count);
  }

  void localBlockchainUpdated(uint64_t height) {
    m_callback.localBlockchainUpdated(*this, height);
  }

  void lastKnownBlockHeightUpdated(uint64_t height) {
    m_callback.lastKnownBlockHeightUpdated(*this, height);
  }
};

Node* createRpcNode(const cryptonote::Currency& currency, INodeCallback& callback, const std::string& nodeHost, unsigned short nodePort) {
  return new RpcNode(currency, callback, nodeHost, nodePort);
}

Node* createInprocessNode(const cryptonote::Currency& currency, Logging::LoggerManager& logManager,
  const cryptonote::CoreConfig& coreConfig, const cryptonote::NetNodeConfig& netNodeConfig, INodeCallback& callback) {
  return new InprocessNode(currency, logManager, coreConfig, netNodeConfig, callback);
}

}
