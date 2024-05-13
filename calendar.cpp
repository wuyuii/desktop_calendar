#include <QPushButton>
#include "calendar.h"
#include "ui_calendar.h"
#include <iostream>
#include<QDebug>
#include<QBitmap>
#include<QGraphicsDropShadowEffect>
#include<QTimer>
#include <QMessageBox>
#include <numeric>
#include<QDateDialog.h>
#include <QSound>
#include "allplan.h"
#include "lib/lunar.h"
#include<QStandardItemModel>
#include <QCoreApplication>
#include <QDir>
calendar::calendar(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::calendar)
{
    ui->setupUi(this);
    cur_date = QDate::currentDate();
    selected_date = QDate::currentDate();
    plans = QVector<plan>();
//   QSound *start_sound=new QSound(":/res/music.wav",this);
    connect(ui->pre_month_pushButton, &QPushButton::clicked, this, [=]() {
        date_clicked(selected_date.addMonths(-1));
    });
    connect(ui->next_month_pushButton, &QPushButton::clicked, this, [=]() {
        date_clicked(selected_date.addMonths(1));
    });
    clear_calendar();
    ui->date_label->setFixedSize(300, 30); // 设置固定大小
    ui->time_label->setFixedSize(300, 30); // 设置固定大小
    // 将年月日设置到data_label
    ui->date_label->setText(QString(" %1 月 %2 日").arg(selected_date.month()).arg(selected_date.day()));

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &calendar::updateTimeLabel);
    timer->start(1000); // 每秒更新一次时间
    show_calendar();
    connect(ui->all_plans, &QPushButton::clicked, this,&calendar::display_all_plans);

    timer_check_event = new QTimer(this);

    // 连接定时器的timeout()信号到槽函数
    connect(timer_check_event, &QTimer::timeout, this, &calendar::remind_window);

    // 设置定时器间隔为1分钟
    timer_check_event->start(60000);


    int padding = 10; // 设置内边距大小
    ui->gridLayoutWidget->setContentsMargins(padding, padding, padding, padding);
    connect(this->ui->search_day_Button, &QPushButton::clicked, this, &calendar::onSearchDayPushButtonClicked);
    //创建日程
    QDir::setCurrent(QCoreApplication::applicationDirPath() + "/..");
    default_plan_filename = "desktop_calendar/resources/plan.txt";//设置日程目录
    connect(ui->add_plan_pushbutton, &QPushButton::clicked, this, &calendar::closest_to_the_event);
    read_plan_file();//读取日程文件
    // 创建几个 plan 对象并添加到 plans 容器中
    plans.push_back(plan(1, "Meeting", "Conference Room", "Discuss project timeline", QDateTime(QDate(2024, 4, 13), QTime(20, 21))));
    plans.push_back(plan(2, "Lunch", "Cafeteria", "Meet with colleagues", QDateTime(QDate(2025, 6, 5), QTime(12, 0))));
    plans.push_back(plan(3, "健身房", "Fitness Center", "Workout session", QDateTime(QDate(2025, 4, 5), QTime(18, 0))));
   // plans[1].delete_mask=1;
    connect(this->ui->delete_Button, &QPushButton::clicked, this, [=]() {
            QModelIndexList selectedIndexes = ui->day_plans->selectionModel()->selectedRows();
            if (!selectedIndexes.isEmpty()) {
                deletePlan(selectedIndexes.first().row());
            }
        });
    connect(this->ui->refresh_Button,&QPushButton::clicked,this,[=]()
    {
       show_calendar();
    });
    remind_window();
}


void calendar::display_all_plans(){
    allplans_window=new Allplan;
    sortPlans( plans, sort_plans);
   // qDebug()<<sort_plans.size();
    allplans_window->displays_plans( plans, sort_plans);
    allplans_window->show();
}

void calendar::remind_window(){
     sortPlans( plans, sort_plans);
if(sort_plans.size()==0){
    return;
}
    // 获取 sort_plans 中第一个元素的值
    int planIndex = sort_plans[0];
    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    // 确保 plans 的索引有效
    if(planIndex >= 1 && planIndex <= plans.size()) {
        // 获取对应的 plan 对象
    const plan& eventPlan = plans[planIndex - 1];
        // 比较计划的时间和当前时间的年月日时分是否一样
    if(eventPlan.time.date() == currentTime.date() && eventPlan.time.toString("hh:mm") == currentTime.toString("hh:mm")) {
            // 将 plan 对象中的信息提取出来，放到提醒事件的窗口中
        QSound* start_sound = new QSound(":/res/music.wav", this);
                    start_sound->play();
            QString title = eventPlan.title;
            QString location = eventPlan.location;
            QString information = eventPlan.information;
            QDateTime time = eventPlan.time;

            // 创建提醒事件的窗口
            QMessageBox::information(this, "您有代办事情", QString("Title: %1\nLocation: %2\nInformation: %3\nTime: %4")
                                     .arg(title).arg(location).arg(information).arg(time.toString()));
        }
    }
}

