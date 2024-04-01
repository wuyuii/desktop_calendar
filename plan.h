//
// Created by zhengdu on 2024/3/15.
//

#ifndef CALENDAR_PLAN_H
#define CALENDAR_PLAN_H


#include <QString>
#include <QDateTime>

class plan {

//    // TODO 如果title 或 location 包含 换行符 ，则需要转义 文本中的换行符
//    QString title;
//    QString location;
//    QDateTime time;

public:
    // TODO 如果title 或 location 包含 换行符 ，则需要转义 文本中的换行符


    size_t id; // 从 1 开始自增
    QString title;
    QString location;
    QString context;
    QDateTime time;
    static const QString DELIMITER;
    static const QString ABANDONED_SYMBOL;

    plan();

    plan(size_t id, QString title, QString location, QString context, QDateTime time);

    QString to_string();

    qint64 seek_offset(const QString& filename, qint64 offset);

    void write(const QString& filename);

    void update(const QString& filename, qint64 offset);

    /**
     * 废弃日程
     */
    void drop(const QString& filename, qint64 line_num);
};


#endif //CALENDAR_PLAN_H
