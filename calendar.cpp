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
#include <QPixmap>
#include<QStandardItemModel>
#include <QCoreApplication>
#include <QDir>
#include<QScrollBar>

calendar::calendar(QWidget *parent) : QMainWindow(parent), ui(new Ui::calendar) {
    ui->setupUi(this);
    setFixedSize(1400, 731);
    setWindowIcon(QIcon(":/image/res/favicon.ico"));
    // 设置窗口背景图片
    QPixmap background(":/image/res/background3.png");
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(background));

    this->setPalette(palette);

    cur_date = QDate::currentDate();
    selected_date = QDate::currentDate();
    plans = QVector<plan>();

    connect(ui->pre_month_pushButton, &QPushButton::clicked, this, [=]() {
        date_clicked(selected_date.addMonths(-1));
    });
    connect(ui->next_month_pushButton, &QPushButton::clicked, this, [=]() {
        date_clicked(selected_date.addMonths(1));
    });
    clear_calendar();

    // 将年月日设置到data_label
    ui->date_label->setText(QString(" %1 月 %2 日").arg(selected_date.month()).arg(selected_date.day()));
    updateTimeLabel();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &calendar::updateTimeLabel);
    timer->start(1000); // 每秒更新一次时间
    show_calendar();
    connect(ui->all_plans, &QPushButton::clicked, this, &calendar::display_all_plans);

    timer_check_event = new QTimer(this);

    // 连接定时器的timeout()信号到槽函数
    connect(timer_check_event, &QTimer::timeout, this, &calendar::remind_window);


    // 设置定时器间隔为1分钟
    timer_check_event->start(40000);

    int padding = 10; // 设置内边距大小
    ui->gridLayoutWidget->setContentsMargins(padding, padding, padding, padding);
    connect(this->ui->search_day_Button, &QPushButton::clicked, this, &calendar::onSearchDayPushButtonClicked);
    //创建日程
    QDir::setCurrent(QCoreApplication::applicationDirPath() + "/..");
    default_plan_filename = "plan.txt";//设置日程目录

    connect(ui->add_plan_pushbutton, &QPushButton::clicked, this, &calendar::closest_to_the_event);
    read_plan_file();//读取日程文件
    remind_window();
    connect(this->ui->delete_Button, &QPushButton::clicked, this, [=]() {
        QModelIndexList selectedIndexes = ui->day_plans->selectionModel()->selectedRows();
        if (!selectedIndexes.isEmpty()) {
            deletePlan(selectedIndexes.first().row());
        }
    });
    connect(this->ui->refresh_Button, &QPushButton::clicked, this, [=]() {
        show_calendar();
    });
    ui->refresh_Button->click();
}


void calendar::display_all_plans() {
    allplans_window = new Allplan;
    sortPlans(plans, sort_plans);
    allplans_window->displays_plans(plans, sort_plans);
    allplans_window->show();
}

void calendar::remind_window() {
    sortPlans(plans, sort_plans);

    if (sort_plans.empty()) {
        return;
    }

    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();

    // 遍历排序后的计划数组
    for (int planIndex: sort_plans) {
        // 确保 plans 的索引有效
        if (planIndex >= 1 && planIndex <= plans.size()) {
            // 获取对应的 plan 对象
            const plan &eventPlan = plans[planIndex - 1];


            // 比较计划的时间和当前时间的年月日时分是否一样
            if (eventPlan.time.date() == currentTime.date() &&
                eventPlan.time.toString("hh:mm") == currentTime.toString("hh:mm")) {
                // 将 plan 对象中的信息提取出来，放到提醒事件的窗口中

                QSound::play(":/music/res/music_plus.wav");
                QString title = eventPlan.title;
                QString location = eventPlan.location;
                QString information = eventPlan.information;
                QDateTime time = eventPlan.time;
                //  qDebug() << "asdasdasd";

                // 创建提醒事件的窗口
                QMessageBox::information(this, "您有代办事情",
                                         QString("Title: %1\nLocation: %2\nInformation: %3\nTime: %4").arg(title).arg(
                                                 location).arg(information).arg(time.toString()));
                break;  // 找到匹配的计划后跳出循环
            }
        }
    }
}


