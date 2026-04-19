#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_textBtn_clicked();

    void on_colorBtn_clicked();

    void on_NumberBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_updateProgressDialog();

    void on_canceledProgressDialog();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QProgressDialog* _progressDialog;
    QTimer* _timer;
    int _counter;
};
#endif // MAINWINDOW_H
