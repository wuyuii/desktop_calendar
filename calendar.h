#ifndef CALENDAR_H
#define CALENDAR_H

#include <QMainWindow>
#include <QDate>
#include <QSet>
#include <QMap>
#include "plan.h"

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

    QMap<plan,qint64> plan_data_index_map; // plan 对应 行号

    void show_calendar();
    void clear_calendar();

    void read_plan_file();

    //TODO 根据日期分类plan

signals:


private slots:
    void date_clicked(const QDate &date);

private:
    Ui::calendar *ui;

};
#endif // CALENDAR_H
