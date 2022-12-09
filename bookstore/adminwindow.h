#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QtSql>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();
    void init();
    void udpateReport();

private slots:
    void on_pushButton_remove_clicked();

    void on_pushButton_add_clicked();

    void on_radioButton_new_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::AdminWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *qmodel;
    QSqlQueryModel *gmodel;
    QSqlQueryModel *amodel;
};

#endif // ADMINWINDOW_H
