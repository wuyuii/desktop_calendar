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
#include <QException> // 异常类的头文件
#include <QDebug>

const QString plan::DELIMITER  = "|";

plan::plan(){}

plan::plan(size_t id , QString title, QString location, QString information, QDateTime time)
        : id(id), title(std::move(title)), location(std::move(location)), information(std::move(information)), time(std::move(time))
        {
    delete_mask = 0;
}

QString plan::to_string() const
{
    return time.toString( (delete_mask ? '1' : '0') + transcoding(title) + DELIMITER + transcoding(location) + DELIMITER + transcoding(information) + DELIMITER + "yyyy-MM-dd hh:mm") + "\n";
}

void plan::update(const QString& filename, qint64 offset)
{
    QFile file(filename); // 创建文件对象
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) // 以读写文本方式打开文件，检查是否成功打开文件
    {
        qDebug() << "Failed to open file: " << file.errorString(); // 输出错误信息
        return; // 返回，不进行后续操作
    }

    QTextStream out(&file); // 创建文本流对象，用于写入文件

    // 删除原数据行
    file.seek(offset); // 定位到指定偏移量处
    file.write(QString("").toUtf8()); // 将空字符串写入指定位置，相当于删除原数据行

    // 在删除的行处插入新数据行
    file.seek(offset); // 重新定位到相同的偏移量处
    out << this->to_string(); // 将当前计划对象的字符串表示写入文件

    file.close(); // 关闭文件
}
//删除日程
void plan::drop(const QString &filename, qint64 line_num)
{
    delete_mask = 1;
    qint64 offset = seek_offset(filename,line_num);
    QFile file(filename); // 创建文件对象
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) // 以读写文本方式打开文件，检查是否成功打开文件
    {
        qDebug() << "Failed to open file: " << file.errorString(); // 输出错误信息
        return; // 返回，不进行后续操作
    }

    QTextStream in(&file);
    QTextStream out(&file); // 创建文本流对象，用于写入文件

    // 在行首插入废弃符号表示废弃，在程序启动时删除有废弃符号的行
    file.seek(offset); // 定位到指定偏移量处
    // 此处缺少实际写入废弃符号的代码
    // 读取并修改下一个字符
    QString line = in.readLine(); // 读取一行内容
    // 修改第一个字符为 '1'
    if (!line.isEmpty()) {
        line[0] = '1';
    }
    // 将修改后的字符写回文件
    file.seek(offset); // 重新定位到相同的位置
    out << line;

    file.close(); // 关闭文件
}
//查找偏移量
qint64 plan::seek_offset(const QString& filename, qint64 line_num)
{

    QFile file(filename); // 创建文件对象
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) // 以读写文本方式打开文件，检查是否成功打开文件
    {
        qDebug() << "Failed to open file: " << file.errorString(); // 抛出异常
        return -1;
    }

    QTextStream in(&file); // 创建文本流对象，用于从文件中读取数据

    // 跳过前面的行
    for (int i = 1; i < line_num; ++i)
    {
        if (in.atEnd()) // 如果已经到达文件末尾
        {
            throw std::out_of_range("行数超出文件实际行数");
        }
        in.readLine(); // 读取并丢弃当前行数据
    }

    // 定位到要修改的行并更新数据
    QString line = in.readLine(); // 读取当前行数据
    qint64 offset = file.pos() - line.length() - 1; // 计算当前行的偏移量（行首）
    file.close(); // 关闭文件
    return offset; // 返回行首偏移量
}
//修改日程
void plan::modify(const QString& filename, qint64 line_num)
{
    qint64 offset = seek_offset(filename,line_num);

    QFile file(filename); // 创建文件对象
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) // 以读写文本方式打开文件，检查是否成功打开文件
    {
        qDebug() << "Failed to open file: " << file.errorString(); // 输出错误信息
        return; // 返回，不进行后续操作
    }

    QTextStream in(&file);
    QTextStream out(&file); // 创建文本流对象，用于写入文件
    file.seek(offset); // 定位到指定偏移量处
    file.write(QString("").toUtf8()); // 将空字符串写入指定位置，相当于删除原数据行
    file.seek(offset); // 重新定位到相同的位置
    out << this->to_string();

    file.close();
}
//写入日程
bool plan::write(const QString &filename) const
{
    std::cout << filename.toStdString() << "\n";
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
