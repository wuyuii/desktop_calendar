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

    //创建日程
    default_plan_filename = "plan.TXT";
    connect(ui->add_plan_pushbutton, &QPushButton::clicked, this, &calendar::closest_to_the_event);
    read_plan_file();//读取日程文件

}


void calendar::closest_to_the_event(){
    //后期改成去读map，现在由于map没有重载大小无法读取
//    QDateTime currentDateTime = QDateTime::currentDateTime();

//    QDateTime time = QDateTime::currentDateTime();

//    plan myplan("考试","公交", time);
//    qDebug()<<myplan.time.toString("yyyy-MM-dd HH:mm")<<"xxxxx"<<currentDateTime.toString("yyyy-MM-dd HH:mm");
//   if (myplan.time.toString("yyyy-MM-dd HH:mm") == currentDateTime.toString("yyyy-MM-dd HH:mm"))
//   {

//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::information(this,
//                                         tr("事件提醒！"),
//                                        myplan.title,
//                                         QMessageBox::Ok | QMessageBox::Cancel,
//                                         QMessageBox::Ok);
//   }
    // 创建一个对话框窗口
     QDialog dialog(this);
     dialog.setWindowTitle("添加日程");
     dialog.setFixedSize(500, 600);

     // 创建标签和文本框以输入标题
     QLabel *titleLabel = new QLabel("标题:", &dialog);
     QLineEdit *titleEdit = new QLineEdit(&dialog);

     // 创建标签和文本框以输入地点
     QLabel *location_label = new QLabel("地点:", &dialog);
     QLineEdit *location_edit = new QLineEdit(&dialog);

     // 创建详细信息和文本框以输入详情
     QLabel *information_label = new QLabel("详细信息:", &dialog);
     QLineEdit *information_edit = new QLineEdit(&dialog);
     information_edit->setFixedSize(480,100); // 设置文本框的宽度为 200

     // 创建标签和日期时间编辑器以输入时间
     QLabel *timeLabel = new QLabel("时间:", &dialog);
//     QDateTimeEdit *timeEdit = new QDateTimeEdit(&dialog);

     QDateTimeEdit *timeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog); // 设置默认时间为当前时间



     // 创建保存和取消按钮
     QPushButton *saveButton = new QPushButton("保存", &dialog);
     QPushButton *cancelButton = new QPushButton("取消", &dialog);

     // 连接保存按钮的点击信号到 lambda 表达式
     connect(saveButton, &QPushButton::clicked, [&]() {
         QString title = titleEdit->text(); // 获取标题文本
         QString location = location_edit->text(); // 获取地点文本
         QString information = information_edit->text(); // 获取详细信息文本
         QDateTime time = timeEdit->dateTime(); // 获取日期时间

         // 将日程信息格式化为字符串
         QString schedule = time.toString( title + " | " + location + "|" + information + "|" + "yyyy-MM-dd hh:mm") + "\n" ;

         // 打开文件以追加方式写入日程信息
         QFile file(default_plan_filename);
         if (file.open(QIODevice::Append | QIODevice::Text)) {
             QTextStream out(&file);
             out << schedule;
             file.close();
             read_plan_file();//读取更新后的日程信息
         } else {
             // 如果无法打开文件，则显示错误消息框
             QMessageBox::critical(&dialog, "错误", "无法打开文件进行写入.");
         }

         dialog.close(); // 关闭对话框
     });

     // 连接取消按钮的点击信号到对话框的关闭槽函数
     connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::close);

     // 创建布局并将各个部件添加到布局中
     QVBoxLayout *layout = new QVBoxLayout(&dialog);
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
    // 打开文件
        QFile file(default_plan_filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "无法打开文件" << default_plan_filename;
            return;
        }

        // 读取文件内容
        QTextStream in(&file);
        size_t id = 1;
        plan p;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|", QString::SkipEmptyParts);
            if (parts.size() >= 4) {
                p.id = id;
                id++;
                p.title = parts[0].trimmed();
                p.location = parts[1].trimmed();
                p.context = parts[2].trimmed();
                p.time = QDateTime::fromString(parts[3].trimmed(), "yyyy-MM-dd hh:mm");

            }
            plans.append(p);
        }
        // 关闭文件
        file.close();

}

calendar::~calendar()
{
    delete ui;
}


