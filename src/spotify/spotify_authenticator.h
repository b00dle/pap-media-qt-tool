#ifndef SPOTIFY_AUTHENTICATOR_H
#define SPOTIFY_AUTHENTICATOR_H

#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>

class SpotifyAuthenticator : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyAuthenticator(QObject *parent = nullptr);
    virtual ~SpotifyAuthenticator();

    QString getToken() const;
    void grant();

signals:
    void tokenChanged(const QString &token);
    void accessGranted();
    void authorizeWithBrowser(const QUrl &url);

public slots:
    void onAccessGranted();

private:
    void init();
    void loadCredentials();

    QString client_id_;
    QString client_secret_;
    QOAuth2AuthorizationCodeFlow spotify_;
    QString token_;
    bool initialized_;
};

#endif // SPOTIFY_AUTHENTICATOR_H
