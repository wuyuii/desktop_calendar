//
// Created by zhengdu on 2024/3/15.
//

#include "plan.h"

#include <utility>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <iostream>

const QString plan::DELIMITER  = "|";

plan::plan(){}

plan::plan(size_t id , QString title, QString location, QString information, QDateTime time)
        : id(id), title(std::move(title)), location(std::move(location)), information(std::move(information)), time(std::move(time))
        {
    delete_mask = false;
}

QString plan::to_string() const
{
    return time.toString( (delete_mask ? '1' : '0') + transcoding(title) + DELIMITER + transcoding(location) + DELIMITER + transcoding(information) + DELIMITER + "yyyy-MM-dd hh:mm") + "\n";
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

bool plan::write(const QString &filename) const
{
    // 打开文件以追加方式写入日程信息
    QFile file(filename);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << this->to_string();
        file.close();
        return true;
    }
    return false;
}

QString plan::transcoding(const QString& string) {

    // 对字符串进行转义
    QString escapedString = string;

    escapedString.replace("|", "\\|");

    escapedString.replace("\n"," ");

    return escapedString;
}