void calendar::closest_to_the_event() {
    QDialog dialog(this);
    dialog.setWindowTitle("添加日程");
    dialog.setFixedSize(400, 400); // 调整对话框大小

    // 设置对话框背景图片
    QPixmap bgImage(":/image/res/background1.jpg"); // 替换路径为实际图片路径
    QPalette palette;
    palette.setBrush(QPalette::Background, bgImage);
    dialog.setPalette(palette);

    // 创建标签和文本框以输入标题
    auto *titleLabel = new QLabel("标题:", &dialog);
    titleLabel->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体加粗
    auto *titleEdit = new QLineEdit(&dialog);
    titleEdit->setStyleSheet(
            "background-color: #f0f0f0; color: black; border: 1px solid black; border-radius: 10px;"); // 设置文本框背景色、边框、字体颜色和边角为圆角

    // 创建标签和文本框以输入地点
    auto *location_label = new QLabel("地点:", &dialog);
    location_label->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体加粗
    auto *location_edit = new QLineEdit(&dialog);
    location_edit->setStyleSheet(
            "background-color: #f0f0f0; color: black; border: 1px solid black; border-radius: 10px;"); // 设置文本框背景色、边框、字体颜色和边角为圆角

    // 创建详细信息和文本框以输入详情
    auto *information_label = new QLabel("详细信息:", &dialog);
    information_label->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体加粗
    auto *information_edit = new QTextEdit(&dialog); // 使用 QTextEdit 替代 QLineEdit
    information_edit->setFixedHeight(80); // 调整文本框的高度
    information_edit->setStyleSheet(
            "background-color: #f0f0f0; color: black; border: 1px solid black; border-radius: 10px;"); // 设置文本框背景色、边框、字体颜色和边角为圆角
    information_edit->setTextInteractionFlags(Qt::TextEditorInteraction); // 设置文本框从第一行开始输入

    // 创建标签和日期时间编辑器以输入时间
    auto *timeLabel = new QLabel("时间:", &dialog);
    timeLabel->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体加粗
    auto *timeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog); // 设置默认时间为当前时间

    // 创建保存和取消按钮
    auto *saveButton = new QPushButton("保存", &dialog);
    auto *cancelButton = new QPushButton("取消", &dialog);

    // 设置按钮样式
    saveButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px 16px;");
    cancelButton->setStyleSheet("background-color: #f44336; color: white; padding: 8px 16px;");

    // 连接保存按钮的点击信号到 lambda 表达式
    connect(saveButton, &QPushButton::clicked, [&]() {

        QString title = titleEdit->text(); // 获取标题文本
        QString location = location_edit->text(); // 获取地点文本
        QString information = information_edit->toPlainText(); // 获取详细信息文本
        QDateTime time = timeEdit->dateTime(); // 获取日期时间

        plan cur_plan(plans.size() + 1, title, location, information, time);

        plans.append(cur_plan);

        if (!cur_plan.write(default_plan_filename)) {
            // 如果无法打开文件，则显示错误消息框
            QMessageBox::critical(&dialog, "错误", "无法打开文件进行写入.");
        }
        dialog.close(); // 关闭对话框
        ui->refresh_Button->click();
    });

    // 连接取消按钮的点击信号到对话框的关闭槽函数
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::close);

    // 创建布局并将各个部件添加到布局中
    auto *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(10); // 调整部件之间的间距
    layout->setContentsMargins(50, 50, 50, 50); // 设置主边框与部件之间的距离
    layout->addWidget(titleLabel);
    layout->addWidget(titleEdit);
    layout->addWidget(location_label);
    layout->addWidget(location_edit);
    layout->addWidget(information_label);
    layout->addWidget(information_edit);
    layout->addWidget(timeLabel);
    layout->addWidget(timeEdit);

    // 创建水平布局来放置按钮
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    dialog.setLayout(layout); // 设置对话框的布局

    dialog.exec(); // 显示对话框

}

// 更新时间标签的槽函数
void calendar::updateTimeLabel() {
    // 获取当前时间
    QTime cur_time = QTime::currentTime();
    int hour = cur_time.hour();
    int minute = cur_time.minute();

    // 将小时和分钟设置到time_label
    QString time_str = QString("%1:%2").arg(hour, 2, 10, QLatin1Char('0')).arg(minute, 2, 10, QLatin1Char('0'));

    ui->time_label->setText(time_str);
}