void calendar::closest_to_the_event()
{
    // 创建一个对话框窗口
    QDialog dialog(this);
    dialog.setWindowTitle("添加日程");
    dialog.setFixedSize(500, 600);

    // 创建标签和文本框以输入标题
    auto *titleLabel = new QLabel("标题:", &dialog);
    auto *titleEdit = new QLineEdit(&dialog);

    // 创建标签和文本框以输入地点
    auto *location_label = new QLabel("地点:", &dialog);
    auto *location_edit = new QLineEdit(&dialog);

    // 创建详细信息和文本框以输入详情
    auto *information_label = new QLabel("详细信息:", &dialog);
    auto *information_edit = new QLineEdit(&dialog);
    information_edit->setFixedSize(480, 100); // 设置文本框的宽度为 200

    // 创建标签和日期时间编辑器以输入时间
    auto *timeLabel = new QLabel("时间:", &dialog);

    auto *timeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog); // 设置默认时间为当前时间

    // 创建保存和取消按钮
    auto *saveButton = new QPushButton("保存", &dialog);
    auto *cancelButton = new QPushButton("取消", &dialog);

    // 连接保存按钮的点击信号到 lambda 表达式
    connect(saveButton, &QPushButton::clicked, [&]() {

        QString title = titleEdit->text(); // 获取标题文本
        QString location = location_edit->text(); // 获取地点文本
        QString information = information_edit->text(); // 获取详细信息文本
        QDateTime time = timeEdit->dateTime(); // 获取日期时间

        plan cur_plan(plans.size() + 1, title, location, information, time);

        plans.append(cur_plan);

        if (!cur_plan.write(default_plan_filename)) {
            // 如果无法打开文件，则显示错误消息框
            QMessageBox::critical(&dialog, "错误", "无法打开文件进行写入.");
        }
        dialog.close(); // 关闭对话框
    });

    // 连接取消按钮的点击信号到对话框的关闭槽函数
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::close);

    // 创建布局并将各个部件添加到布局中
    auto *layout = new QVBoxLayout(&dialog);
    layout->addWidget(titleLabel);
    layout->addWidget(titleEdit);
    layout->addWidget(location_label);
    layout->addWidget(location_edit);
    layout->addWidget(information_label);
    layout->addWidget(information_edit);
    layout->addWidget(timeLabel);
    layout->addWidget(timeEdit);
    layout->addWidget(saveButton);
    layout->addWidget(cancelButton);

    dialog.setLayout(layout); // 设置对话框的布局
    dialog.exec(); // 显示对话框
}

// 更新时间标签的槽函数
void calendar::updateTimeLabel()
{
    // 获取当前时间
    QTime cur_time = QTime::currentTime();
    int hour = cur_time.hour();
    int minute = cur_time.minute();

    // 将小时和分钟设置到time_label
    QString time_str = QString("%1:%2").arg(hour, 2, 10, QLatin1Char('0')).arg(minute, 2, 10, QLatin1Char('0'));

    ui->time_label->setText(time_str);
}

