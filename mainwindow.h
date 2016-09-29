#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
     void download();
     void onDownloadFinished(QNetworkReply *);
signals:
     void downloadProgress(qint64, qint64);
     void available(bool);
     void running(bool);
private:
    Ui::MainWindow *ui;
    QUrl api_key = QUrl{"&key="};
    QJsonArray AddressData;
    QNetworkAccessManager *naManager;
    void saveFile(QNetworkReply *);
    QString postcode {""}; // Initialize to surpress url error
    QString address {""};
    QMessageBox error_report;

};

#endif // MAINWINDOW_H
