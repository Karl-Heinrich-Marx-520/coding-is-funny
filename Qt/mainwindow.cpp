#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWizard>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _counter(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_textBtn_clicked()
{
    QString path = QDir::currentPath();
    QString title = tr("Text Dialog");
    QString filter = tr("Text(*.txt);;photo(*.jpg *.gif *.png);;ALL(*.*)");
    QString aFileName = QFileDialog::getOpenFileName(this, title, path, filter);
    qDebug() << aFileName << Qt::endl;
}


void MainWindow::on_colorBtn_clicked()
{
    QColorDialog colorDlg(Qt::blue, this);
    colorDlg.setOption(QColorDialog::ShowAlphaChannel);
    colorDlg.exec();
    QColor color = colorDlg.currentColor();
    qDebug() << "color is " << color;
}


void MainWindow::on_NumberBtn_clicked()
{
    bool ok = false;
    auto intdate = QInputDialog::getInt(this, tr("Number Input Dialog"), tr("please"), 200, -200, 400, 10, &ok);
    if(ok){
        qDebug() << intdate << Qt::endl;
    }
}


void MainWindow::on_pushButton_clicked()
{
    bool ok = false;
    auto intdate = QInputDialog::getDouble(this, tr("Number Input Dialog"), tr("please"), 0.1, -2, 4, 2, &ok);
    if(ok){
        qDebug() << intdate << Qt::endl;
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    QStringList items;
    items << tr("条目1") << tr("条目2");
    bool ok = false;
    auto itemDate =  QInputDialog::getItem(this, tr("条目Dialog"), tr("Input or Select"), items, 0, true, &ok);
    if(ok){
        qDebug() << "items is" << itemDate << Qt::endl;
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    auto ret = QMessageBox::question(this, tr("提问对话框"), tr("are you singal?"), QMessageBox::Yes, QMessageBox::No);
    if(ret == QMessageBox::Yes){
        qDebug() << "Hello singal DOG" << Qt::endl;
    }
    else{
        return ;
    }
    auto ret2 = QMessageBox::information(this, tr("info Dialog"), tr("Hello"), QMessageBox::Yes);
    if(ret2 == QMessageBox::Yes){
        qDebug() << "ret2 is" << ret2;
    }
    else{
        return ;
    }
}


void MainWindow::on_pushButton_4_clicked()
{
//notice Dialog
}


void MainWindow::on_pushButton_5_clicked()
{
    _progressDialog =  new QProgressDialog(tr("正在复制"), tr("取消复制"), 0, 500, this);
    _progressDialog->setWindowTitle(tr("文件复制Dialog"));
    _progressDialog->setWindowModality(Qt::ApplicationModal);
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &MainWindow::on_updateProgressDialog);
    connect(_progressDialog, &QProgressDialog::canceled, this, &MainWindow::on_canceledProgressDialog);
    _timer->start(20);
}

void MainWindow::on_updateProgressDialog()
{
    _counter++;
    if(_counter > 500){
        _timer->stop();
        delete _timer;
        _timer = nullptr;
        delete _progressDialog;
        _counter = 0;
        return;
    }
    _progressDialog->setValue(_counter);

}

void MainWindow::on_canceledProgressDialog()
{
    _timer->stop();
    delete _timer;
    _timer = nullptr;
    delete _progressDialog;
    _progressDialog = nullptr;
    _counter = 0;
    return ;
}


void MainWindow::on_pushButton_6_clicked()
{
    QWizard wiza(this);
    wiza.setWindowTitle(tr("全城热恋"));
    auto page1 = new QWizardPage();
    page1->setTitle(tr("婚恋介绍引导程序"));
    auto label1 = new QLabel();
    label1->setText(tr("帮你找到人生伴侣"));
    auto layout = new QVBoxLayout();
    layout->addWidget(label1);
    page1->setLayout(layout);
    wiza.addPage(page1);

    QWizardPage* page2 = new QWizardPage();
    page2->setTitle(tr("选择你的心动类型"));
    auto group = new QButtonGroup(page2);
    auto btn1 = new QRadioButton();
    group->addButton(btn1);
    btn1->setText(tr("白富美"));
    auto btn2 = new QRadioButton();
    group->addButton(btn2);
    btn1->setText(tr("萝莉"));
    auto btn3 = new QRadioButton();
    group->addButton(btn3);
    btn1->setText(tr("御姐"));
    auto btn4 = new QRadioButton();
    btn1->setText(tr("小家碧玉"));
    group->addButton(btn4);

    auto layout2 = new QVBoxLayout();
    for(int i = 0; i < group->buttons().size(); ++i){
        layout2->addWidget(group->buttons()[i]);
    }
    page2->setLayout(layout2);
    wiza.addPage(page2);

    wiza.show();
    wiza.exec();

}

