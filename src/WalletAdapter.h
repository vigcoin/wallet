// Copyright (c) 2011-2015 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QMutex>
#include <QObject>
#include <QTimer>

#include <atomic>
#include <fstream>

#include <IWalletLegacy.h>

namespace WalletGui {

class WalletAdapter : public QObject, public cryptonote::IWalletLegacyObserver {
  Q_OBJECT
  Q_DISABLE_COPY(WalletAdapter)

public:
  static WalletAdapter& instance();

  void open(const QString& _password);
  void close();
  bool save(bool _details, bool _cache);
  void backup(const QString& _file);

  QString getAddress() const;
  quint64 getActualBalance() const;
  quint64 getPendingBalance() const;
  quint64 getTransactionCount() const;
  quint64 getTransferCount() const;
  bool getTransaction(cryptonote::TransactionId& _id, cryptonote::WalletLegacyTransaction& _transaction);
  bool getTransfer(cryptonote::TransferId& _id, cryptonote::WalletLegacyTransfer& _transfer);
  bool isOpen() const;
  void sendTransaction(const QVector<cryptonote::WalletLegacyTransfer>& _transfers, quint64 _fee, const QString& _payment_id, quint64 _mixin);
  bool changePassword(const QString& _old_pass, const QString& _new_pass);
  void setWalletFile(const QString& _path);

  void initCompleted(std::error_code _result) Q_DECL_OVERRIDE;
  void saveCompleted(std::error_code _result) Q_DECL_OVERRIDE;
  void synchronizationProgressUpdated(uint32_t _current, uint32_t _total) Q_DECL_OVERRIDE;
  void synchronizationCompleted(std::error_code _error) Q_DECL_OVERRIDE;
  void actualBalanceUpdated(uint64_t _actual_balance) Q_DECL_OVERRIDE;
  void pendingBalanceUpdated(uint64_t _pending_balance) Q_DECL_OVERRIDE;
  void externalTransactionCreated(cryptonote::TransactionId _transaction_id) Q_DECL_OVERRIDE;
  void sendTransactionCompleted(cryptonote::TransactionId _transaction_id, std::error_code _result) Q_DECL_OVERRIDE;
  void transactionUpdated(cryptonote::TransactionId _transaction_id) Q_DECL_OVERRIDE;

private:
  std::fstream m_file;
  cryptonote::IWalletLegacy* m_wallet;
  QMutex m_mutex;
  std::atomic<bool> m_isBackupInProgress;
  std::atomic<bool> m_isSynchronized;
  std::atomic<quint64> m_lastWalletTransactionId;
  QTimer m_newTransactionsNotificationTimer;

  WalletAdapter();
  ~WalletAdapter();

  void onWalletInitCompleted(int _error, const QString& _error_text);
  void onWalletSendTransactionCompleted(cryptonote::TransactionId _transaction_id, int _error, const QString& _error_text);

  bool importLegacyWallet(const QString &_password);
  bool save(const QString& _file, bool _details, bool _cache);
  void lock();
  void unlock();
  bool openFile(const QString& _file, bool _read_only);
  void closeFile();
  void notifyAboutLastTransaction();

  static void renameFile(const QString& _old_name, const QString& _new_name);
  Q_SLOT void updateBlockStatusText();
  Q_SLOT void updateBlockStatusTextWithDelay();

Q_SIGNALS:
  void walletInitCompletedSignal(int _error, const QString& _error_text);
  void walletCloseCompletedSignal();
  void walletSaveCompletedSignal(int _error, const QString& _error_text);
  void walletSynchronizationProgressUpdatedSignal(quint64 _current, quint64 _total);
  void walletSynchronizationCompletedSignal(int _error, const QString& _error_text);
  void walletActualBalanceUpdatedSignal(quint64 _actual_balance);
  void walletPendingBalanceUpdatedSignal(quint64 _pending_balance);
  void walletTransactionCreatedSignal(cryptonote::TransactionId _transaction_id);
  void walletSendTransactionCompletedSignal(cryptonote::TransactionId _transaction_id, int _error, const QString& _error_text);
  void walletTransactionUpdatedSignal(cryptonote::TransactionId _transaction_id);
  void walletStateChangedSignal(const QString &_state_text);

  void openWalletWithPasswordSignal(bool _error);
  void changeWalletPasswordSignal();
  void updateWalletAddressSignal(const QString& _address);
  void reloadWalletTransactionsSignal();
  void updateBlockStatusTextSignal();
  void updateBlockStatusTextWithDelaySignal();
};



}
