#include "allplan.h"
#include "ui_allplan.h"
#include <QVector>
#include "plan.h"
#include<QStandardItemModel>
#include<QDebug>
Allplan::Allplan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Allplan)
{
    ui->setupUi(this);
    resize(1200,800);

}

Allplan::~Allplan()
{
    delete ui;
}
void Allplan::displays_plans(QVector<plan> plans, QVector<int> &sortplants){
    // 判断是否有计划
    if (sortplants.empty()) {
        // 如果没有计划，显示"当前还有没要完成的日程"
        QStandardItemModel * model = new QStandardItemModel(1, 1, this);
        model->setItem(0, 0, new QStandardItem("当前没有日程"));
         ui->plan_tableView->setModel(model);
    }
    else{
       // 创建一个表格模型
       QStandardItemModel *model = new QStandardItemModel(sortplants.size(), 4, this);

       // 设置表头
       model->setHorizontalHeaderLabels(QStringList() << "Time" << "Information" << "Location" << "Title");

       // 逐个将计划添加到表格中
       for (int i = 0; i < sortplants.size(); ++i) {
           int id = sortplants[i];
           plan myplan = plans[id - 1]; // id 从 1 开始，所以需要减去 1 获取正确的索引
           model->setItem(i, 0, new QStandardItem(myplan.time.toString()));
           model->setItem(i, 1, new QStandardItem(myplan.information));
           model->setItem(i, 2, new QStandardItem(myplan.location));
           model->setItem(i, 3, new QStandardItem(myplan.title));
       }

       // 将模型设置给表格视图
       ui->plan_tableView->setModel(model);
       // 设置表头大小策略
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // 时间列固定大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 信息列自动适应内容大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 地点列自动适应内容大小
       ui->plan_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // 标题列自动适应内容大小
}

}
