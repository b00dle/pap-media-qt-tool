#ifndef WEB_IMAGE_H
#define WEB_IMAGE_H

#include <QObject>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

class WebPixmap : public QObject
{
    Q_OBJECT
public:
    explicit WebPixmap(QObject *parent = nullptr);

    void setUrl(const QUrl& url);
    const QUrl& getUrl() const;

    const QPixmap& getPixmap() const;

protected slots:
    void onManagerFinished(QNetworkReply*);

signals:
    void contentChanged();

public slots:

private:
    void requestPixmap();

    QPixmap pixmap_;
    QNetworkAccessManager* network_manager_;
    QUrl url_;
};

#endif // WEB_IMAGE_H
