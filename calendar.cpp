#include <QPushButton>
#include "calendar.h"
#include "ui_calendar.h"
#include <iostream>

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

    show_calendar();
}


void calendar::clear_calendar()
{
    // 清除之前创建的按钮
    QLayoutItem *item;
    while ((item = ui->gridLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
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

    QDate first_day_of_month = selected_date.addDays(-selected_date.day() + 1);
    int start_day = first_day_of_month.dayOfWeek(); // 获取当前月份的第一天是星期几
    int prev_month_days = start_day - 1;
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

            //TODO 个性化日期显示效果
            button->setText(QString::number(tmp_date.day()));

            if(selected_date == tmp_date)
            {    // TODO 如果是被选中的日期
                // 可以在这里进行特殊标记或者高亮等操作
            }

            if(cur_date == tmp_date)
            {
                // TODO 当前日期 效果
            }

            tmp_date = tmp_date.addDays(1);
        }
    }

    ui->selected_date_label->setText(QString("%1 年 %2 月 %3 日").arg(selected_date.year()).arg(selected_date.month()).arg(selected_date.day()));

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


