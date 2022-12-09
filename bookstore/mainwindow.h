#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QtSql>
#include <QSpinBox>
#include "adminwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void listAllBooks();
    void updateCart();
    void updateTotal(); // shopping cart

private slots:
    void on_pushButton_admin_login_clicked();

    void on_pushButton_user_login_clicked();

    void on_pushButton_register_clicked();

    void on_pushButton_clicked();

    void on_pushButton_track_clicked();

    void on_pushButton_sort_clicked();

    //void tableIndexChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_pushButton_addToCart_clicked();

    void spinBox_valueChanged(int value);


    void on_pushButton_order_clicked();

private:
    Ui::MainWindow *ui;
    AdminWindow *adminUI;
    QSqlQueryModel *qmodel; // books table
    QSqlQueryModel *trackmodel; // tracking table
    QSqlDatabase db;
    QString current_user="";
    QMap<QString, QPair<QString, QString>> cart_map; // a map of {ISBN : (book, price)} for populating shopping cart
    QMap<QString, int> cart_amount; // {isbn: order_amount}
    bool DBInit();
    bool DBInsertDummyData();
    bool createView();
    bool createTrigger();
};
#endif // MAINWINDOW_H
