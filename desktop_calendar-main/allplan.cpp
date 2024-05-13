#include "allplan.h"
#include "ui_allplan.h"
#include <QVector>
#include "plan.h"
#include<QStandardItemModel>
#include<QDebug>
#include<QComboBox>
#include<QLabel>
Allplan::Allplan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Allplan)
{
    ui->setupUi(this);
    ui->themeSelector->addItem("黑夜");
    ui->themeSelector->addItem("白昼");
    ui->themeSelector->addItem("黄昏");
    ui->themeSelector->addItem("少女");
    ui->themeSelector->addItem("湛蓝");
    ui->themeSelector->addItem("自然");
    // 连接下拉框的信号到槽函数
        connect(ui->themeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &Allplan::changeTheme);
        connect(ui->search_Button, &QPushButton::clicked, this,&Allplan::search_Button_click);
        // 应用默认主题（黑夜）
        changeTheme(0);
        ui->search_lable->setStyleSheet("QLabel { color: red; }");

    resize(1200,800);

}

Allplan::~Allplan()
{
    delete ui;
}

void Allplan::search_Button_click() {
    QString searchText = ui->lineEdit->text();  // 获取搜索框中的文本
    if (searchText.isEmpty()) {
        displays_plans( this_plans, this_sortplants);
        return;  // 如果搜索框为空，则不执行任何操作
    }

    // 创建一个新的表格模型
    QStandardItemModel *model = new QStandardItemModel(0, 4, this);
    model->setHorizontalHeaderLabels(QStringList() << "Time" << "Information" << "Location" << "Title");

    // 遍历排序后的计划数组
    int j = 0;  // 行计数器
    for (int id : this_sortplants) {
        plan myplan = this_plans[id - 1]; // 假设ID从1开始，所以减1获取索引
         //qDebug()<<searchText;
        if (myplan.delete_mask == 0 && myplan.title.contains(searchText, Qt::CaseInsensitive)) {
            // 如果标题包含搜索文本，并且该计划未被标记为删除
            model->setItem(j, 0, new QStandardItem(myplan.time.toString()));
            model->setItem(j, 1, new QStandardItem(myplan.information));
            model->setItem(j, 2, new QStandardItem(myplan.location));
            model->setItem(j, 3, new QStandardItem(myplan.title));
            j++;  // 增加行数

        }
    }

    // 将模型设置给表格视图
    ui->plan_tableView->setModel(model);
    ui->plan_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->plan_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->plan_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->plan_tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->plan_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void Allplan::displays_plans(QVector<plan> plans, QVector<int> &sortplants){
    // 判断是否有计划
    this_plans = plans;
    this_sortplants = sortplants;
    if (sortplants.empty()) {
        // 如果没有计划，显示"当前还有没要完成的日程"
        QStandardItemModel * model = new QStandardItemModel(1, 1, this);
        model->setItem(0, 0, new QStandardItem("当前没有日程"));
         ui->plan_tableView->setModel(model);
    }
    else{
       // 创建一个表格模型
        int row=0;
          for (int i = 0; i < sortplants.size(); ++i) {
              int id = sortplants[i];
              plan myplan = plans[id - 1]; // id 从 1 开始，所以需要减去 1 获取正确的索引
              if(myplan.delete_mask==0){
                  row++;
              }
          }
       QStandardItemModel *model = new QStandardItemModel(row, 4, this);

       // 设置表头
       model->setHorizontalHeaderLabels(QStringList() << "Time" << "Information" << "Location" << "Title");

       // 逐个将计划添加到表格中
       int j=0;
       for (int i = 0; i < sortplants.size(); ++i) {
           int id = sortplants[i];
           plan myplan = plans[id - 1]; // id 从 1 开始，所以需要减去 1 获取正确的索引
           if(myplan.delete_mask==0){
           model->setItem(j, 0, new QStandardItem(myplan.time.toString()));
           model->setItem(j, 1, new QStandardItem(myplan.information));
           model->setItem(j, 2, new QStandardItem(myplan.location));
           model->setItem(j, 3, new QStandardItem(myplan.title));
           j++;
           }
       }

       // 将模型设置给表格视图
       ui->plan_tableView->setModel(model);
       // 设置表格视图的编辑模式为只读
       ui->plan_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
       // 设置表头大小策略
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // 时间列固定大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 信息列自动适应内容大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 地点列自动适应内容大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // 标题列自动适应内容大小
}

}




void Allplan::changeTheme(int index) {
    QString styleSheet;
    switch (index) {
        case 0: // 黑夜
            styleSheet = "\
            QMainWindow { \
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                                  stop:0 #333333, stop:1 #000000); \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                            stop:0 #555753, stop:1 #2e3436); \
                color: #FFFFFF; \
            } \
            QTableView { \
                background-color: #191919; \
                color: #E0E0E0; \
                border: 1px solid #333; \
            } \
            QHeaderView::section { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #616161, stop:0.5 #505050, \
                                                  stop:0.6 #434343, stop:1 #656565); \
                color: white; \
                padding-left: 4px; \
                border: 1px solid #6c6c6c; \
            } \
            QHeaderView { \
                background-color: transparent; \
            } \
            \
            QAbstractItemView { \
                selection-background-color: #555; \
                selection-color: #FFF; \
            }";
            break;

        case 1: // 白昼
            styleSheet = "\
            QMainWindow { \
                background-color: #FFFFFF; \
                color: #333; \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background-color: #FFFFFF; \
                color: #333; \
            } \
            QTableView { \
                background-color: #FFF; \
                color: #000; \
                border: 1px solid #DDD; \
                gridline-color: #EEE; \
            } \
            QHeaderView::section { \
                background-color: #EEE; \
                color: #333; \
                padding: 5px; \
                border: 1px solid #DDD; \
            } \
            QHeaderView { \
                background-color: #DDD; \
            } \
            QAbstractItemView { \
                selection-background-color: #CDE; \
                selection-color: #000; \
            }";
            break;

        case 2: // 黄昏
            styleSheet = "\
            QMainWindow { \
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                                  stop:0 #FFDAB9, stop:1 #FFA07A); \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                            stop:0 #FFDAB9, stop:1 #FFA07A); \
                color: #000; \
            } \
            QTableView { \
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                                  stop:0 #FFEFD5, stop:1 #FFDAB9); \
                color: #000; \
                border: 1px solid #FA8072; \
            } \
            QHeaderView::section { \
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                            stop:0 #FFA07A, stop:1 #FFDAB9); \
                color: #000; \
                padding: 5px; \
                border: 1px solid #FA8072; \
            } \
            QHeaderView { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                            stop:0 #FFB6C1, stop:1 #FFA07A); \
            } \
            QAbstractItemView { \
                selection-background-color: #EE9A00; \
                selection-color: #000; \
            }";
            break;

        case 3: // 少女风格
            styleSheet = "\
            QMainWindow::separator { \
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                            stop:0 #FFB6C1, stop:1 #FFC0CB); \
            } \
            QMainWindow { \
                background-color: #FFF0F5; \
                color: #2F4F4F; \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                            stop:0 #FFB6C1, stop:1 #FFC0CB); \
                color: #2F4F4F; \
            } \
            QTableView { \
                background-color: #FFF0F5; \
                color: #2F4F4F; \
                border: 1px solid #FFB6C1; \
            } \
            QHeaderView::section { \
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                            stop:0 #FFB6C1, stop:1 #FFC0CB); \
                color: #2F4F4F; \
                padding: 5px; \
                border: 1px solid #FF69B4; \
            } \
            QHeaderView { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                            stop:0 #FF69B4, stop:1 #FFB6C1); \
            } \
            QAbstractItemView { \
                selection-background-color: #FF69B4; \
                selection-color: #FFF; \
            }";
            break;

        case 4: // 湛蓝
            styleSheet = "\
            QMainWindow { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #E3F2FD, stop:1 #BBDEFB); \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                            stop:0 #BBDEFB, stop:1 #90CAF9); \
                color: #1A237E; \
            } \
            QTableView { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #E1F5FE, stop:1 #C5E1FB); \
                color: #1A237E; \
                border: 1px solid #B3E5FC; \
            } \
            QHeaderView::section { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #B3E5FC, stop:1 #81D4FA); \
                color: #1A237E; \
                padding: 5px; \
                border: 1px solid #81D4FA; \
            } \
            QHeaderView { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #90CAF9, stop:1 #B3E5FC); \
            } \
            QAbstractItemView { \
                selection-background-color: #4FC3F7; \
                selection-color: #FFFFFF; \
            }";
            break;

        case 5: // 自然
            styleSheet = "\
            QMainWindow { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #FFF8DC, stop:1 #FEF9E7); \
            } \
            QMenuBar, QMenu, QToolBar, QStatusBar, QComboBox { \
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                            stop:0 #FEF9E7, stop:1 #FFFDE7); \
                color: #556B2F; \
            } \
            QTableView { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #FFFFF0, stop:1 #FEFCE5); \
                color: #6B8E23; \
                border: 1px solid #F5F5DC; \
            } \
            QHeaderView::section { \
                background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                                                  stop:0 #F0FFF0, stop:1 #E8F5E9); \
                color: #8FBC8F; \
                padding: 4px; \
                border: 1px solid #8FBC8F; \
            } \
            QHeaderView { \
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, \
                                                  stop:0 #E8F5E9, stop:1 #F0FFF0); \
            } \
            QAbstractItemView { \
                selection-background-color: #9ACD32; \
                selection-color: #000; \
            }";
            break;
    }

    this->setStyleSheet(styleSheet);
}

