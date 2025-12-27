#include <QDebug>

#include "keychainclass.h"

static const QString service = "keychain.example.project.app";

// fix for https://github.com/frankosterfeld/qtkeychain/issues/288

KeyChainClass::KeyChainClass(QObject *parent)
    : QObject(parent)
      // m_readCredentialJob(QLatin1String("keychain.example.project.app")),
      // m_writeCredentialJob(QLatin1String("keychain.example.project.app")),
      // m_deleteCredentialJob(QLatin1String("keychain.example.project.app"))
{
    // m_readCredentialJob.setAutoDelete(false);
    // m_writeCredentialJob.setAutoDelete(false);
    // m_deleteCredentialJob.setAutoDelete(false);
}

void KeyChainClass::readKey(const QString &key)
{
    auto readCredentialJob = new QKeychain::ReadPasswordJob(service);
    readCredentialJob->setAutoDelete(true);
    readCredentialJob->setKey(key);

    QObject::connect(readCredentialJob, &QKeychain::ReadPasswordJob::finished, this,
                     [this, key](QKeychain::Job *job) {
                         auto j = static_cast<QKeychain::ReadPasswordJob*>(job);
                         if (j->error() == QKeychain::NoError) {
                             emit keyRestored(key, j->textData());
                         } else {
                             emit error(tr("Read key failed: %1").arg(qPrintable(j->errorString())));
                         }
                         // no delete needed, autoDelete takes care of it
                     });

    // // Debugging: verify cleanup
    // QObject::connect(readCredentialJob, &QObject::destroyed, this, [=]() {
    //     qInfo() << "QObject deleted for read key:" << key;
    // });
    // QObject::connect(readCredentialJob, &QKeychain::ReadPasswordJob::destroyed, this, [=]() {
    //     qInfo() << "readCredentialJob deleted for key:" << key;
    // });

    readCredentialJob->start();
}

void KeyChainClass::writeKey(const QString &key, const QString &value)
{
    auto writeCredentialJob = new QKeychain::WritePasswordJob(service);
    writeCredentialJob->setAutoDelete(true);
    writeCredentialJob->setKey(key);
    writeCredentialJob->setTextData(value);

    QObject::connect(writeCredentialJob, &QKeychain::WritePasswordJob::finished, this,
                     [this, key](QKeychain::Job *job) {
                         auto j = static_cast<QKeychain::WritePasswordJob*>(job);
                         if (j->error() == QKeychain::NoError) {
                             emit keyStored(key);
                         } else {
                             emit error(tr("Write key failed: %1").arg(qPrintable(j->errorString())));
                         }
                     });

    // // Debugging: verify cleanup
    // QObject::connect(writeCredentialJob, &QObject::destroyed, this, [=]() {
    //     qInfo() << "QObject deleted for write key:" << key;
    // });
    // QObject::connect(writeCredentialJob, &QKeychain::WritePasswordJob::destroyed, this, [=]() {
    //     qInfo() << "writeCredentialJob deleted for key:" << key;
    // });

    writeCredentialJob->start();
}

void KeyChainClass::deleteKey(const QString &key)
{
    auto deleteCredentialJob = new QKeychain::DeletePasswordJob(service);
    deleteCredentialJob->setAutoDelete(true);
    deleteCredentialJob->setKey(key);

    QObject::connect(deleteCredentialJob, &QKeychain::DeletePasswordJob::finished, this,
                     [this, key](QKeychain::Job *job) {
                         auto j = static_cast<QKeychain::DeletePasswordJob*>(job);
                         if (j->error() == QKeychain::NoError) {
                             emit keyDeleted(key);
                         } else {
                             emit error(tr("Delete key failed: %1").arg(qPrintable(j->errorString())));
                         }
                     });

    // // Debugging: verify cleanup
    // QObject::connect(deleteCredentialJob, &QObject::destroyed, this, [=]() {
    //     qInfo() << "QObject deleted for delete key:" << key;
    // });
    // QObject::connect(deleteCredentialJob, &QKeychain::DeletePasswordJob::destroyed, this, [=]() {
    //     qInfo() << "deleteCredentialJob deleted for key:" << key;
    // });

    deleteCredentialJob->start();
}