void calendar::clear_calendar()
{
    // 清除之前创建的按钮
    QLayoutItem *item;
    while ((item = ui->gridLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();//返回相关联的部件
        delete item;
    }
}

/**
 * 显示日历
 */
void calendar::show_calendar()
{
    // 清除之前创建的按钮
    clear_calendar();
    QStringList weekdayLabels = {"一", "二", "三", "四", "五", "六", "日"};
    for (int i = 0; i < 7; ++i) {
        QLabel *weekdayLabel = new QLabel(weekdayLabels[i], this);
        weekdayLabel->setAlignment(Qt::AlignCenter); // 居中对齐
        weekdayLabel->setFixedHeight(20); // 设置固定高度
        ui->gridLayout->addWidget(weekdayLabel, 0, i);
    }
    QDate first_day_of_month = selected_date.addDays(-selected_date.day() + 1);//得到本月第一天的月、日
    int start_day = first_day_of_month.dayOfWeek(); // 获取当前月份的第一天是星期几
    int prev_month_days = start_day - 1;//本月有多少天不是本月里面的
    QDate tmp_date = selected_date.addDays(-selected_date.day() + 1 - prev_month_days);// 日历格中的第一个
    for (int row = 1; row < 7; ++row)
    {
        for (int column = 0; column < 7; ++column)
        {

            auto *button = new QPushButton(this);
            connect(button, &QPushButton::clicked, this, [=]() {
                date_clicked(tmp_date);
            });
            ui->gridLayout->addWidget(button, row, column);

            auto lunar = new Lunar();
            LunarObj* obj = lunar->solar2lunar(tmp_date.year(), tmp_date.month(), tmp_date.day());

            // 个性化日期显示效果
            button->setText(QString::number(tmp_date.day()) + "\n" + QString::fromStdString(obj->lunarDayChineseName));

            button->setFixedSize(75, 75); // 按钮大小根据网格大小设定

            QString styleSheet = "QPushButton {"
                                 "    border-radius: 36px;" // 圆角半径固定为按钮高度的一半
                                 "    padding: 10px;" // 内边距固定为10px
                                 "}";
            // 非本月日期样式
            if (tmp_date.month() != selected_date.month())
            {
                styleSheet += "QPushButton {"
                              "    border: 1px solid transparent; background-color: #f0f0f0; color: #808080;"
                              "}";
            }

            // 被选中日期样式
            if (selected_date == tmp_date)
            {
                styleSheet += "QPushButton {"
                              "    background-color: transparent; border: 2px solid lightblue; padding: 6px;"
                              "}";
            }

            // 当前日期样式
            if (cur_date == tmp_date)
            {
                styleSheet += "QPushButton {"
                              "    background-color: lightblue; border: 2px lightblue; padding: 6px;"
                              "}";
            }

            button->setStyleSheet(styleSheet);

            tmp_date = tmp_date.addDays(1);
        }
    }

    ui->selected_date_label->setText(QString("%1 年 %2 月 ").arg(selected_date.year()).arg(selected_date.month()));
    // 调用 sortPlans 函数对日程进行排序
    sortPlans(plans, sort_plans);
    // 调用 show_day_plans 函数显示当天的日程
    show_day_plans(plans, sort_plans, selected_date);
}


void calendar::date_clicked(const QDate &date)
{
    selected_date = date;
    show_calendar();

}
//查找某一天
void calendar::onSearchDayPushButtonClicked()
{

    QDateDialog dialog(this); // 假设存在一个名为 QDateDialog 的对话框类
    if (dialog.exec() == QDialog::Accepted) {
        QDate selectedDate = dialog.selectedDate();
        date_clicked(selectedDate); // 调用原来的 date_clicked 函数处理选中日期的逻辑
    }
}
void calendar::read_plan_file()
{
    // 打开文件
    QFile file(default_plan_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QVector<QString> lines;

    // 读取文件内容
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.isEmpty())
        {
            break;
        }
        if (line.at(0) != '1')
        {
            lines.push_back(line);
        }
    }

    file.close(); // 关闭文件

    for (auto &line: lines)
    {
        QStringList parts = line.split(QRegularExpression(R"((?<!\\)\|)"), QString::KeepEmptyParts);

        for (auto &item: parts)
        {
            item.replace("\\|", "|");
        }

        plan p(plans.size() + 1, parts[0].remove(0, 1), parts[1], parts[2],
               QDateTime::fromString(parts[3], "yyyy-MM-dd hh:mm"));

        plans.append(p);
    }

    // 清空文件内容
    if (!file.resize(0))
    {
        return;
    }

    // 将 lines 中的内容写回文件中
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    for (const QString &line: lines)
    {
        out << line << "\n";
    }
    file.close(); // 关闭文件
}