void calendar::clear_calendar() {
    // 清除之前创建的按钮
    QLayoutItem *item;
    while ((item = ui->gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();//返回相关联的部件
        delete item;
    }
}

/**
 * 显示日历
 */
void calendar::show_calendar() {
    // 清除之前创建的按钮
    clear_calendar();
    QStringList weekdayLabels = {"一", "二", "三", "四", "五", "六", "日"};
    for (int i = 0; i < 7; ++i) {
        auto *weekdayLabel = new QLabel(weekdayLabels[i], this);
        weekdayLabel->setAlignment(Qt::AlignCenter); // 居中对齐
        weekdayLabel->setFixedHeight(20); // 设置固定高度
        ui->gridLayout->addWidget(weekdayLabel, 0, i);
    }

    ui->date_label->setText(QString(" %1 月 %2 日").arg(selected_date.month()).arg(selected_date.day()));

    QDate first_day_of_month = selected_date.addDays(-selected_date.day() + 1);//得到本月第一天的月、日
    int start_day = first_day_of_month.dayOfWeek(); // 获取当前月份的第一天是星期几
    int prev_month_days = start_day - 1;//本月有多少天不是本月里面的
    QDate tmp_date = selected_date.addDays(-selected_date.day() + 1 - prev_month_days);// 日历格中的第一个
    for (int row = 1; row < 7; ++row) {
        for (int column = 0; column < 7; ++column) {
            auto *button = new QPushButton(this);
            connect(button, &QPushButton::clicked, this, [=]() {
                date_clicked(tmp_date);
            });
            ui->gridLayout->addWidget(button, row, column);

            auto lunar = new Lunar();
            LunarObj *obj = lunar->solar2lunar(tmp_date.year(), tmp_date.month(), tmp_date.day());

            // 个性化日期显示效果
            button->setText(QString::number(tmp_date.day()) + "\n" + QString::fromStdString(obj->lunarDayChineseName));

            button->setFixedSize(75, 75); // 按钮大小根据网格大小设定

            QString styleSheet = "QPushButton {"
                                 "    border-radius: 36px;" // 圆角半径固定为按钮高度的一半
                                 "    padding: 10px;" // 内边距固定为10px
                                 "}";
            // 非本月日期样式
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

    ui->selected_date_label->setText(QString("%1 年 %2 月 ").arg(selected_date.year()).arg(selected_date.month()));
    // 调用 sortPlans 函数对日程进行排序
    sortPlans(plans, sort_plans);
    // 调用 show_day_plans 函数显示当天的日程
    show_day_plans(plans, sort_plans, selected_date);
}


void calendar::date_clicked(const QDate &date) {
    selected_date = date;
    ui->refresh_Button->click();
    show_day_plans(plans, sort_plans, selected_date);

}
//检查查找范围
bool calendar::validateDate(const QDate& date) {
    // 检查月份范围
    if (date.month() < 1 || date.month() > 12) {
        QMessageBox::warning(this, tr("无效日期"), tr("月份应在 1 到 12 之间。"));
        return false;
    }
    // 检查天数范围
    if (date.day() < 1 || date.day() > date.daysInMonth()) {
        QMessageBox::warning(this, tr("无效日期"), tr("所选月份的日期超出范围。"));
        return false;
    }
    // 可选：检查年份范围
    if (date.year() < 1900 || date.year() > QDate::currentDate().year()) {
        QMessageBox::warning(this, tr("无效日期"), tr("年份应在 1900 年到当前年份之间。"));
        return false;
    }
    return true;
}
//跳转日期
void calendar::onSearchDayPushButtonClicked() {
    QDateDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate selectedDate = dialog.selectedDate();

        // 验证日期是否有效
        if (!validateDate(selectedDate))
            return;

        date_clicked(selectedDate);
    }
}

void calendar::read_plan_file() {
    // 打开文件
    QFile file(default_plan_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QVector<QString> lines;

    // 读取文件内容
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty()) {
            break;
        }
        if (line.at(0) != '1') {
            lines.push_back(line);
        }
    }

    file.close(); // 关闭文件

    for (auto &line: lines) {
        QStringList parts = line.split(QRegularExpression(R"((?<!\\)\|)"), QString::KeepEmptyParts);

        for (auto &item: parts) {
            item.replace("\\|", "|");
        }

        plan p(plans.size() + 1, parts[0].remove(0, 1), parts[1], parts[2],
               QDateTime::fromString(parts[3], "yyyy-MM-dd hh:mm"));

        plans.append(p);
    }

    // 清空文件内容
    if (!file.resize(0)) {
        return;
    }

    // 将 lines 中的内容写回文件中
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    for (const QString &line: lines) {
        out << line << "\n";
    }
    file.close(); // 关闭文件
}

void calendar::sortPlans(QVector<plan> curPlans, QVector<int> &sortplants) {

    // 清空sortplants以防万一里面有旧数据
    sortplants.clear();
    // 创建一个索引数组，初试状态下为0, 1, 2, ... n
    QVector<int> indices(curPlans.size());
    std::iota(indices.begin(), indices.end(), 0); // 需要#include <numeric>

    // 使用std::sort和自定义比较函数来排序索引
    std::sort(indices.begin(), indices.end(), [&](int i1, int i2) {
        return comparePlans(curPlans[i1], curPlans[i2]);
    });
    // 把排序后的索引（需要加1，因为id从1开始）存入sortplants
    for (int idx: indices) {
        if (curPlans[idx].time.date() >= QDateTime::currentDateTime().date() ||
            (curPlans[idx].time.date() == QDateTime::currentDateTime().date() &&
             curPlans[idx].time.time().hour() * 60 + curPlans[idx].time.time().minute() >=
             QDateTime::currentDateTime().time().hour() * 60 + QDateTime::currentDateTime().time().minute())) {
            sortplants.push_back(idx + 1); // 下标加1变为id
        }
    }
}

// 在 calendar 类中添加一个方法来显示日程表
void
calendar::show_day_plans(const QVector<plan> &curPlans, const QVector<int> &sortplants, const QDate &selectedDate) {
    // 创建一个表格模型用于显示日程信息
    auto *model = new QStandardItemModel(0, 4, this); // 设置初始行数为0

    // 设置表头
    model->setHorizontalHeaderLabels(QStringList() << "时间" << "标题" << "地点" << "信息");

    // 遍历所有计划，筛选出当天的计划并添加到表格模型中
    for (int id: sortplants) {
        const plan &myplan = curPlans[id - 1]; // id 从 1 开始，所以需要减去 1 获取正确的索引

        // 检查计划是否是选中日期的
        if (myplan.time.date() == selectedDate) {
            // 在表格模型中添加该日程
            int row = model->rowCount(); // 获取当前行数
            model->insertRow(row); // 插入新行
            QString formattedTime = myplan.time.toString("dddd yyyy-MM-dd HH:mm"); // 使用指定格式显示时间，包括星期几
            // 添加日程信息到对应的单元格
            model->setItem(row, 0, new QStandardItem(formattedTime));
            model->setItem(row, 1, new QStandardItem(myplan.title));
            model->setItem(row, 2, new QStandardItem(myplan.location));
            model->setItem(row, 3, new QStandardItem(myplan.information));
            auto *delete_Button = new QPushButton("Delete");
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

    ui->day_plans->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{background:transparent;" //垂直滑块整体/背景透明
                                                      "width:20px;padding-left:14px;padding-right:1px;}"    //左/右预留位置
                                                      "QScrollBar::handle:vertical{"//滑块样式
                                                      "background:#929292;"  //滑块颜色
                                                      "border-radius:2px;"   //边角圆润
                                                      "min-height:50px;}"    //滑块最小高度
                                                      "QScrollBar::handle:vertical:hover{background:#929292;}"//鼠标触及滑块样式/滑块颜色
                                                      "QScrollBar::add-line:vertical{border:none;}"//向下箭头样式
                                                      "QScrollBar::sub-line:vertical{border:none;}"//向上箭头样式
                                                      "QScrollBar::add-page:vertical{background: #C9C9C9;border-radius:2px;}"//滑块所在垂直区域
                                                      "QScrollBar::sub-page:vertical{background: #C9C9C9;border-radius:2px;}");//滑块所在区域

    ui->day_plans->setStyleSheet(
            "QTableView#device_author_tableview{background: transparent;border: 0px solid red;color: black;"
            "alternate-background-color: rgb(141, 163, 215);"
            "selection-background-color: #D7D7D7;}"//选中区域的背景色
            "QTableView#device_author_tableview::item{background:#E7E7E7;border:0px solid green;}"
            "QTableView#device_author_tableview::item:selected{background:#D7D7D7;border:0px solid green;}"
            "QTableView#device_author_tableview::item:hover{background:#D7D7D7;}");
    QString styleSheet = "QTableView {"
                         "    border-radius: 10px;" // 设置圆角
                         "    border: 2px solid #aaaaaa;" // 设置边框
                         "    border: none;" // 取消边框
                         "}"
                         "QTableView::item:selected {"
                         "    background-color: #a0c6e3;" // 设置选中行的背景色

                         "}";

    ui->day_plans->setStyleSheet(styleSheet);
    // 添加交互效果
    ui->day_plans->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置选中整行
    //ui->day_plans->setSelectionMode(QAbstractItemView::SingleSelection); // 设置单选模式
    ui->day_plans->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑

}

void calendar::deletePlan(int row) {
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
        // 更新 sort_plans 数组中大于被删除索引的值
        for (int &sort_plan: sort_plans) {
            if (sort_plan > planIndex) {
                sort_plan--;
            }
        }

        // 更新文件内容
        QString filename = default_plan_filename;
        QFile file(filename);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            return;
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.isEmpty()) {
                break;
            }
            lines.append(line);
        }

        file.resize(0); // 清空文件内容

        // 修改被删除日程的首行
        if (planIndex - 1 < lines.size()) {
            lines[planIndex - 1].replace(0, 1, '1');
        }

        QTextStream out(&file);
        for (const QString &line: lines) {
            out << line << "\n";
        }

        file.close();

        // 重新显示日程
        show_day_plans(plans, sort_plans, selected_date);
    }
}


calendar::~calendar() {
    delete ui;
}


