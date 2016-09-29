#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    naManager = new QNetworkAccessManager();
    connect(naManager, &QNetworkAccessManager::finished, this, &MainWindow::onDownloadFinished);

    connect (ui->key_api, &QLineEdit::editingFinished, [this] {
        api_key = QUrl("&key="+ui->key_api->text()); // Append the inserted api key to the end of URL
    });
    connect (ui->postcode, &QLineEdit::editingFinished, [this] {
        postcode = QString(ui->postcode->text()); // Append the postcode to url in order to get back correct data
    });

    connect(ui->quit_b, &QPushButton::clicked, this, &QApplication::quit);
    connect(ui->go_b, &QPushButton::clicked, this, &MainWindow::download);
    //below assigns custom signal
    //connect(this, &MainWindow::url_assigned, this, &MainWindow::onApiAssigned);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete naManager;
}

void MainWindow::onDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        error_report.setText(reply->errorString());
        error_report.setIcon(QMessageBox::Critical);
        error_report.exec();
    } else{
        saveFile(reply);
    }
    reply->deleteLater();
    emit running(false);
    emit available(true);
}

// This function takes the URL as an argument and downloads the content
// of URL.
void MainWindow::download()
{
    QUrl url = QUrl{"https://maps.googleapis.com/maps/api/geocode/json?address="};
    QString tmp;
    tmp = url.toString() + postcode + api_key.toString();
    qDebug() << tmp;
    url = QUrl(tmp);
    QNetworkRequest req(url);
    QNetworkReply * reply = naManager->get(req);
    // Clear the URL and tmp vars so we dont append api_key and postcode for each request
    tmp.clear();
    url.clear();
    emit running(true);
    emit available(false);

}

void MainWindow::saveFile(QNetworkReply *reply)
{
    QByteArray tmp = {reply->readAll()};
    QJsonDocument addr_json(QJsonDocument::fromJson(tmp));
    QJsonObject json_obj {addr_json.object()};
    QJsonValue results {json_obj.value("results")};

    // Check API Key and Postcode entry is in correct format!
    if (results.isNull() || results.toArray().isEmpty()){
        if (json_obj.value("status").toString() == "REQUEST_DENIED")
        {
            error_report.setText("Invalid Google Maps API Key!");
        } else if (json_obj.value("status").toString() == "ZERO_RESULTS")
        {
            error_report.setText("Invalid Postcode entry!");
        } else {
            error_report.setText("Unknown Error!");
            error_report.setInformativeText(results.toObject().value("status").toString());
        }
        error_report.setIcon(QMessageBox::Critical);
        error_report.exec();
    }
    else {
        QJsonArray iter_json = results.toArray();
        int tmp_counter = 0; // to count the json nodes
        foreach(const QJsonValue &item, iter_json)
        {
            QJsonArray address_components = item.toObject().value("address_components").toArray();
            foreach (const QJsonValue & attr, address_components)
            {
                if (tmp_counter == 1)
                {
                    ui->streetname->setText(attr.toObject().value("long_name").toString());
                    break;
                }
               tmp_counter +=1;
            }
        }
     }
}


