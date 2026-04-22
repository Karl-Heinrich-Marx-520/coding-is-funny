#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto* listwidget = new QListWidget(this);

    auto* listwidgetitem = new QListWidgetItem;
    listwidgetitem->setText("listitem");
    QPixmap pixmap(50,50);
    pixmap.fill(Qt::blue);
    listwidgetitem->setIcon(pixmap);
    listwidgetitem->setToolTip("this is list item");
    listwidget->insertItem(1,listwidgetitem);

    auto* listwidgetitem2 = new QListWidgetItem;
    listwidgetitem2->setText("listitem2");
    QPixmap pixmap2(50,50);
    pixmap2.fill(Qt::red);
    listwidgetitem2->setIcon(pixmap2);
    listwidgetitem2->setToolTip("this is list item");
    listwidget->insertItem(2,listwidgetitem2);

    listwidget->show();
    this->setCentralWidget(listwidget);

    _treewidget = new QTreeWidget();
    _treewidget->setColumnCount(2);
    QStringList headers;
    headers << "name" << "year";
    _treewidget->setHeaderLabels(headers);

    auto* grade1 = new QTreeWidgetItem(_treewidget);
    grade1->setText(0, "Grade1");
    auto* grade2 = new QTreeWidgetItem(_treewidget);
    grade2->setText(0, "Grade2");

    QTreeWidgetItem* student1 = new QTreeWidgetItem(grade1);
    student1->setText(0, "Tom");
    student1->setText(1,"1996");

    QTreeWidgetItem* student2 = new QTreeWidgetItem(grade2);
    student2->setText(0, "Joe");
    student2->setText(1,"2000");

    QTreeWidgetItem* student3 = new QTreeWidgetItem(grade2);
    student3->setText(0, "Gres");
    student3->setText(1,"1500");

    _treewidget->show();

    _tableWidget =  new QTableWidget(3,2);
    //创建表格项目，将其插入到表格中
    QTableWidgetItem * tableWidgetItem = new QTableWidgetItem("qt");
    _tableWidget->setItem(1,1,tableWidgetItem);
    //创建表头
    QTableWidgetItem * headerV = new QTableWidgetItem("first");
    _tableWidget->setVerticalHeaderItem(0, headerV);
    QTableWidgetItem * headerH = new QTableWidgetItem("ID");
    _tableWidget->setHorizontalHeaderItem(0, headerH);
    _tableWidget->show();

    listwidget->setSelectionMode(QAbstractItemView::SingleSelection);
    //启用拖动
    listwidget->setDragEnabled(true);
    //设置接受拖放
    listwidget->viewport()->setAcceptDrops(true);
    //设置显示将要放置的位置
    listwidget->setDropIndicatorShown(true);
    //设置拖放模式为移动项目，如果不设置，则为复制项目
    listwidget->setDragDropMode(QAbstractItemView::InternalMove);
}

MainWindow::~MainWindow()
{
    delete ui;
}
