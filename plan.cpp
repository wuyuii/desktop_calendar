//
// Created by zhengdu on 2024/3/15.
//

#include "plan.h"

#include <utility>
#include <QFile>
#include <QTextStream>

const QString plan::DELIMITER  = "\\|";
const QString plan::ABANDONED_SYMBOL  = "\\~";

plan::plan(QString title, QString location, QDateTime time) : title(std::move(title)), location(std::move(location)),
                                                                                   time(std::move(time)) {}

QString plan::to_string()
{
    QString context = QString(time.toString() + DELIMITER + title + DELIMITER + location + "\n");
    return context;
}

void plan::update(const QString& filename, qint64 offset)
{
    QFile file(filename);
    QTextStream out(&file);

    // 删除原数据行
    file.seek(offset);
    file.write(QString("").toUtf8());

    //在删除的行处插入新数据行
    file.seek(offset);
    out << this->to_string();
}

void plan::drop(const QString &filename, qint64 offset)
{
    QFile file(filename);
    QTextStream out(&file);

    // 在行首插入废弃符号表示废弃，在程序启动时删除有废弃符号的行
    file.seek(offset);
    out << ABANDONED_SYMBOL;
}


qint64 plan::seek_offset(const QString& filename, qint64 line_num)
{

    // TODO 补充异常
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        throw ;
    }

    QTextStream in(&file);

    // 跳过前面的行
    for (int i = 1; i < line_num; ++i)
    {
        if (in.atEnd())
        {
            throw ;
        }
        in.readLine();
    }

    // 定位到要修改的行并更新数据
    QString line = in.readLine();
    qint64 offset = file.pos() - line.length() - 1;// 定位到行首
    file.close();
    return offset;
}

// TODO 在文件末尾追加数据
void plan::write(const QString &filename)
{

}