void calendar::sortPlans(QVector<plan> plans, QVector<int> &sortplants){


    // 清空sortplants以防万一里面有旧数据
        sortplants.clear();
        // 获取当前系统时间
            QDateTime currentDateTime = QDateTime::currentDateTime();
        // 创建一个索引数组，初试状态下为0, 1, 2, ... n
        QVector<int> indices(plans.size());
        std::iota(indices.begin(), indices.end(), 0); // 需要#include <numeric>

        // 使用std::sort和自定义比较函数来排序索引
        std::sort(indices.begin(), indices.end(), [&](int i1, int i2) {
            return comparePlans(plans[i1], plans[i2]);
        });
//qDebug()<<indices[0]<<indices[1];
        // 把排序后的索引（需要加1，因为id从1开始）存入sortplants
        for (int idx : indices) {
                if (plans[idx].time.date() >= QDateTime::currentDateTime().date() ||(plans[idx].time.date() == QDateTime::currentDateTime().date()&&
                        plans[idx].time.time().hour() * 60 + plans[idx].time.time().minute() >= QDateTime::currentDateTime().time().hour() * 60
                        + QDateTime::currentDateTime().time().minute() )) {
                    sortplants.push_back(idx + 1); // 下标加1变为id
                    //qDebug()<<idx + 1<<" ";
                }
            }
}
// 在 calendar 类中添加一个方法来显示日程表
void calendar::show_day_plans(const QVector<plan> &plans, const QVector<int> &sortplants, const QDate &selectedDate)
{
    // 创建一个表格模型用于显示日程信息
    QStandardItemModel *model = new QStandardItemModel(0, 4, this); // 设置初始行数为0

    // 设置表头
    model->setHorizontalHeaderLabels(QStringList() << "Time" << "Information" << "Location" << "Title");

    // 遍历所有计划，筛选出当天的计划并添加到表格模型中
    for (int i = 0; i < sortplants.size(); ++i) {
        int id = sortplants[i];
        const plan &myplan = plans[id - 1]; // id 从 1 开始，所以需要减去 1 获取正确的索引

        // 检查计划是否是选中日期的
        if (myplan.time.date() == selectedDate) {
            // 在表格模型中添加该日程
            int row = model->rowCount(); // 获取当前行数
            model->insertRow(row); // 插入新行

            // 添加日程信息到对应的单元格
            model->setItem(row, 0, new QStandardItem(myplan.time.toString()));
            model->setItem(row, 1, new QStandardItem(myplan.information));
            model->setItem(row, 2, new QStandardItem(myplan.location));
            model->setItem(row, 3, new QStandardItem(myplan.title));
            QPushButton *delete_Button = new QPushButton("Delete");
                        connect(delete_Button, &QPushButton::clicked, [=]() {
                            deletePlan(row); // 调用删除日程的槽函数
                        });
        ui->day_plans->setIndexWidget(model->index(row, 4), delete_Button); // 将删除按钮添加到表格中
        }
    }

    // 根据时间排序表格模型中的行
    model->sort(0); // 按第一列（时间）排序

    // 将模型设置给表格视图
    ui->day_plans->setModel(model);
    // 设置表头大小策略
    ui->day_plans->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 时间列固定大小
    ui->day_plans->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 信息列自动适应内容大小
    ui->day_plans->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 地点列自动适应内容大小
    ui->day_plans->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // 标题列自动适应内容大小

    QString styleSheet = "QTableView {"
                         "    background-color: #f0f0f0;" // 设置背景色
                         "    border: 1px solid #ccc;" // 设置边框
                         "}"
                         "QTableView::item {"
                         "    padding: 5px;" // 设置内边距
                         "}"
                         "QTableView::item:selected {"
                         "    background-color: #a0c6e3;" // 设置选中行的背景色
                         "}"
                         "QHeaderView::section {"
                         "    background-color: #d3d3d3;" // 设置表头的背景色
                         "    color: #333;" // 设置表头文字颜色
                         "    padding: 5px;" // 设置表头内边距
                         "}";

    ui->day_plans->setStyleSheet(styleSheet);
    // 添加交互效果
    ui->day_plans->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置选中整行
    ui->day_plans->setSelectionMode(QAbstractItemView::SingleSelection); // 设置单选模式
    ui->day_plans->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑

}
void calendar::deletePlan(int row)
{
    if (row < 0 || row >= ui->day_plans->model()->rowCount()) {
        return;
    }

    // 获取排序后的日程索引
    int planIndex = sort_plans[row];

    if (planIndex >= 1 && planIndex <= plans.size()) {
        // 删除对应的日程
        plans.remove(planIndex - 1);

        // 更新 sort_plans 数组
        sort_plans.remove(row);

        // 重新显示日程
        show_day_plans(plans, sort_plans, selected_date);
    }
}


calendar::~calendar() {
    delete ui;
}


