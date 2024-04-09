#ifndef CALENDAR_H
#define CALENDAR_H

#include <QMainWindow>
#include <QDate>
#include <QSet>
#include <QMap>
#include<QTimer>
#include "plan.h"
#include "allplan.h"
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTimeEdit>
#include <algorithm>
QT_BEGIN_NAMESPACE
namespace Ui { class calendar; }
QT_END_NAMESPACE

class calendar : public QMainWindow
{
    Q_OBJECT

public:
    explicit calendar(QWidget *parent = nullptr);
    ~calendar() override;

    QDate cur_date;// 当前日期
    QDate selected_date;// 选中的日期
    QString default_plan_filename;// 默认日程文件路径
    QTimer *timer;//定时器对象
    QTimer *timer_check_event;//定时器对象
    Allplan *allplans_window;
    QVector<plan> plans;// 下标 + 1 对应 plan里的 id
    QVector<int> sort_plans;
    void show_calendar();

    void clear_calendar();

    void read_plan_file();
    // 比较两个计划时间的函数
    bool comparePlans(const plan &a, const plan &b) {
        return a.time < b.time;
    }
    void onSearchDayPushButtonClicked();//查找
    void sortPlans(QVector<plan> plans, QVector<int> &sortplants);

    //TODO 根据日期分类plan

signals:

private slots:
    void date_clicked(const QDate &date);
    void updateTimeLabel();
    void closest_to_the_event();//测试弹窗用的，后面
    void remind_window();
    void display_all_plans();

private:
    Ui::calendar *ui;
};
#endif // CALENDAR_H
