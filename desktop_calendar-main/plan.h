//
// Created by zhengdu on 2024/3/15.
//

#ifndef CALENDAR_PLAN_H
#define CALENDAR_PLAN_H


#include <QString>
#include <QDateTime>

class plan {

public:
    // TODO 如果title 或 location 包含 换行符 ，则需要转义 文本中的换行符

    size_t id; // 从 1 开始自增
    QString title;
    QString location;
    QString information;
    QDateTime time;
    bool delete_mask;// 0 表示 不废弃 ，1 表示 废弃
    static const QString DELIMITER;

    plan();

    plan(size_t id, QString title, QString location, QString information, QDateTime time);

    QString to_string() const;

    qint64 seek_offset(const QString& filename, qint64 offset);

    bool write(const QString& filename) const;

    void update(const QString& filename, qint64 offset);

    /**
     * 废弃日程
     */
    void drop(const QString& filename, qint64 line_num);

    /**
     * 转义分隔符和换行符
     */
    static QString transcoding(const QString& string);
};


#endif //CALENDAR_PLAN_H
