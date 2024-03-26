#include <QPushButton>
#include "calendar.h"
#include "ui_calendar.h"
#include <iostream>
#include<QDebug>
#include<QBitmap>
#include<QGraphicsDropShadowEffect>
#include<QTimer>
#include <QMessageBox>
calendar::calendar(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::calendar)
{
    ui->setupUi(this);
    cur_date = QDate::currentDate();
    selected_date = QDate::currentDate();

    connect(ui->pre_month_pushButton,&QPushButton::clicked, this,[=]() {
        date_clicked(selected_date.addMonths(-1));
    });
    connect(ui->next_month_pushButton,&QPushButton::clicked, this,[=]() {
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


    timer_check_event = new QTimer(this);

   // 连接定时器的timeout()信号到槽函数
   connect(timer_check_event, &QTimer::timeout, this, &calendar::closest_to_the_event);

   // 设置定时器间隔为1分钟
   timer_check_event->start(60000);


    int padding = 10; // 设置内边距大小
    ui->gridLayoutWidget->setContentsMargins(padding, padding, padding, padding);
}


void calendar::closest_to_the_event(){//后期改成去读map，现在由于map没有重载大小无法读取
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QDateTime time = QDateTime::currentDateTime();
    plan myplan("考试","公交", time);
    qDebug()<<myplan.time.toString("yyyy-MM-dd HH:mm")<<"xxxxx"<<currentDateTime.toString("yyyy-MM-dd HH:mm");
   if (myplan.time.toString("yyyy-MM-dd HH:mm") == currentDateTime.toString("yyyy-MM-dd HH:mm")) {

        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this,
                                         tr("事件提醒！"),
                                        myplan.title,
                                         QMessageBox::Ok | QMessageBox::Cancel,
                                         QMessageBox::Ok);

        if (reply == QMessageBox::Ok) {
            // 用户点击了OK按钮,可以弹出相关任务
            // 在这里执行相关操作
        } else {
            // 用户点击了Cancel按钮或者点击了窗口右上角的关闭按钮
            // 在这里执行相关操作
        }
    }



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

    QDate first_day_of_month = selected_date.addDays(-selected_date.day() + 1);//得到本月第一天的月、日
    int start_day = first_day_of_month.dayOfWeek(); // 获取当前月份的第一天是星期几
    int prev_month_days = start_day - 1;//本月有多少天不是本月里面的
    QDate tmp_date = selected_date.addDays(-selected_date.day() + 1 - prev_month_days);// 日历格中的第一个
    for (int row = 0; row < 6; ++row)
    {
        for (int column = 0; column < 7; ++column)
        {
            // TODO 暂时用QPushButton代替，可用其他组件实现个性化


            auto *button = new QPushButton(this);
            connect(button, &QPushButton::clicked, this, [=]() {
                date_clicked(tmp_date);
            });
            ui->gridLayout->addWidget(button, row, column);

            // 个性化日期显示效果
            button->setText(QString::number(tmp_date.day()));
            button->setFixedSize(75, 75); // 按钮大小根据网格大小设定

            QString styleSheet = "QPushButton {"
                                 "    border-radius: 36px;" // 圆角半径固定为按钮高度的一半
                                 "    padding: 10px;" // 内边距固定为20px
                                 "}";
            // 默认样式
            if (tmp_date.month() != selected_date.month()) {
                styleSheet += "QPushButton {"
                              "    border: 1px solid transparent; background-color: #f0f0f0; color: #808080;"
                              "}";
            }

            // 被选中日期样式
            if (selected_date == tmp_date) {
                styleSheet += "QPushButton {"
                              "    background-color: transparent; border: 2px solid lightblue; padding: 6px;"
                              "}";
            }

            // 当前日期样式
            if (cur_date == tmp_date) {
                styleSheet += "QPushButton {"
                              "    background-color: lightblue; border: 2px lightblue; padding: 6px;"
                              "}";
            }

            button->setStyleSheet(styleSheet);

            tmp_date = tmp_date.addDays(1);
        }
    }

   // ui->selected_date_label->setText(QString("%1 年 %2 月 %3 日").arg(selected_date.year()).arg(selected_date.month()).arg(selected_date.day()));
    ui->selected_date_label->setText(QString("%1 年 %2 月 ").arg(selected_date.year()).arg(selected_date.month()));
}


void calendar::date_clicked(const QDate &date)
{
    selected_date = date;
    show_calendar();

}

// TODO 按每行读取plan，删除有废弃符号的行并不读取，并将plan及对应的行号写入planDataIndexMap
void calendar::read_plan_file()
{

}

calendar::~calendar()
{
    delete ui;
}


