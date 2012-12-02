#include "network/operationsmanager.h"
#include "share/commontools.h"
#include "core/driveengine.h"
#include "share/debug.h"
#include <QStringList>

OperationsManager::OperationsManager(QObject *parent):
    NetworkManager(parent)
{
}

void OperationsManager::deleteFile(const QString &url)
{
    CommonTools::setHeader(request);
    request.setRawHeader("If-Match", "*");

    init();

    request.setUrl(getDeleteFileQuery(url));

    DEBUG << getDeleteFileQuery(url);

    reply = networkManager->deleteResource(request);

    connect(networkManager.data(), SIGNAL(finished(QNetworkReply*)),this, SLOT(slotReplyFinished(QNetworkReply*)));
    connectErrorHandlers();
}

void OperationsManager::copyWebFile(const ItemInfo::Data &source, const QString &destFolder)
{    
    init();

    QString data = QString("{\"kind\": \"drive#file\", \"title\": \"%1\",\"parents\": [{\"id\":\"%2\"}]}").arg(source.name).arg(getIDFromURL(destFolder));

    postData = data.toLatin1();

    CommonTools::setHeader(request);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(postData.size()));

    QString copyLink(COPY_FILE_FIRST_QUERY_PART + getIDFromURL(source.self) + COPY_FILE_LAST_QUERY_PART);

    DEBUG << "copyLink" << copyLink;

    request.setUrl(QUrl(copyLink));

    reply = networkManager->post(request, postData);

    connect(networkManager.data(), SIGNAL(finished(QNetworkReply*)), this, SLOT(slotPostFinished(QNetworkReply*)));
    connectErrorHandlers();
}

void OperationsManager::setStartSettings(QUrl url, const QString &fileName, const QString &progressBarDialogInfoText)
{
    Q_UNUSED(url);
    Q_UNUSED(fileName);
    Q_UNUSED(progressBarDialogInfoText);
}

void OperationsManager::createFolder(const QString &folderUrl, const QString &name)
{
    init();

    QString data = QString("{\"title\": \"%1\",\"parents\": [{\"id\": \"%2\"}],\"mimeType\": \"application/vnd.google-apps.folder\"}").arg(name).arg(getIDFromURL(folderUrl));

    postData = data.toLatin1();

    CommonTools::setHeader(request);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(postData.size()));

    request.setUrl(QUrl("https://www.googleapis.com/drive/v2/files"));

    reply = networkManager->post(request, postData);

    connect(networkManager.data(), SIGNAL(finished(QNetworkReply*)), this, SLOT(slotPostFinished(QNetworkReply*)));
    connectErrorHandlers();
}

QUrl OperationsManager::getDeleteFileQuery(const QString &url)
{
    return QUrl(QString(DELETE_FILE += getIDFromURL(url)));
}

QUrl OperationsManager::getCopyFileQuery(const QString &url)
{
    return QUrl(QString(COPY_FILE += getIDFromURL(url)));
}

QString OperationsManager::getIDFromURL(const QString &url)
{
    int backParamNum = 1;

    if(url.contains(QRegExp("max-results")))
    {
        backParamNum = 2;
    }

    QStringList queryStrs(url.split("/"));
    QString lastParam(queryStrs[queryStrs.count()  - backParamNum]);

    queryStrs = lastParam.split("%3A");

    return queryStrs[queryStrs.count()  - 1];
}

void OperationsManager::slotReplyFinished(QNetworkReply *reply)
{
    Q_UNUSED(reply);
    emit signalDelFinished();
}

void OperationsManager::slotPostFinished(QNetworkReply* reply)
{
    NetworkManager::slotPostFinished(reply);
    SDriveEngine::inst()->getFilesMngr()->get(SDriveEngine::inst()->getFilesMngr()->getUpLevelFolderLink());
}

